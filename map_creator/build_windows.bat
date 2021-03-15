@echo off
replace360000a.py windows
cd build_windows
gcc map_creator.c resources.res -o map_creator.exe -L./lib -I./../include -lmingw32 -lSDL2main -lSDL2 -lcomdlg32 %1
cd ..
replace_pixels.py windows
pause
