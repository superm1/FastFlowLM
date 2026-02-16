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

HEADERS += ../../include/gemma_text/gemma_text_npu.hpp
HEADERS += ../../include/gemma_text/gemma_text_npu_sequence.hpp
HEADERS += ../../include/gemma_text/gemma_text_lm_head.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_gemma_text_npu

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_gemma_text_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/test_gemma_text_npu
	make -C ../../detail/gemma_text_npu
	cp $(LIB_DIR)/libgemma_text_npu.so $(BUILD_DIR)
	cp ../../../model_list_linux.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_gemma_text_npu --model gemma3:270m -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe

SOURCES := $(wildcard ../../detail/gemma_text_npu/*.hpp)
SOURCES += $(wildcard ../../detail/gemma_text_npu/*.cpp)
SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_gemma3_text.cpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

all: directories test

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_gemma_text_npu.exe: $(SOURCES)
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/gemma_text_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_gemma_text_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_gemma_text_npu.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_gemma_text_npu.exe -m gemma3:270m -s 1 -p 0"

.PHONY: all clean test directories

endif 
