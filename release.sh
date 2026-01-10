#!/bin/bash

cmake -S . -B Build/Release -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
mv -f Build/Release/compile_commands.json Build
cmake --build Build/Release --config Release
ln -s Build/compile_commands.json .
cd Build/Release/Run
hyprctl dispatch fullscreenstate 0

./MyProject
cd ../../..
