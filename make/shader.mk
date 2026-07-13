# --- AUTOMATED SHADER COMPILATION MODULE ---
.PHONY: shaders

shaders:
	@echo "Checking for Vulkan shader source files..."
	@mkdir -p $(SHADER_BUILD)
	
	# 1. Dynamically locate and compile any Vertex Shaders found
	@for f in shaders/*.vert; do \
		if [ -f "$$f" ]; then \
			out=$(SHADER_BUILD)/$$(basename "$$f").spv; \
			echo "Compiling Vertex Shader: $$f -> $$out"; \
			$(GLSLC) "$$f" -o "$$out"; \
		fi \
	done
	
	# 2. Dynamically locate and compile any Fragment Shaders found
	@for f in shaders/*.frag; do \
		if [ -f "$$f" ]; then \
			out=$(SHADER_BUILD)/$$(basename "$$f").spv; \
			echo "Compiling Fragment Shader: $$f -> $$out"; \
			$(GLSLC) "$$f" -o "$$out"; \
		fi \
	done

