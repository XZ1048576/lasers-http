with open("../map_creator.c")as f:
    ctn=f.read()
with open("map_creator.c","w")as f:
    f.write(ctn.replace("###360000a###","a"*360000))
import glob
for x in glob.glob("../*.h"):
    with open(x) as f:
        ctn=f.read()
    with open(x[3:],"w")as f:
        f.write(ctn)
