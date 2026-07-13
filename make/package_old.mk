# --- DISTRIBUTION PACKAGING CONFIGURATION ---
APP_NAME    := VulkanApp.app
APP_BUNDLE  := $(BUILD_DIR)/$(APP_NAME)
CONTENTS    := $(APP_BUNDLE)/Contents
MAC_OS_DIR  := $(CONTENTS)/MacOS
FRAMEWORKS  := $(CONTENTS)/Frameworks
RESOURCES   := $(CONTENTS)/Resources
DIST_DMG    := $(BUILD_DIR)/VulkanApp_Installer.dmg

.PHONY: bundle dmg

bundle: $(TARGET) shaders icon
	@echo "Assembling macOS App Bundle..."
	@mkdir -p "$(MAC_OS_DIR)" "$(FRAMEWORKS)" "$(RESOURCES)/shaders"

	# Copy configurations securely
	cp $(TARGET) "$(MAC_OS_DIR)/"
	#cp Info.plist "$(CONTENTS)/"
	#cp $(SHADER_BUILD)/*.spv "$(RESOURCES)/shaders/"
	#cp $(LIB_ENGINE) "$(FRAMEWORKS)/"
	#cp $(SDL_BUILD_DIR)/lib/libSDL3*.dylib "$(FRAMEWORKS)/" 2>/dev/null || cp $(SDL_BUILD_DIR)/lib/libSDL3*.so "$(FRAMEWORKS)/"
	
	
	# 1. Compile main.c into an object file without ASan to prevent LaunchServices panics
	$(CC) $(filter-out -fsanitize=address,$(CFLAGS)) $(LOCAL_INC) $(GLOBAL_INC) -c app/src/main.c -o $(BUILD_DIR)/main_dist.o
	
	# 2. Force re-compilation of subdirectories for the bundle wrapper
	$(MAKE) -C libs BUILD=release
	
	# 3. Link the clean desktop production binary
	$(CXX) $(filter-out -fsanitize=address,$(CXXFLAGS)) $(BUILD_DIR)/main_dist.o $(INTERNAL_LIBS) $(EXT_LIBS) \
		-Wl,-rpath,$(SDL_BUILD_DIR)/lib \
		-Wl,-rpath,@executable_path/../Frameworks -o "$(MAC_OS_DIR)/app_main"
	
	# 4. Copy support files and internal engines into place
	cp $(ASSETS_DIR)/Info.plist "$(CONTENTS)/"
	cp $(SHADER_BUILD)/*.spv "$(RESOURCES)/shaders/"
	cp $(LIB_ENGINE) "$(FRAMEWORKS)/"
	cp $(SDL_BUILD_DIR)/lib/libSDL3*.dylib "$(FRAMEWORKS)/" 2>/dev/null || cp $(SDL_BUILD_DIR)/lib/libSDL3*.so "$(FRAMEWORKS)/"
	# --- ADD THIS LINE TO COPY THE AUTO-GENERATED ICON ---
	cp $(ICON_FILE) "$(RESOURCES)/"

	# --- THE VULKAN BUNDLE FIX ---
	# Automatically discover your system's Homebrew or LunarG Vulkan Loader library file
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
	install_name_tool -id @executable_path/../Frameworks/libengine.dylib "$(FRAMEWORKS)/libengine.dylib"
	install_name_tool -change @rpath/libengine.dylib @executable_path/../Frameworks/libengine.dylib "$(MAC_OS_DIR)/app_main"
	install_name_tool -change @rpath/libSDL3.dylib @executable_path/../Frameworks/libSDL3.dylib "$(MAC_OS_DIR)/app_main"
	install_name_tool -change @rpath/libSDL3.dylib @loader_path/libSDL3.dylib "$(FRAMEWORKS)/libengine.dylib"
	
	# Fix the app binary link to use the bundled Vulkan loader instead of the system link
	install_name_tool -change @rpath/libvulkan.1.dylib @executable_path/../Frameworks/libvulkan.1.dylib "$(MAC_OS_DIR)/app_main"
	
	# 6. Clean up temporary tracking tags, attributes, and sign the container
	xattr -cr "$(APP_BUNDLE)"
	codesign --force --deep --sign - "$(APP_BUNDLE)"

dmg: bundle
	@echo "Packaging App Bundle into standalone Installer DMG..."
	@rm -f "$(DIST_DMG)"
	@rm -rf "$(BUILD_DIR)/dmg_stage"
	
	# Establish structured clean staging layout folder
	@mkdir -p "$(BUILD_DIR)/dmg_stage"
	cp -R "$(APP_BUNDLE)" "$(BUILD_DIR)/dmg_stage/"
	ln -s /Applications "$(BUILD_DIR)/dmg_stage/Applications"
	
	# Corrected hdiutil syntax: removed invalid manual size overrides parameters completely
	hdiutil create -volname "Vulkan App Installer" -srcfolder "$(BUILD_DIR)/dmg_stage" -fs HFS+ -format UDZO "$(DIST_DMG)"
	
	# Flush staging footprint
	@rm -rf "$(BUILD_DIR)/dmg_stage"
	@echo "Success! Package created at: $(DIST_DMG)"# --- AUTOMATED CODE FORMATTING RULES ---
