# =============================================================================
# Qwen NPU Library Makefile
# =============================================================================
#
# Author: Zhenyu Xu (zxu3@clemson.edu)
# Created: 2024
#
# This Makefile is used to build the Qwen NPU library components.
#
# Usage:
#   make        - Build all targets
#   make clean  - Remove all built files
#   make help   - Show this help message
#
# =============================================================================
-include ../common.mk
ifeq ($(WSL), 0)

TEST_SOURCES += test.cpp
TEST_SOURCES += ../../common/AutoModel/automodel.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_gemma3_text.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/gemma_text/gemma_embedding.hpp
HEADERS += ../../include/gemma_text/gemma_embedding_sequence.hpp
HEADERS += ../../include/gemma_text/gemma_text_lm_head.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_gemma_embedding

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_gemma_embedding: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/test_gemma_embedding
	make -C ../../detail/gemma_embedding
	cp $(LIB_DIR)/libgemma_embedding.so $(BUILD_DIR)
	cp ../../../model_list_linux.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../lib && ./test_gemma_embedding --model embed-gemma:300m -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe

SOURCES += test.cpp
SOURCES += ../../common/AutoEmbeddingModel/auto_embedding_model.cpp
SOURCES += ../../common/AutoEmbeddingModel/modeling_gemma_embedding.cpp
SOURCES += ../../common/tokenizer/tokenizer.cpp

HEADERS += ../../include/AutoEmbeddingModel/auto_embedding_model.hpp
HEADERS += ../../include/AutoEmbeddingModel/modeling_gemma_embedding.hpp
HEADERS += ../../include/tokenizer/tokenizer.hpp

all: directories test

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_gemma_embedding.exe: $(SOURCES) $(HEADERS)
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/gemma_embedding"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_gemma_embedding_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_gemma_embedding.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_gemma_embedding.exe -m embed-gemma:300m -s 1 -p 0"

.PHONY: all clean test directories

endif 
