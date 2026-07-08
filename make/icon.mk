# --- ADD THESE VARIABLES TO YOUR DIRECTORIES SECTION ---
ICON_MASTER := $(ASSETS_DIR)/icon_master.png
ICONSET_DIR := $(BUILD_DIR)/AppIcon.iconset
ICON_FILE   := $(BUILD_DIR)/AppIcon.icns

# --- ADD THIS PIPELINE TARGET TO AUTOMATE IMAGE SLICING ---
.PHONY: icon
icon: $(ICON_MASTER)
	@echo "Generating Apple .icns bundle from master image..."
	@mkdir -p $(ICONSET_DIR)
	
	# Automatically rescale the master 1024x1024 PNG into all standard macOS resolutions
	sips -z 16 16     $(ICON_MASTER) --out $(ICONSET_DIR)/icon_16x16.png > /dev/null
	sips -z 32 32     $(ICON_MASTER) --out $(ICONSET_DIR)/icon_16x16@2x.png > /dev/null
	sips -z 32 32     $(ICON_MASTER) --out $(ICONSET_DIR)/icon_32x32.png > /dev/null
	sips -z 64 64     $(ICON_MASTER) --out $(ICONSET_DIR)/icon_32x32@2x.png > /dev/null
	sips -z 128 128   $(ICON_MASTER) --out $(ICONSET_DIR)/icon_128x128.png > /dev/null
	sips -z 256 256   $(ICON_MASTER) --out $(ICONSET_DIR)/icon_128x128@2x.png > /dev/null
	sips -z 256 256   $(ICON_MASTER) --out $(ICONSET_DIR)/icon_256x256.png > /dev/null
	sips -z 512 512   $(ICON_MASTER) --out $(ICONSET_DIR)/icon_256x256@2x.png > /dev/null
	sips -z 512 512   $(ICON_MASTER) --out $(ICONSET_DIR)/icon_512x512.png > /dev/null
	sips -z 1024 1024 $(ICON_MASTER) --out $(ICONSET_DIR)/icon_512x512@2x.png > /dev/null
	
	# Pack the generated iconset directory into a single system .icns file
	iconutil -c icns $(ICONSET_DIR) -o $(ICON_FILE)
	@rm -rf $(ICONSET_DIR)
	@echo "Successfully generated Apple icon file: $(ICON_FILE)"
