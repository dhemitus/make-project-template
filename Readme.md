# make project template


to compile:
```cli
rm -rf build
make clean
```

Recompile the app bundle and installer
```cli
bear --use-cc /usr/bin/clang --use-c++ /usr/bin/clang++ -- make dmg
```

clear macos ui cache db
```cli
/System/Library/Frameworks/CoreServices.framework/Frameworks/LaunchServices.framework/Support/lsregister -f build/VulkanApp.app
```

compile to android app
```cli
bear -- make android-apk
```

install to android device
```cli
adb install -r build/android/app/build/outputs/apk/debug/app-debug.apk
```

to watch log in terminal
```cli
adb logcat | grep -E "SDL|AndroidRuntime|libengine"
```
```cli
adb logcat -s SDL,AndroidRuntime,Engine
```
