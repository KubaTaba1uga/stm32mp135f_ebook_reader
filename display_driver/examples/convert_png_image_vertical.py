from PIL import Image

class Point:
    def __init__(self, x, y, is_black):
        self.x=x
        self.y=y
        self.is_black=is_black

    def __str__(self):
        return f"x={self.x} y={self.y} is_black={self.is_black}"



img = Image.open("bw_turtle.png").convert("RGBA")
w, h = img.size
pix = img.load()

# points = [Point(x=x, y=y, is_black=(pix[x, y][0] == 0)) for y in range(h) for x in range(w)]


assert(h == 800)
assert(w == 480)

points_matrix = []
for y in range(h):
    row = []
    for x in range(w):
        row.append(Point(x=x, y=y, is_black=(pix[x, y][0] == 0)))        
    points_matrix.append(row)


# Now we need to transform this bytes to layout 800x480
# Now we need to iterate over columns starting from end (x=last y=0, x=last y=1, ...)
#
new_points = []
# for i in range(480):
#     new_matrix.append([])
    
for x in range(len(points_matrix[0]) - 1, 0, -1):
    # new_row = []
    for y in range( 0, len(points_matrix), 1):
        new_points.append(points_matrix[y][x])

    # new_matrix.append(new_row)

def convert_points(points):
    points_bytes = []
    current = None
    for i, point in enumerate(points):
        if i % 8 == 0:
            current = []
            points_bytes.append(current)
        current.append(point)
        # print(point)

    dd_bytes = []
    for points_byte in points_bytes:
        byte = 0

        for i, bit in enumerate(points_byte):
            if bit.is_black:
                byte |= (1 << (7 -i)) # MSB

        dd_bytes.append(byte ^ 0xFF)
    # print(byte)
    return dd_bytes
    
with open("bw_turtle.c", "w") as fp:
    fp.write("unsigned char bw_turtle[] = {")
    fp.write(" ".join("0x%.2X," % byte for byte in convert_points(new_points)))
    fp.write("};")

    
