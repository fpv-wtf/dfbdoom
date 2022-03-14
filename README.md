## sfbdoom for DJI FPV Goggles
Directfb Doom, forked from [fbdoom](https://github.com/stoffera/fbdoom "fbdoom").

## Installing
Grab the latest binary from [Releases](https://github.com/fpv-wtf/dfbdoom/releases) and have your Doom WAD in the same folder.
```
adb push dfbdoom /blackbox
#mind the lower case target file name!
adb push DOOM.WAD /blackbox/doom.wad
adb shell chmod 755 /blackbox/dfbdoom
```
## Running
```
#disable the gui on V1, ignored on V2
adb shell setprop dji.glasses_service 0
#disable the gui on V2, ignored on V1
adb shell setprop dji.glasses_wm150_service 0

#start doom - Doom complains about $HOME not being set if we just adb shell command
echo "cd /blackbox/ && ./dfbdoom" | adb shell 

#enable the gui on V1, ignored on V2
adb shell setprop dji.glasses_service 1
#enable the gui on V2, ignored on V1
adb shell setprop dji.glasses_wm150_service 1
```
### 'Features'
- No input (see blow)
- No sound
- Autoplays demo
- 4x upscaling
- Tested only on V2s

### Adding input
There's currently a broken DirectFB input implementation in i_fb_video.c. It detects Goggle button presses, however for whatever reason DirectFB drops the events for joystick up/down and channel select down. Additionally, the input event translation in dlatekey() is unfinished due to the previous issue and likely broken.

The former, some keys being dropped, might necessitate a rewrite to libevdev or direct handling of /dev/input/event0.

Theoretically USB keyboards should also be supported by DirectFB/evdev on V2s. If you want to debug this, you should set up a rooted AU or Vista with adequete cooling and power from the wall and use `adb connect 192.168.41.1 && adb shell` from the AU/Vista to debug while your Goggles USB port is busy with the keyboard.

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