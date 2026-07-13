# --- AUTOMATED IOS APPPACKAGE MODULE ---
IOS_BUILD_DIR   := $(BUILD_DIR)/ios
IOS_APP_BUNDLE  := $(IOS_BUILD_DIR)/VulkanApp.app
IOS_TARGET      := $(IOS_APP_BUNDLE)/app_main

# Resolve Apple iOS SDK Paths dynamically from your Mac
IOS_SDK_PATH    := $(shell xcode-select -p)/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
IOS_CC          := xcrun -sdk iphoneos clang
IOS_CXX         := xcrun -sdk iphoneos clang++

# iOS Cross-Compiler target architectures (Mandates 64-bit arm64 layout flags)
IOS_CFLAGS      := -arch arm64 -isysroot $(IOS_SDK_PATH) -miphoneos-version-min=12.0 -Wall -Wextra -std=c17 -O3
IOS_CXXFLAGS     := -arch arm64 -isysroot $(IOS_SDK_PATH) -miphoneos-version-min=12.0 -Wall -Wextra -std=c++17 -O3

# Include parameters map
IOS_INC         := -I$(ROOT_DIR)/include \
                   -I$(ROOT_DIR)/libs/common/include \
                   -I$(ROOT_DIR)/libs/core/include \
                   -I$(SDL_SRC_DIR)/include

.PHONY: ios-init ios-libs ios-app

# 1. Structure the iOS build workspace directories
ios-init: init_submodules
	@echo "Structuring iOS deployment bundle container layout..."
	@mkdir -p "$(IOS_APP_BUNDLE)"

# 2. Compile your engine source files into an iOS-compliant Static Library (.a)
ios-libs: ios-init
	@echo "Compiling Engine Source code into iOS Static Library..."
	@mkdir -p $(IOS_BUILD_DIR)/obj
	
	# Compile common.c and core.c into native objects targeting iOS
	$(IOS_CC) $(IOS_CFLAGS) $(IOS_INC) -c libs/common/src/common.c -o $(IOS_BUILD_DIR)/obj/common.o
	$(IOS_CC) $(IOS_CFLAGS) $(IOS_INC) -c libs/core/src/core.c -o $(IOS_BUILD_DIR)/obj/core.o
	
	# Pack the compiled objects into a unified engine archive
	$(AR) $(ARFLAGS) $(IOS_BUILD_DIR)/libengine_ios.a $(IOS_BUILD_DIR)/obj/common.o $(IOS_BUILD_DIR)/obj/core.o

# 3. Compile SDL3 for iOS, link your application, and sign the container bundle
ios-app: ios-libs
	@echo "Cross-compiling your main application loop for mobile iOS deployment..."
	
	# Cross-compile your main application loop directly into an iOS object layer
	$(IOS_CC) $(IOS_CFLAGS) $(IOS_INC) -c app/src/main.c -o $(IOS_BUILD_DIR)/obj/main.o
	
	# Compile SDL3 natively for the iOS platform structure using CMake
	@if [ ! -f "$(IOS_BUILD_DIR)/sdl_build/libSDL3.a" ]; then \
		echo "Building SDL3 static library for iPhone hardware platforms..."; \
		mkdir -p $(IOS_BUILD_DIR)/sdl_build; \
		cd $(IOS_BUILD_DIR)/sdl_build && cmake $(SDL_SRC_DIR) \
			-DCMAKE_SYSTEM_NAME=iOS \
			-DCMAKE_OSX_ARCHITECTURES=arm64 \
			-DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
			-DSDL_STATIC=ON -DSDL_SHARED=OFF -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF; \
		$(MAKE) -C $(IOS_BUILD_DIR)/sdl_build; \
	fi

	# Link everything together into the final iOS app executable bundle
	$(IOS_CXX) $(IOS_CFLAGS) $(IOS_BUILD_DIR)/obj/main.o \
		$(IOS_BUILD_DIR)/libengine_ios.a \
		$(IOS_BUILD_DIR)/sdl_build/libSDL3.a \
		-framework Foundation -framework UIKit -framework CoreGraphics -framework QuartzCore \
		-framework Metal -framework CoreAudio -framework AudioToolbox -framework AVFoundation \
		-framework GameController -framework CoreMotion -o "$(IOS_TARGET)"

	# Copy app assets, configuration properties plists, and auto-generated icons into place
	@cp Info-iOS.plist "$(IOS_APP_BUNDLE)/Info.plist"
	@mkdir -p "$(IOS_APP_BUNDLE)/shaders"
	@cp $(SHADER_BUILD)/*.spv "$(IOS_APP_BUNDLE)/shaders/" 2>/dev/null || true
	@if [ -f "$(ICON_FILE)" ]; then cp $(ICON_FILE) "$(IOS_APP_BUNDLE)/AppIcon.icns"; fi

	# Apply localized ad-hoc codesigning variables to unlock deployment on sandbox targets
	@echo "Signing iOS application wrapper..."
	codesign --force --deep --sign - "$(IOS_APP_BUNDLE)"
	@echo "Success! Standalone iOS application bundle compiled at: $(IOS_APP_BUNDLE)"
