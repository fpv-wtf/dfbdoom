## dfbdoom for DJI FPV Goggles
Directfb Doom, forked from [fbdoom](https://github.com/stoffera/fbdoom "fbdoom").

## Installing
Grab the latest binary from [Releases](https://github.com/fpv-wtf/dfbdoom/releases) and have your Doom WAD in the same folder.
```
adb push dbfdoom /blackbox
#mind the lower case target file name!
adb push DOOM.WAD /blackbox/doom.wad
adb shell chmod 755 /blackbox/dfbdoom
```
## Running
```
#disable the gui
adb shell setprop dji.glasses_wm150_service 0
#start doom
adb shell "cd /blackbox/ && ./dfbdoom"
#enable the gui
adb shell setprop dji.glasses_wm150_service 1
```
### Building
You need: 
- NDK in your host path
- adb connected
- a Doom WAD with a lower case filename in /blackbox/

Then run:
```
cd jni
./debug.sh
```

### 'Features'
- No input
- No sound
- Autoplays demo
- 4x upscaling
- Tested only on gp150
