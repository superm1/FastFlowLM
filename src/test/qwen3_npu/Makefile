# =============================================================================
# qwen3 NPU Library Makefile
# =============================================================================
#
# Author: Zhenyu Xu (zxu3@clemson.edu)
# Created: 2024
#
# This Makefile is used to build the qwen3 NPU library components.
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
TEST_SOURCES += ../../common/AutoModel/modeling_qwen33.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/qwen3/qwen3_npu_sequence.hpp
HEADERS += ../../include/qwen3/qwen3_npu.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BUILD_DIR)/test_qwen3_npu

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen3_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: directories $(BUILD_DIR)/test_qwen3_npu
	make -C ../../detail/qwen3_npu
	cp $(LIB_DIR)/*.so $(BUILD_DIR)
	cp ../../model_list_linux.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_qwen3_npu --model qwen33:8b -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe

SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_qwen33.cpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

all: directories test

directories:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_qwen3_npu.exe: ${SOURCES}
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/qwen3_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_qwen3_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BUILD_DIR)/test_qwen3_npu.exe
	cp ../../model_list.json $(BUILD_DIR)/model_list.json
	cd $(BUILD_DIR) && ${PWSH} -Command "\$$env:PATH = '..\..\..\lib;' + \$$env:PATH; .\test_qwen3_npu.exe -m deepseek-r1-0528 -s 1 -p 0"

.PHONY: all clean test directories

endif 
