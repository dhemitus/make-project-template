.PHONY: shaders
# --- VULKAN SHADER PIPELINE RULES ---
shaders: $(SPV_OUTPUTS)

$(SHADER_BUILD)/%.vert.spv: shaders/%.vert
	@mkdir -p $(SHADER_BUILD)
	@echo "Compiling Vertex Shader: $<"
	$(GLSLC) $< -o $@

$(SHADER_BUILD)/%.frag.spv: shaders/%.frag
	@mkdir -p $(SHADER_BUILD)
	@echo "Compiling Fragment Shader: $<"
	$(GLSLC) $< -o $@
