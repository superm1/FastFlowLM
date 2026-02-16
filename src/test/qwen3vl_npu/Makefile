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
TEST_SOURCES += ../../common/AutoModel/modeling_qwen3vl.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_qwen3vl_image.cpp
TEST_SOURCES += ../../common/image_process_utils/imageproc.cpp
TEST_SOURCES += ../../common/image_process_utils/imageprocAVX512.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/qwen3vl/qwen3vl_npu_sequence.hpp
HEADERS += ../../include/qwen3vl/qwen3vl_npu.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_qwen3vl_npu

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen3vl_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/test_qwen3vl_npu
	make -C ../../detail/qwen3vl_npu
	cp $(LIB_DIR)/*.so $(BUILD_DIR)
	cp ../../model_list_linux.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_qwen3vl_npu --model qwen3vl-it -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe
SOURCES := $(wildcard ../../detail/qwen3vl_npu/*.hpp)
SOURCES += $(wildcard ../../detail/qwen3vl_npu/*.cpp)
SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_qwen3vl.cpp)
SOURCES += $(wildcard ../../include/AutoModel/automodel.hpp)
SOURCES += $(wildcard ../../include/AutoModel/modeling_qwen3vl.hpp)
SOURCES += $(wildcard ../../include/qwen3vl/qwen3vl_npu.hpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

all: directories test

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen3vl_npu.exe: $(SOURCES)
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/qwen3vl_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_qwen3vl_npu"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_qwen3vl_npu.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_qwen3vl_npu.exe -m qwen3vl-it:4b -s 1 -p 0"

.PHONY: all clean test directories

endif 
