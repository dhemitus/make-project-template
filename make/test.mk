# Test Suite Integration Target Block
.PHONY: test format format-check

$(BUILD_DIR)/test_engine.o: tests/test_engine.c subdirs
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LOCAL_INC) $(GLOBAL_INC) -c $< -o $@

test: $(BUILD_DIR)/test_engine.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(INTERNAL_LIBS) $(EXT_LIBS) \
		-Wl,-rpath,$(SDL_BUILD_DIR)/lib \
		-Wl,-rpath,$(ROOT_DIR)/libs -o $(TEST_TARGET)
	@echo "Running test suite..."
	@./$(TEST_TARGET)

format:
	@echo "Formatting workspace source code..."
	$(CLANG_FORMAT) -i app/src/*.c tests/*.c include/*.h
	$(CLANG_FORMAT) -i libs/common/src/*.c libs/common/include/*.h
	$(CLANG_FORMAT) -i libs/core/src/*.c libs/core/include/*.h

format-check:
	@echo "Checking formatting rules consistency..."
	$(CLANG_FORMAT) --dry-run --Werror app/src/*.c tests/*.c include/*.h libs/common/src/*.c libs/core/src/*.c
