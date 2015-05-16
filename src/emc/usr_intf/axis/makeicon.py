import struct, sys, Image

icon = []

for f in sys.argv[1:]:
    img = Image.open(f).convert("RGBA")
    icon.append(struct.pack("II", *img.size))
    icon.append(img.tostring())

icon = "".join(icon)

icon = icon.encode("hex")
print "icon = ("
for i in range(0, len(icon), 72):
	print repr(icon[i:i+72])
print ").decode('hex')"

