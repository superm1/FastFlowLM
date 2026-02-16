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
TEST_SOURCES += ../../common/AutoModel/modeling_qwen2.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/qwen2/qwen2_npu.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_qwen2_npu

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen2_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/test_qwen2_npu
	make -C ../../detail/qwen2_npu
	cp $(LIB_DIR)/libqwen2_npu.so $(BUILD_DIR)
	cp ../../../model_list_linux.json ../../../model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_qwen2_npu --model qwen2:0.6b -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe

SOURCES := $(wildcard ../../detail/qwen2_npu/*.hpp)
SOURCES += $(wildcard ../../detail/qwen2_npu/*.cpp)
SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_qwen2.cpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

TARGET = $(BUILD_DIR)/qwen2_npu.dll

all: directories test

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen2_npu.exe: ${SOURCES}
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/qwen2_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_qwen2_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_qwen2_npu.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_qwen2_npu.exe -m qwen2.5-it -s 1 -p 0"

.PHONY: all clean test directories

endif 
