"""
patch kernel image for the Raspberry Pi Imager
"""
import os
import sys

#---
# Public
#---

if len(sys.argv) != 3:
    print("./script <kernel_original> <output>")

if os.path.exists(sys.argv[2]):
    yes = input("output file already exists, overwrite? [Yn]")
    if yes and yes not in ['y', 'Y', 'yes']:
        print("output file already exists, abord", file=sys.stderr)
        sys.exit(84)
    os.remove(sys.argv[2])

with open(sys.argv[2], "xb") as patch:
    with open(sys.argv[1], "rb") as kernel:
        content = kernel.read()
        patch.write(content)
        kernel_size = len(content)
    if kernel_size % 512:
        for _ in range(0, 512 - (kernel_size % 512)):
            patch.write(0x00.to_bytes(1))

print(f"{sys.argv[2]}: patched and generated")
