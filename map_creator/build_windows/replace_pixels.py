import glob
import os
with open("map_creator.exe","rb")as f:
    ctn=f.read()
with open("pixels","rb")as f:
    pixels=f.read()
with open("../windows/Lasers map creator.exe","wb")as f:
    f.write(ctn.replace(b"a"*360000,pixels))
for x in glob.glob("*.h"):
    os.remove(x)
