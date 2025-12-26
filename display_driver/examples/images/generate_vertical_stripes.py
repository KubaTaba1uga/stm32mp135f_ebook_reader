class Point:
    def __init__(self, x, y, is_black):
        self.x=x
        self.y=y
        self.is_black=is_black

    def __str__(self):
        return f"x={self.x} y={self.y} is_black={self.is_black}"
    
screen = []
for y in range(h):
    row = []
    for x in range (w):
        if y % 10:
            row.append(Point(x=x, y=y, is_black=True))
        else:
            row.append(Point(x=x, y=y, is_black=False))
    screen.append(row)
            
# this should create vertical strips on the screen:
#
# ----------------------
#
# ----------------------
#
#

# The screen expects that we invert x and y
inverted_screen = []

for y in range (w):
    new_row = []
    for x in range(h):
        screen
for row in screen:
    
    
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
            byte |= (1 << i)

    dd_bytes.append(byte)

with open("bw_turtle.c", "w") as fp:
    fp.write("unsigned char bw_turtle[] = {")
    fp.write(" ".join("0x%.2X," % byte for byte in dd_bytes ))
    fp.write("};")

    
