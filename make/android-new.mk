# --- ANDROID CUSTOMIZATION CONFIGURATION ---
ANDROID_ABI      ?= arm64-v8a
ANDROID_API      ?= 21
ANDROID_BUILD_DIR := $(BUILD_DIR)/android

# Your unique Store/Device identity
APPLICATION_ID   := com.vulkanforge.engine
APP_NAME         := Vulkan Forge Engine
ANDROID_ICON_SRC := $(ASSETS_DIR)/android_icon.png

.PHONY: android-init android-apk

# Step 1: Prepare Android project workspace and override template properties completely
android-init: init_submodules
	@echo "Initializing Android build staging directory..."
	@mkdir -p $(ANDROID_BUILD_DIR)
	
	# Copy SDL3's native Android project scaffold into our local build workspace
	@if [ ! -d "$(ANDROID_BUILD_DIR)/app" ]; then \
		cp -R $(SDL_SRC_DIR)/android-project/* $(ANDROID_BUILD_DIR)/; \
	fi
	
	# Establish the mandatory jni/SDL folder links so Gradle can compile SDL3
	@mkdir -p $(ANDROID_BUILD_DIR)/app/jni
	@rm -f $(ANDROID_BUILD_DIR)/app/jni/SDL
	@ln -s $(SDL_SRC_DIR) $(ANDROID_BUILD_DIR)/app/jni/SDL

	# --- 1. THE PRECISION APP NAME FIX ---
	@sed -i '' 's#name="app_name">Game</#name="app_name">$(APP_NAME)</#g' $(ANDROID_BUILD_DIR)/app/src/main/res/values/strings.xml 2>/dev/null || \
	 sed -i 's#name="app_name">Game</#name="app_name">$(APP_NAME)</#g' $(ANDROID_BUILD_DIR)/app/src/main/res/values/strings.xml

	# --- FORCE STRICT LANDSCAPE ORIENTATION ---
	@sed -i '' 's#android:screenOrientation=".*"#android:screenOrientation="landscape"#g' $(ANDROID_BUILD_DIR)/app/src/main/AndroidManifest.xml 2>/dev/null || \
	 sed -i 's#android:screenOrientation=".*"#android:screenOrientation="landscape"#g' $(ANDROID_BUILD_DIR)/app/src/main/AndroidManifest.xml

	# --- 2. CUSTOMIZE APPLICATION ID ---
	@sed -i '' 's/applicationId "org.libsdl.app"/applicationId "$(APPLICATION_ID)"/g' $(ANDROID_BUILD_DIR)/app/build.gradle 2>/dev/null || \
	 sed -i 's/applicationId "org.libsdl.app"/applicationId "$(APPLICATION_ID)"/g' $(ANDROID_BUILD_DIR)/app/build.gradle

	# --- 3. THE OFFICIAL SDL3 APP MODULE PROPERTIES INJECTION ---
	# We rewrite the template arguments to register your specific dependencies natively inside Gradle
	@sed -i '' 's!arguments "-DANDROID_SHARED_LIBRARIES=main"!arguments "-DANDROID_SHARED_LIBRARIES=engine;vulkan_forge_app"!g' $(ANDROID_BUILD_DIR)/app/build.gradle 2>/dev/null || \
	 sed -i 's!arguments "-DANDROID_SHARED_LIBRARIES=main"!arguments "-DANDROID_SHARED_LIBRARIES=engine;vulkan_forge_app"!g' $(ANDROID_BUILD_DIR)/app/build.gradle

	# --- 4. THE CRITICAL JAVA SUBSYSTEM LINK ---
	# Tell SDLActivity to explicitly mount your libengine.so framework before running your code!
	@sed -i '' 's/"main"/"engine", "vulkan_forge_app"/g' $(ANDROID_BUILD_DIR)/app/src/main/java/org/libsdl/app/SDLActivity.java 2>/dev/null || \
	 sed -i 's/"main"/"engine", "vulkan_forge_app"/g' $(ANDROID_BUILD_DIR)/app/src/main/java/org/libsdl/app/SDLActivity.java

	# --- 5. THE MASTER BUILD OVERWRITE FIX ---
	@cp -f make/Android_Master.cmake $(ANDROID_BUILD_DIR)/app/jni/CMakeLists.txt
	@echo "# Cache timestamp: $$(date +%s)" >> $(ANDROID_BUILD_DIR)/app/jni/CMakeLists.txt

	# --- 6. CUSTOMIZE MOBILE APP ICON ---
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
	@echo "\nandroid.packagingOptions.pickFirst 'lib/*/libengine.so'" >> $(ANDROID_BUILD_DIR)/app/build.gradle
	@echo "\nandroid.packagingOptions.pickFirst 'lib/*/libvulkan_forge_app.so'" >> $(ANDROID_BUILD_DIR)/app/build.gradle

	# Symlink our source shaders directly into the Android asset directory structure
	@mkdir -p $(ANDROID_BUILD_DIR)/app/src/main/assets/shaders
	@cp -R shaders/* $(ANDROID_BUILD_DIR)/app/src/main/assets/shaders/ 2>/dev/null || true

# Step 2: Invoke Gradle to compile and sign everything cleanly in one unified pass
android-apk: android-init
	@echo "Assembling fully compiled, linked, and signed Android package installer via Gradle..."
	@export JAVA_HOME="/opt/homebrew/opt/openjdk@17/libexec/openjdk.jdk/Contents/Home"; \
	export PATH="$$JAVA_HOME/bin:$$PATH"; \
	cd $(ANDROID_BUILD_DIR) && ./gradlew assembleDebug --no-daemon --stacktrace
	@echo "Success! Custom Signed Android APK built flawlessly at: $(ANDROID_BUILD_DIR)/app/build/outputs/apk/debug/app-debug.apk"

