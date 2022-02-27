#!/bin/sh
ndk-build -j8
adb push ../obj/local/armeabi-v7a/fbdoom /blackbox/ 
#something about $HOME not being defined if we don't pipe the input in but provide as an arg
echo "setprop dji.glasses_wm150_service 0 && cd /blackbox && chmod o+x fbdoom && ./fbdoom" | adb shell
