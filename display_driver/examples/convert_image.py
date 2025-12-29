"""
Convert an image to a packed C `unsigned char[]` array.

- Input: image file (PNG/JPG/etc.), expected size 800x480 or 480x800.
- Output: .c file containing bytes packed 8 pixels per byte (bit order MSB or LSB).

Install dependency:
    pip install pillow==12.0.0

Usage:
    python3 convert_image.py INPUT.png OUTPUT.c --bit-order MSB
"""
from PIL import Image
import argparse

class Point:
    def __init__(self, x, y, is_black):
        self.x=x
        self.y=y
        self.is_black=is_black

    def __str__(self):
        return f"x={self.x} y={self.y} is_black={self.is_black}"

    @classmethod
    def is_black(cls, pix, threshold):
        return pix[0] <= threshold and pix[1] <= threshold and pix[2] <= threshold
    
def main():
    ap = argparse.ArgumentParser(description="Convert BW image to packed C byte array")
    ap.add_argument("input", help="Input image path (png, jpg, ...)")
    ap.add_argument("output", help="Output .c file path")
    ap.add_argument("--bit-order", default="MSB", help="Output bit order")
    ap.add_argument("--threshold", default=128, help="When pixel should be considered black and when white")        
    args = ap.parse_args()
    
    img = Image.open(args.input).convert("RGBA")
    width, heigth = img.size
    pix = img.load()

    if heigth == 480 and width == 800:
        points = [Point(x=x, y=y, is_black=Point.is_black(pix[x, y], args.threshold)) for y in range(heigth) for x in range(width)]
    elif heigth == 800 and width == 480:
        # Now we need to transform this bytes to layout 480x800
        # Now we need to iterate over columns starting from end (x=last y=0, x=last y=1, ...)
        points = []
        for x in range(width-1, 0, -1):
            for y in range(0, heigth, 1):
                points.append(Point(x=x, y=y, is_black=Point.is_black(pix[x, y], args.threshold)))
    else:
        raise Exception("Unsupported image resolution: %d x %d" % (width, heigth))
    
    with open(args.output, "w") as fp:
        fp.write("unsigned char output[] = {")
        fp.write(" ".join("0x%.2X," % byte for byte in convert_points(points, args.bit_order)))
        fp.write("};")


def convert_points(points, bit_order):
    points_bytes = []
    current = None
    for i, point in enumerate(points):
        if i % 8 == 0:
            current = []
            points_bytes.append(current)
        current.append(point)

    dd_bytes = []
    for points_byte in points_bytes:
        byte = 0

        for i, bit in enumerate(points_byte):
            if bit.is_black:
                if bit_order == "MSB":
                    byte |= (1 << (7 - i))
                elif bit_order == "LSB":                    
                    byte |= (1 << i)
                else:
                    raise Exception("Unsupported bit order: %s" % bit_order)

        dd_bytes.append(byte ^ 0xFF)
        
    return dd_bytes

if __name__ == "__main__":
    main()
