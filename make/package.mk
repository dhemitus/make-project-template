
# --- DISTRIBUTION PACKAGING CONFIGURATION ---
APP_BUNDLE  := $(BUILD_DIR)/$(APP_NAME)
CONTENTS    := $(APP_BUNDLE)/Contents
MAC_OS_DIR  := $(CONTENTS)/MacOS
FRAMEWORKS  := $(CONTENTS)/Frameworks
RESOURCES   := $(CONTENTS)/Resources
DIST_DMG    := $(BUILD_DIR)/VulkanApp_Installer.dmg

.PHONY: bundle dmg

bundle: $(TARGET) shaders icon
	@echo "Assembling macOS App Bundle Container Layout..."
	@mkdir -p "$(MAC_OS_DIR)" "$(FRAMEWORKS)" "$(RESOURCES)/shaders"
	
	# 1. Compile main.c into a production object file without ASan to prevent LaunchServices crashes
	$(CC) $(filter-out -fsanitize=address,$(CFLAGS)) $(LOCAL_INC) $(GLOBAL_INC) -c app/src/main.c -o $(BUILD_DIR)/main_dist.o
	
	# 2. Force a clean re-compilation of internal libraries for release deployment
	$(MAKE) -C libs BUILD=release
	
	# 3. Link the clean desktop production binary
	$(CXX) $(filter-out -fsanitize=address,$(CXXFLAGS)) $(BUILD_DIR)/main_dist.o $(INTERNAL_LIBS) $(EXT_LIBS) \
		-Wl,-rpath,@executable_path/../Frameworks -o "$(MAC_OS_DIR)/$(EXC_NAME)"
	
	# 4. Copy support asset files, configuration plists, and engine drivers into place
	#cp $(ASSETS_DIR)/Info.plist "$(CONTENTS)/"
	@echo '<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://apple.com">\n<plist version="1.0">\n<dict>\n\t<key>CFBundleIconFile</key>\n\t<string>AppIcon.icns</string>\n\t<key>CFBundleExecutable</key>\n\t<string>$(EXC_NAME)</string>\n\t<key>CFBundleIdentifier</key>\n\t<string>$(APP_ID)</string>\n\t<key>CFBundleName</key>\n\t<string>$(APP_NAME)</string>\n\t<key>CFBundlePackageType</key>\n\t<string>APPL</string>\n\t<key>CFBundleShortVersionString</key>\n\t<string>$(VERSION)</string>\n\t<key>CFBundleSignature</key>\n\t<string>????</string>\n\t<key>CFBundleSupportedPlatforms</key>\n\t<array>\n\t\t<string>MacOSX</string>\n\t</array>\n\t<key>LSMinimumSystemVersion</key>\n\t<string>11.0</string>\n\t<key>NSHighResolutionCapable</key>\n\t<true/>\n</dict>\n</plist>' > $(CONTENTS)/Info.plist
	cp $(SHADER_BUILD)/*.spv "$(RESOURCES)/shaders/"
	cp $(LIB_ENGINE) "$(FRAMEWORKS)/"
	cp $(SDL_BUILD_DIR)/lib/libSDL3*.dylib "$(FRAMEWORKS)/" 2>/dev/null || true
	if [ -f "$(ICON_FILE)" ]; then cp $(ICON_FILE) "$(RESOURCES)/"; fi

	# --- THE VULKAN BUNDLE FRAMEWORK ASSIGNMENT ---
	@VULKAN_SYSTEM_DYLIB=$$(pkg-config --variable=libdir vulkan 2>/dev/null)/libvulkan.1.dylib; \
	if [ -f "$$VULKAN_SYSTEM_DYLIB" ]; then \
		echo "Bundling Vulkan Loader from: $$VULKAN_SYSTEM_DYLIB"; \
		cp "$$VULKAN_SYSTEM_DYLIB" "$(FRAMEWORKS)/"; \
	elif [ -f "/opt/homebrew/lib/libvulkan.1.dylib" ]; then \
		echo "Bundled fallback Homebrew Vulkan Loader"; \
		cp "/opt/homebrew/lib/libvulkan.1.dylib" "$(FRAMEWORKS)/"; \
	else \
		echo "Bundled fallback /usr/local Vulkan Loader"; \
		cp "/usr/local/lib/libvulkan.1.dylib" "$(FRAMEWORKS)/"; \
	fi
	
	# 5. Establish local relative framework binding rules inside the app bundle
	@install_name_tool -id @executable_path/../Frameworks/libengine.dylib "$(FRAMEWORKS)/libengine.dylib" 2>/dev/null || true
	@install_name_tool -id @executable_path/../Frameworks/libSDL3.dylib "$(FRAMEWORKS)/libSDL3.dylib" 2>/dev/null || true
	@install_name_tool -id @executable_path/../Frameworks/libvulkan.1.dylib "$(FRAMEWORKS)/libvulkan.1.dylib" 2>/dev/null || true
	
	# Dynamically target whatever internal tracking name SDL3 embedded during its compilation
	@SDL_INT_NAME=$$(otool -D "$(FRAMEWORKS)/libSDL3.dylib" | tail -n 1); \
	install_name_tool -change "$$SDL_INT_NAME" @executable_path/../Frameworks/libSDL3.dylib "$(MAC_OS_DIR)/$(EXC_NAME)" 2>/dev/null || true; \
	install_name_tool -change "$$SDL_INT_NAME" @loader_path/libSDL3.dylib "$(FRAMEWORKS)/libengine.dylib" 2>/dev/null || true
	
	@install_name_tool -change @rpath/libengine.dylib @executable_path/../Frameworks/libengine.dylib "$(MAC_OS_DIR)/$(EXC_NAME)" 2>/dev/null || true
	@install_name_tool -change libengine.dylib @executable_path/../Frameworks/libengine.dylib "$(MAC_OS_DIR)/$(EXC_NAME)" 2>/dev/null || true
	@install_name_tool -change @rpath/libvulkan.1.dylib @executable_path/../Frameworks/libvulkan.1.dylib "$(MAC_OS_DIR)/$(EXC_NAME)" 2>/dev/null || true
	@install_name_tool -change libvulkan.1.dylib @executable_path/../Frameworks/libvulkan.1.dylib "$(MAC_OS_DIR)/$(EXC_NAME)" 2>/dev/null || true
	
	# 6. Clean up temporary tracking tags, attributes, and sign the container
	xattr -cr "$(APP_BUNDLE)"
	codesign --force --deep --sign - "$(APP_BUNDLE)"

dmg: bundle
	@echo "Packaging App Bundle into standalone Installer DMG..."
	@rm -f "$(DIST_DMG)"
	@rm -rf "$(BUILD_DIR)/dmg_stage"
	
	@mkdir -p "$(BUILD_DIR)/dmg_stage"
	cp -R "$(APP_BUNDLE)" "$(BUILD_DIR)/dmg_stage/"
	ln -s /Applications "$(BUILD_DIR)/dmg_stage/Applications"
	
	hdiutil create -volname "Vulkan App Installer" -srcfolder "$(BUILD_DIR)/dmg_stage" -fs HFS+ -format UDZO "$(DIST_DMG)"
	
	@rm -rf "$(BUILD_DIR)/dmg_stage"
	@echo "Success! Package created at: $(DIST_DMG)"
