@echo off
cd build
replace360000a.py
gcc map_creator.c resources.res -o map_creator.exe -L./lib -I./../include -lmingw32 -lSDL2main -lSDL2 -lSDL2_image %1
replace_pixels.py
pause
