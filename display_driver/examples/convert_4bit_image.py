"""
Convert an image to a packed C `unsigned char[]` array.
Use 4 bit gray scale. 0b00 is white, 0b01 is gray A, 0b10 is gray B, 0b11 is black.

You can prepare source image from any image using ImageMagick:
✦ ❯ magick ~/Downloads/cats.jpg -colorspace Gray -colors 4 -quality 100 +dither ~/Downloads/cats_gray4.png

- Input: image file (PNG/JPG/etc.), expected size 800x480 or 480x800.
- Output: .c file containing bytes packed 4 pixels per byte (bit order MSB or LSB).

Install dependency:
    pip install pillow==12.0.0

Usage:
    python3 convert_4bit_image.py INPUT.png OUTPUT.c --bit-order MSB
"""

from PIL import Image
import argparse


class Pixel:
    def __init__(self, x, y, colour):
        self.x = x
        self.y = y
        self.colour = colour

    def __str__(self):
        return f"x={self.x} y={self.y} colour={self.colour}"


def main():
    ap = argparse.ArgumentParser(description="Convert BW image to packed C byte array")
    ap.add_argument("input", help="Input image path (png, jpg, ...)")
    ap.add_argument("output", help="Output .c file path")
    ap.add_argument("--bit-order", default="MSB", help="Output bit order")
    ap.add_argument("--name", default="output", help="Name of generated variable")
    args = ap.parse_args()

    img = Image.open(args.input).convert("RGBA")
    width, height = img.size
    pix = img.load()

    pixels = []
    colours = set()
    for y in range(height):
        for x in range(width):
            v = pix[x, y]  # 0=black .. 255=white

            # map so 0b00=white ... 0b11=black
            if any(val > 230 for val in v[0:3]):

                colour = 0b00

            elif any(val > 200 for val in v[0:3]):
                colour = 0b10
            elif any(val > 140 for val in v[0:3]):
                colour = 0b01
            else:
                colour = 0b11

            colours.add(colour)
            # colours.add(v)
                # print(f"{v=}:{colour=}")            
            pixels.append(Pixel(x, y, colour))


    print(colours)
    with open(args.output, "w") as fp:
        fp.write(f"unsigned char {args.name}[] = {{")
        fp.write(" ".join("0x%.2X," % byte for byte in convert_pixels(pixels)))
        fp.write("};")


def convert_pixels(pixels):
    pixels_bytes = []
    current = None
    for i, pixel in enumerate(pixels):
        # We have 2 bits per colour so 4 pixels per byte
        if i % 4 == 0:
            current = []
            pixels_bytes.append(current)
        current.append(pixel)

    dd_bytes = []
    for pixels_byte in pixels_bytes:
        byte = 0

        for i, pixel in enumerate(pixels_byte):
            # byte |= pixel.colour << (i * 2)
            byte |= pixel.colour << (3 - i) * 2

        dd_bytes.append(byte ^ 0xFF)

    return dd_bytes


if __name__ == "__main__":
    main()
