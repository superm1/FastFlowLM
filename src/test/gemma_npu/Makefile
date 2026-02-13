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

# Check if it is in Windows Subsystem for Linux (WSL)
ifeq ($(shell uname -a | grep -i WSL),)
	WSL := 0
else
	WSL := 1
endif

BUILD_DIR := ../../build/gemma_npu/test
LIB_DIR := ../../lib
BIN_DIR := $(BUILD_DIR)/bin
ifeq ($(WSL), 0)
# Linux build environment
# Use g++-13 directly without CMake

COMMON_LIB_DIR := ../../build/common/lib

TARGET = $(LIB_DIR)/libgemma_npu.so

CXX := g++-13
CXX_FLAGS := -std=c++20 -fPIC -Wall -DUSEAVX2=1
CXX_FLAGS += -mavx2 -mfma -march=native -ffast-math
CXX_FLAGS += -fmax-errors=1
CXX_FLAGS += -I../../include
CXX_FLAGS += -I../q4nx
CXX_FLAGS += -I/opt/xilinx/xrt/include
CXX_FLAGS += -MMD -MP


# Flags for test executable
TEST_FLAGS := $(CXX_FLAGS)
TEST_FLAGS += -L$(LIB_DIR)
TEST_FLAGS += -Wl,-rpath,$(LIB_DIR)

LDFLAGS += -L/opt/xilinx/xrt/lib
LDFLAGS += -Wl,-rpath,/opt/xilinx/xrt/lib
LDFLAGS += -lxrt_coreutil
LDFLAGS += -lboost_program_options -lboost_filesystem
LDFLAGS += -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR)
LDFLAGS += -L../../build/lib
LDFLAGS += -L../../build/tokenizer/lib
LDFLAGS += -ltokenizers_cpp -ltokenizers_c -l sentencepiece
LDFLAGS += -lgemma_npu -ldequant -lgemm -llm_head -lq4_npu_eXpress -lnpu_utils


TEST_SOURCES += test.cpp
TEST_SOURCES += ../../common/AutoModel/automodel.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_gemma3.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp

HEADERS += ../../include/gemma/gemma_npu_sequence.hpp
HEADERS += ../../include/gemma/gemma_npu.hpp
HEADERS += ../../include/modules/embedding.hpp
HEADERS += ../../include/modules/lm_head.hpp
HEADERS += ../../include/modules/gemm.hpp
HEADERS += ../../include/modules/dequant.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BIN_DIR)/test_gemma_npu

directories:
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/test_gemma_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BIN_DIR)/test_gemma_npu
	make -C ../../detail/gemma_npu
	cp ../../../model_list_linux.json ../../../model_list.json
	cp $(LIB_DIR)/libgemma_npu.so $(BIN_DIR)
	cd $(BIN_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_gemma_npu --model gemma3:4b -s 1

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe


TARGET = $(BUILD_DIR)/gemma_npu.dll

SOURCES := test.cpp
SOURCES += ../../common/AutoModel/automodel.cpp
SOURCES += ../../common/AutoModel/modeling_gemma3.cpp
SOURCES += ../../common/tokenizer/tokenizer.cpp
SOURCES += ../../common/modules/sampler.cpp
SOURCES += $(wildcard ../../detail/gemma_npu/*.hpp)
SOURCES += $(wildcard ../../detail/gemma_npu/*.cpp)


all: directories test

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/xclbins
	mkdir -p $(LIB_DIR)

$(BIN_DIR)/test_gemma_npu.exe: $(SOURCES)
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/gemma_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_gemma_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BIN_DIR)/test_gemma_npu.exe
	cp ../../lib/*.dll $(BIN_DIR)
	cp -r ../../xclbins/Gemma3-4B-NPU2 $(BIN_DIR)/xclbins/
	cp -r ../../xclbins/Medgemma-4B-NPU2 $(BIN_DIR)/xclbins/
	cp ../../model_list.json $(BIN_DIR)/model_list.json
	cd $(BIN_DIR) && ${PWSH} -Command ".\test_gemma_npu.exe -m gemma3:4b -s 1 -p 0"

.PHONY: all clean test directories

endif 
