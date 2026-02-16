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
# Linux build environment
# Use g++-13 directly without CMake

LDFLAGS += -lqwen2vl_npu
LDFLAGS += -lavformat -lavcodec -lavutil -lswscale -lswresample

TEST_SOURCES += test.cpp
TEST_SOURCES += ../../common/AutoModel/automodel.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_qwen2vl.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_qwen2vl_image.cpp
TEST_SOURCES += ../../common/image_process_utils/imageproc.cpp
TEST_SOURCES += ../../common/image_process_utils/imageprocAVX512.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/qwen2vl/qwen2vl_npu.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_qwen2vl_npu

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen2vl_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/test_qwen2vl_npu
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../lib:$(LD_LIBRARY_PATH) && ./test_qwen2vl_npu --model qwen2.5vl-it -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe

SOURCES := $(wildcard ../../detail/qwen2vl_npu/*.hpp)
SOURCES += $(wildcard ../../detail/qwen2vl_npu/*.cpp)
SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_qwen2vl.cpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

all: directories test

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/xclbins
	mkdir -p $(LIB_DIR)

$(BUILD_DIR)/test_qwen2vl_npu.exe: ${SOURCES}
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/qwen2vl_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_qwen2vl_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_qwen2vl_npu.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_qwen2vl_npu.exe -m qwen2.5vl-it -s 1 -p 0"

.PHONY: all clean test directories

endif 
