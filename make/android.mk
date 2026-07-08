# --- ANDROID CUSTOMIZATION CONFIGURATION ---
ANDROID_ABI      ?= arm64-v8a
ANDROID_API      ?= 21
ANDROID_BUILD_DIR := $(BUILD_DIR)/android

# Your unique Store/Device identity (Can be completely custom!)
APPLICATION_ID  := com.vulkanforge.engine
APP_NAME        := "Vulkan Forge Engine"
ANDROID_ICON_SRC := $(ASSETS_DIR)/android_icon.png

# NDK Toolchain Path Resolution
NDK_CC  := $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang
NDK_CXX := $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang++

# Android Cross-Compiler target flags
AND_CFLAGS   := -target aarch64-linux-android$(ANDROID_API) -Wall -Wextra -std=c17 -O3 -fPIC
AND_CXXFLAGS := -target aarch64-linux-android$(ANDROID_API) -Wall -Wextra -std=c++17 -O3 -fPIC

# Include directories mapping relative paths for Android
AND_INC := -I$(ROOT_DIR)/include \
           -I$(ROOT_DIR)/libs/common/include \
           -I$(ROOT_DIR)/libs/core/include \
           -I$(SDL_SRC_DIR)/include

.PHONY: android-init android-libs android-apk

# Step 1: Prepare Android project and customize package identity without breaking JNI
android-init: init_submodules
	@echo "Initializing Android build staging directory..."
	@mkdir -p $(ANDROID_BUILD_DIR)
	
	# Copy SDL3's native Android project scaffold into our local build workspace
	@if [ ! -d "$(ANDROID_BUILD_DIR)/app" ]; then \
		cp -R $(SDL_SRC_DIR)/android-project/* $(ANDROID_BUILD_DIR)/; \
	fi
	
	# Establish the mandatory jni/SDL folder links
	@mkdir -p $(ANDROID_BUILD_DIR)/app/jni
	@rm -f $(ANDROID_BUILD_DIR)/app/jni/SDL
	@ln -s $(SDL_SRC_DIR) $(ANDROID_BUILD_DIR)/app/jni/SDL

	# --- 1. CUSTOMIZE APP NAME ---
	@sed -i '' 's/>SDLApp</>$(APP_NAME)</g' $(ANDROID_BUILD_DIR)/app/src/main/res/values/strings.xml 2>/dev/null || \
	 sed -i 's/>SDLApp</>$(APP_NAME)</g' $(ANDROID_BUILD_DIR)/app/src/main/res/values/strings.xml

	# --- FORCE STRICT LANDSCAPE ORIENTATION ---
	# Adjusts the AndroidManifest template configuration to permanently lock horizontal layout
	@sed -i '' 's/android:screenOrientation=".*"/android:screenOrientation="landscape"/g' $(ANDROID_BUILD_DIR)/app/src/main/AndroidManifest.xml 2>/dev/null || \
	 sed -i 's/android:screenOrientation=".*"/android:screenOrientation="landscape"/g' $(ANDROID_BUILD_DIR)/app/src/main/AndroidManifest.xml

	# --- 2. CUSTOMIZE APPLICATION ID (The Dual Package Fix) ---
	# We rewrite only the applicationID block inside build.gradle so it is unique to the device,
	# but we leave the internal Java namespace folder layout strictly as org.libsdl.app for JNI safety.
	@sed -i '' 's/applicationId "org.libsdl.app"/applicationId "$(APPLICATION_ID)"/g' $(ANDROID_BUILD_DIR)/app/build.gradle 2>/dev/null || \
	 sed -i 's/applicationId "org.libsdl.app"/applicationId "$(APPLICATION_ID)"/g' $(ANDROID_BUILD_DIR)/app/build.gradle

	# --- 3. THE JAVA DEP INJECTION ---
	# Tell the Java template launcher to load your 'libengine.so' library asset cleanly
	@sed -i '' 's/"main"/"engine", "main"/g' $(ANDROID_BUILD_DIR)/app/src/main/java/org/libsdl/app/SDLActivity.java 2>/dev/null || \
	 sed -i 's/"main"/"engine", "main"/g' $(ANDROID_BUILD_DIR)/app/src/main/java/org/libsdl/app/SDLActivity.java

	# --- 4. CUSTOMIZE MOBILE APP ICON ---
	@if [ -f "$(ANDROID_ICON_SRC)" ]; then \
		echo "Injecting custom app launcher icons into Android bundle..."; \
		mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-mdpi; \
		mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-hdpi; \
		mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xhdpi; \
		mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xxhdpi; \
		mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xxxhdpi; \
		sips -z 48 48   $(ANDROID_ICON_SRC) --out $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-mdpi/ic_launcher.png > /dev/null; \
		sips -z 72 72   $(ANDROID_ICON_SRC) --out $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-hdpi/ic_launcher.png > /dev/null; \
		sips -z 96 96   $(ANDROID_ICON_SRC) --out $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xhdpi/ic_launcher.png > /dev/null; \
		sips -z 144 144 $(ANDROID_ICON_SRC) --out $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xxhdpi/ic_launcher.png > /dev/null; \
		sips -z 192 192 $(ANDROID_ICON_SRC) --out $(ANDROID_BUILD_DIR)/app/src/main/res/mipmap-xxxhdpi/ic_launcher.png > /dev/null; \
	fi
	
	# --- THE DUPLICATE LIBRARY PICKFIRST FIX ---
	@echo "\nandroid.packagingOptions.pickFirst 'lib/*/libmain.so'" >> $(ANDROID_BUILD_DIR)/app/build.gradle

	# Symlink our source shaders directly into the Android asset directory structure
	@mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/assets/shaders
	@cp -R shaders/* $(ANDROID_BUILD_DIR)/app/src/main/assets/shaders/ 2>/dev/null || true

# Step 2: The Reordered Build Pipeline Target
android-apk: android-init
	@echo "Phase 1: Bootstrapping Gradle to natively compile native dependencies first..."
	@chmod +x $(ANDROID_BUILD_DIR)/gradlew
	
	@export JAVA_HOME="/opt/homebrew/opt/openjdk@17/libexec/openjdk.jdk/Contents/Home"; \
	export PATH="$$JAVA_HOME/bin:$$PATH"; \
	cd $(ANDROID_BUILD_DIR) && ./gradlew :app:externalNativeBuildDebug --stacktrace || \
	./gradlew :app:mergeDebugNativeLibs --stacktrace || true

	@echo "Phase 2: Manually cross-compiling Engine Libraries & linking against compiled SDL3..."
	@mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/jniLibs/$(ANDROID_ABI)
	
	$(NDK_CC) $(AND_CFLAGS) $(AND_INC) -shared -Wl,-soname,libengine.so \
		libs/common/src/common.c libs/core/src/core.c \
		-o $(ANDROID_BUILD_DIR)/app/src/main/jniLibs/$(ANDROID_ABI)/libengine.so -lm -llog -landroid

	@SDL3_FULL_PATH=$$(find $(ANDROID_BUILD_DIR)/app/build/intermediates -name "libSDL3.so" | head -n 1); \
	if [ -z "$$SDL3_FULL_PATH" ]; then \
		SDL3_FULL_PATH=$$(find $(ANDROID_BUILD_DIR)/app/src/main/jniLibs -name "libSDL3.so" | head -n 1); \
	fi; \
	SDL3_DIR=$$(dirname "$$SDL3_FULL_PATH"); \
	echo "Linking main code layers cleanly with SDL3 system objects..."; \
	$(NDK_CC) $(AND_CFLAGS) $(AND_INC) -shared -Wl,-soname,libmain.so \
		app/src/main.c \
		-L$(ANDROID_BUILD_DIR)/app/src/main/jniLibs/$(ANDROID_ABI) \
		-L"$$SDL3_DIR" \
		-lengine -lSDL3 \
		-o $(ANDROID_BUILD_DIR)/app/src/main/jniLibs/$(ANDROID_ABI)/libmain.so -lm -llog -landroid

	@echo "Phase 3: Finalizing APK packaging layout..."
	@export JAVA_HOME="/opt/homebrew/opt/openjdk@17/libexec/openjdk.jdk/Contents/Home"; \
	export PATH="$$JAVA_HOME/bin:$$PATH"; \
	cd $(ANDROID_BUILD_DIR) && ./gradlew assembleDebug --no-daemon --stacktrace
	
	@echo "Success! Custom Android APK built at: $(ANDROID_BUILD_DIR)/app/build/outputs/apk/debug/app-debug.apk"
