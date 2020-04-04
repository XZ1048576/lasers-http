#! /bin/bash
cd build_OSX
python3 replace360000a.py
gcc -L ./SDL2.framework/Versions/Current -F . -framework SDL2 -I ./../include map_creator.c -o map_creator
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/SDL2.framework/SDL2 map_creator
python3 replace_pixels.py
chmod 755 ../MacOSX/map_creator
