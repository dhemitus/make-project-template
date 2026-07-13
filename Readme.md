to compile:
# Force delete all old build outputs
rm -rf build
make clean

# Recompile the app bundle and installer
bear --use-cc /usr/bin/clang --use-c++ /usr/bin/clang++ -- make dmg

# clear macos ui cache db
/System/Library/Frameworks/CoreServices.framework/Frameworks/LaunchServices.framework/Support/lsregister -f build/VulkanApp.app

# compile to android app

```cli
bear -- make android-apk
```

# install to android device
adb install -r build/android/app/build/outputs/apk/debug/app-debug.apk

# to watch log in terminal
adb logcat | grep -E "SDL|AndroidRuntime|libengine"
adb logcat -s SDL,AndroidRuntime,Engine
