import glob
import os
import PIL.Image as Image
with open("build_"+sys.argv[1]+"/map_creator"+(".exe" if sys.argv[1]=="windows" else ""),"rb")as f:
    ctn=f.read()
img=Image.open("pixels.png")
pixels=b""
for i in range(300):
    for j in range(300):
        pixels+=bytes(reversed(img.getpixel((j,i))))
with open(sys.argv[1]+"/Lasers map creator"+(".exe" if sys.argv[1]=="windows" else ""),"wb")as f:
    f.write(ctn.replace(b"a"*360000,pixels))
for x in glob.glob("build_"+sys.argv[1]+"/*.h"):
    os.remove(x)
