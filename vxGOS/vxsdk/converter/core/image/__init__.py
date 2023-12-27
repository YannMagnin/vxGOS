"""
core.conv.type.image - Vhex image converter
"""
import os

from PIL import Image

from core.logger import log
from core.pixel import pixel_rgb24to16

__all__ = [
    'image_generate'
]

#---
# Internals
#---

## Profile handling

def _profile_gen(profile, name, palette=None, alpha=None):
    r""" Internal image profile class

    ================================== ========================================
    Property                           Description
    ================================== ========================================
      id                         (int) profile ID
      names             (array of str) list all profile names
      format                     (str) profile format name (vhex API)
      has_alpha                 (bool) indicate if the profil has alpha
      alpha                      (int) alpha index in the palette (or mask)
      is_indexed                (bool) indicate if it should be indexed
      palette_base               (int) indicate base index for color inserting
      palette_color_count        (int) indicate the number of color (palette)
      palette_trim              (bool) indicate if the palette should be trimed
    ================================== ========================================
    """
    profile = {
        'profile'   : profile,
        'name'      : name,
        'has_alpha' : (alpha is not None),
        'alpha'     : alpha,
        'is_indexed': (palette is not None),
        'palette'   : None
    }
    if palette is not None:
        profile['palette_base']        = palette[0]
        profile['palette_color_count'] = palette[1]
        profile['palette_trim']        = palette[2]
    return profile

# all supported profile information
VX_PROFILES = [
    _profile_gen('IMAGE_RGB565',     "p16"),
    _profile_gen('IMAGE_RGB565A',    "p16a", alpha=0x0001),
    _profile_gen('IMAGE_P8_RGB565',  "p8",  palette=(0,256,True)),
    _profile_gen('IMAGE_P8_RGB565A', "p8a", palette=(1,256,True), alpha=0),
    _profile_gen('IMAGE_P4_RGB565',  "p4",  palette=(0,16,False)),
    _profile_gen('IMAGE_P4_RGB565A', "p4a", palette=(1,16,False), alpha=0),
]

def _profile_find(name):
    """Find a profile by name."""
    for profile in VX_PROFILES:
        if name == profile['name']:
            return profile
    return None

## Image manipulation

def _image_isolate_alpha(info):
    """ Isolate alpha corlor of the image

    Vhex use a particular handling for alpha color and this information should
    use a strict encoding way. Things that Pillow don't do properly. So, lets
    manually setup our alpha isolation and patch Pillow alpha palette handling.

    @args
    > info    (dict) - contains all needed information (image, data, ...)

    @return
    > Nothing
    """
    # fetch needed information
    img = info['img']
    profile = info['profile']

    # Save the alpha channel and make it 1-bit. We need to do this because
    # the alpha value is handled specialy in Vhex and the image conversion
    # to palette-oriented image is weird : the alpha colors is also converted
    # in the palette
    if profile['has_alpha']:
        alpha_channel = img.getchannel("A").convert("1", dither=Image.NONE)
    else:
        alpha_channel = Image.new("1", img.size, 1)

    alpha_pixels = alpha_channel.load()
    img = img.convert("RGB")

    # Transparent pixels have random values on the RGB channels, causing
    # them to use up palette entries during quantization. To avoid that, set
    # their RGB data to a color used somewhere else in the image.
    pixels = img.load()
    bg_color = next(
        (
            pixels[x,y]
            for x in range(img.width)
                for y in range(img.height)
                    if alpha_pixels[x,y] > 0
        ),
        (0,0,0)
    )
    for _y in range(img.height):
        for _x in range(img.width):
            if alpha_pixels[_x, _y] == 0:
                pixels[_x, _y] = bg_color

    # update external information
    info['img'] = img
    info['img_pixel_list_alpha'] = alpha_pixels
    info['img_pixel_list_clean'] = pixels

def _image_encode_palette(info):
    """ Generate palette information

    This routine is involved only if the targeted profile is indexed. We need
    to generate (and isolate) color palette.

    @args
    > info    (dict) - contains all needed information (image, data, ...)

    @return
    > Nothing
    """
    # fetch needed information
    img = info['img']
    profile = info['profile']

    # convert image into palette format
    # note: we remove one color slot in the palette for the alpha one
    color_count = profile['palette_color_count'] - int(profile['has_alpha'])
    img = img.convert(
        'P',
        dither=Image.NONE,
        palette=Image.ADAPTIVE,
        colors=color_count
    )

    # The palette format is a list of N triplets ([r, g, b, ...]). But,
    # sometimes, colors after img.convert() are not numbered 0 to
    # `color_count`, because the palette don't need to be that big. So,
    # we calculate the "palette size" by walking throuth the bitmap and
    # by saving the biggest index used.
    pixels = img.load()
    nb_triplet = 1 + max(
        pixels[x,y]
        for y in range(img.height)
            for x in range(img.width)
    )
    palette = img.getpalette()[:3 * nb_triplet]
    palette = list(zip(palette[::3], palette[1::3], palette[2::3]))

    # For formats with transparency, add an "unused" palette slot which
    # will used has pink/purple in case of a bad application try to use
    # this value anyway
    if profile['has_alpha']:
        palette = [(255, 0, 255)] + palette
        nb_triplet += 1

    # Also keep track of how to remap indices from the values generated
    # by img.convert() into the palette, which is shifted by 1 due to
    # alpha and also starts at profile.palette_base.
    #
    # Note: profile.palette_base already starts 1 value later for
    # formats with alpha.
    palette_map = [
        (profile['palette_base'] + i) % profile['palette_color_count']
        for i in range(nb_triplet)
    ]

    # Encode the palette
    palette_color_count = nb_triplet
    if not profile['palette_trim']:
        palette_color_count = profile['palette_color_count']

    palette_data = [0] * palette_color_count
    for i, rgb24 in enumerate(palette):
        palette_data[i] = pixel_rgb24to16(rgb24)

    # update internal information
    info['palette_map'] = palette_map
    info['palette_data'] = palette_data
    info['palette_color_count'] = palette_color_count
    info['nb_triplet'] = nb_triplet
    info['img_pixel_list_clean'] = pixels

def _image_encode_bitmap(info):
    """ Encode the bitmap

    This routine will generate the main data list which will contains the
    bitmap using Vhex-specific encoding.

    @args
    > info    (dict) - contains all needed information (image, data, ...)

    @return
    > Nothing
    """
    # fetch needed information
    img = info['img']
    profile = info['profile']
    alpha_pixels = info['img_pixel_list_alpha']
    pixels = info['img_pixel_list_clean']
    palette_map = info['palette_map']

    # generate profile-specific geometry information
    if profile['name'] in ['p16', 'p16a']:
        # Preserve alignment between rows by padding to 4 bytes
        nb_stride = ((img.width + 1) // 2) * 4
        data_size = (nb_stride * img.height) * 2
    elif profile['name'] in ['p8', 'p8a']:
        nb_stride = img.width
        data_size = img.width * img.height
    else:
        # Pad whole bytes
        nb_stride = (img.width + 1) // 2
        data_size = nb_stride * img.height

    # Generate the real data map
    data = [0] * data_size

    # encode the bitmap
    for _y in range(img.height):
        for _x in range(img.width):
            # get alpha information about this pixel
            _a = alpha_pixels[_x, _y]

            if profile['name'] in ['p16', 'p16a']:
                # If c lands on the alpha value, flip its lowest bit to avoid
                # ambiguity with alpha
                _c = profile['alpha']
                if not _a:
                    _c = pixel_rgb24to16(pixels[_x, _y]) & ~1
                data[(img.width * _y) + _x] = _c

            elif profile['name'] in ['p8', 'p8a']:
                _c = palette_map[pixels[_x,_y]] if _a > 0 else profile['alpha']
                data[(img.width * _y) + _x] = _c

            else:
                _c = palette_map[pixels[_x,_y]] if _a > 0 else profile['alpha']
                offset = (nb_stride * _y) + (_x // 2)
                if _x % 2 == 0:
                    data[offset] |= (_c << 4)
                else:
                    data[offset] |= _c

    # update external information
    info['data'] = data
    info['data_size'] = data_size
    info['nb_stride'] = nb_stride
    info['data_size'] = data_size

def _image_convert(asset, profile_name):
    """ Image asset convertion

    @args
    > asset   (_VxAsset) - asset information
    > profile_name (str) - profile name information

    @return
    > a dictionary with all image information
    """
    # generate critical information and check posible error
    img_info = {
        'img'     : Image.open(asset.path),
        'profile' : _profile_find(profile_name)
    }
    if not img_info['img']:
        log.error(f"unable to open the asset '{asset.path}', abord")
        return None
    if not img_info['profile']:
        log.error(f"unable to find the color profile '{profile_name}', abord")
        return None

    # convert the bitmap and generate critical information
    _image_isolate_alpha(img_info)
    if img_info['profile']['is_indexed']:
        _image_encode_palette(img_info)
    _image_encode_bitmap(img_info)

    # return generated information
    return img_info

## source file content generation

def _display_array(array, prefix='\t\t'):
    """ Display array information (only for p16* profile) """
    line = 0
    content = ''
    for pixels in array:
        if line == 0:
            content += prefix
        if line >= 1:
            content += ' '
        content += f'{pixels:#06x},'
        if (line := line + 1) >= 8:
            content += '\n'
            line = 0
    if line != 0:
        content += '\n'
    return content

def _image_generate_source_file(asset, info):
    """Generate image source file

    @args
    > asset (VxAsset) - asset information
    > info     (dict) - hold image information

    @return
    > file C content string
    """
    img = info['img']
    profile = info['profile']

    # generate basic header
    content  =  "#include <vhex/display/image/types.h>\n"
    content +=  "\n"
    content += f"/* {asset.name} - Vhex asset\n"
    content +=  "   This object has been converted by using the vxSDK "
    content +=  "converter */\n"
    content +=  "const image_t " + f"{asset.name} = " + "{\n"
    content += f"\t.format = {profile['profile']},\n"
    content +=  "\t.flags = IMAGE_FLAGS_RO | IMAGE_FLAGS_OWN,\n"
    content += f"\t.color_count = {profile['palette_color_count']},\n"
    content += f"\t.width  = {img.width},\n"
    content += f"\t.height = {img.height},\n"
    content += f"\t.stride = {info['nb_stride']},\n"

    # encode bitmap table
    encode = 16 if profile['profile'] in ['p16', 'p16a'] else 8
    content += f"\t.data = (void*)(const uint{encode}_t [])" + "{\n"
    for _y in range(img.height):
        content += '\t\t'
        for _x in range(info['nb_stride']):
            pixel = info['data'][(_y * info['nb_stride']) + _x]
            if profile['profile'] in ['p16', 'p16a']:
                content += f'{pixel:#06x},'
            elif profile['profile'] in ['p8', 'p8a']:
                content += f'{pixel:#04x},'
            else:
                content += f'{pixel:3},'
        content += '\n'

    content += '\t},\n'

    # add palette information
    if 'palette_data' in info:
        content += "\t.palette = (void*)(const uint16_t []){\n"
        content += _display_array(info['palette_data'])
        content += "\t},\n"
    else:
        content += "\t.palette = NULL,\n"

    # closure and return
    content += '};'
    return content

#---
# Public
#---

def image_generate(asset, prefix_output, force_generate):
    """ Convert an image asset to a C source file

    @args
    > asset         (_VxAsset) - minimal asset information
    > prefix_output      (str) - prefix for source file generation
    > force_generate    (bool) - force generate the source file

    @return
    > pathname of the generated file
    """
    # check critical requirement
    if 'profile' not in asset.meta:
        log.error(f"[{asset.name}] missing profile information!")
        return ''

    # check if the file already exists
    asset_src = f'{prefix_output}/{asset.name}_vximage.c'
    if not force_generate and os.path.exists(asset_src):
        return asset_src

    #generate the source file content
    if not (img_info := _image_convert(asset, asset.meta['profile'])):
        return ''
    content = _image_generate_source_file(asset, img_info)

    # generate the source file
    with open(asset_src, "w", encoding='utf8') as file:
        file.write(content)
    return asset_src
