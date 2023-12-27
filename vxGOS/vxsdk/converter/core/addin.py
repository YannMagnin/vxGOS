#from core.conv.pixel import rgba8conv
#
#from PIL import Image
#import os
#
#__all__ = [
#    'generate_addin'
#]
#
#
#def generate_addin(binary, icon=None, name=None, output=None, version=None):
#    r"""Generate an addin for the Vhex Operating System.
#
#    The addin name (passed through the `name` argument) is optional here. In
#    this case, the name will use the internal name...which can be guessed using
#    the binary name (e.i '/path/to/the/super_addin.elf' -> internal name =
#    'super_addin' -> output name = '/path/to/the/super_addin').
#
#    The output path for the generated file is, by defautl, the same path that
#    the binary but the suffix '.vxos' will be added.
#
#    if the icon is not specified, a default blank icon will be used.
#
#    Args:
#    > binary (str) - binary path
#    > icon   (str) - addin icon path                        (optional)
#    > name   (str) - addin name (displayed in the menu)     (optional)
#    > output (str) - output path for the generated addin    (optional)
#
#    _fixme:
#    > generate default internal name
#    > change 8-bits icon into rgb565
#    > add internal addin version in the header
#    """
#    if not os.path.exists(binary):
#        logger(LOG_ERR, 'binary path is invalid')
#        sys.exit(84)
#    if icon and not os.path.exists(icon):
#        logger(LOG_WARN, f'{icon}: icon does not exists, ignored')
#        icon = None
#    if not name:
#        name = ''
#    if not output:
#        output = binary + '.vxos'
#
#    if icon:
#        bitmap = Image.open(icon)
#        if bitmap.size != (92, 64):
#            logger(
#                LOG_ERR,
#                f'{icon}:icon size does not match {bitmap.size} != (92, 64)',
#                exit=84
#            )
#
#    with open(binary, 'rb') as b:
#        with open(output, 'wb') as a:
#            a.write(b'VHEX')
#            a.write(name.encode('utf8'))
#            a.write(b'\x00')
#            if icon:
#                for pixel in bitmap.getdata():
#                    a.write(rgba8conv(pixel).to_bytes(1, 'big'))
#            else:
#                a.write(bytes(92*64))
#            a.write(b.read())
#
#    return 0
