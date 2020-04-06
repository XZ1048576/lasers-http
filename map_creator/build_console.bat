@echo off
cd build_windows
replace360000a.py
gcc map_creator.c resources.res -o map_creator.exe -L./lib -I./../include -lmingw32 -lSDL2main -lSDL2 -lcomdlg32 %1
replace_pixels.py
pause
