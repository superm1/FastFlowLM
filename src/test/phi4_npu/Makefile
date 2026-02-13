# =============================================================================
# Llama NPU Library Makefile
# =============================================================================
#
# Author: Zhenyu Xu (zxu3@clemson.edu)
# Created: 2024
#
# This Makefile is used to build the Llama NPU library components.
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

BUILD_DIR := ../../build/phi4_npu/test
LIB_DIR := ../../lib
BIN_DIR := $(BUILD_DIR)/bin
ifeq ($(WSL), 0)
# Linux build environment
# Use g++-13 directly without CMake

COMMON_LIB_DIR := ../../build/common/lib

TARGET = $(LIB_DIR)/libphi4_npu.so

CXX := g++-13
CXX_FLAGS := -std=c++20 -fPIC -Wall -DUSEAVX2=1
CXX_FLAGS += -mavx2 -mfma -march=native -ffast-math
CXX_FLAGS += -fmax-errors=1
CXX_FLAGS += -I../../include
CXX_FLAGS += -I../q4nx
CXX_FLAGS += -I/opt/xilinx/xrt/include
CXX_FLAGS += -I/home/alfred/mlir-aie/xdna-driver/xrt/build/Release/opt/xilinx/aiebu/include
CXX_FLAGS += -MMD -MP


# Flags for test executable
TEST_FLAGS := $(CXX_FLAGS)
TEST_FLAGS += -L$(LIB_DIR)
TEST_FLAGS += -Wl,-rpath,$(LIB_DIR)

LDFLAGS += -L/opt/xilinx/xrt/lib
LDFLAGS += -L/home/alfred/mlir-aie/xdna-driver/xrt/build/Release/opt/xilinx/aiebu/lib
LDFLAGS += -Wl,-rpath,/opt/xilinx/xrt/lib
LDFLAGS += -Wl,-rpath,/home/alfred/mlir-aie/xdna-driver/xrt/build/Release/opt/xilinx/aiebu/lib
LDFLAGS += -lxrt_coreutil -laiebu_static
LDFLAGS += -lboost_program_options -lboost_filesystem
LDFLAGS += -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR)
LDFLAGS += -L../../build/lib
LDFLAGS += -L../../build/tokenizer/lib
LDFLAGS += -ltokenizers_cpp -ltokenizers_c -l sentencepiece
LDFLAGS += -lphi4_npu -ldequant -lgemm -llm_head -lq4_npu_eXpress -lnpu_utils



TEST_SOURCES += test.cpp
TEST_SOURCES += ../../common/AutoModel/automodel.cpp
TEST_SOURCES += ../../common/AutoModel/modeling_phi4.cpp
TEST_SOURCES += ../../common/tokenizer/tokenizer.cpp
TEST_SOURCES += ../../common/modules/sampler.cpp


HEADERS += ../../include/phi4/phi4_npu.hpp
HEADERS += ../../include/modules/embedding.hpp
HEADERS += ../../include/modules/lm_head.hpp
HEADERS += ../../include/modules/gemm.hpp
HEADERS += ../../include/modules/dequant.hpp

TEST_DEPS := $(test.cpp:.cpp=.d)

all: directories $(BIN_DIR)/test_phi4_npu

directories:
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/test_phi4_npu: $(TEST_SOURCES) $(TEST_DEPS)
	$(CXX) $(TEST_FLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

test: $(BIN_DIR)/test_phi4_npu
	cp $(LIB_DIR)/libphi4_npu.so $(BIN_DIR)
	cp ../../../model_list_linux.json $(BIN_DIR)/model_list.json
	cd $(BIN_DIR) && export LD_LIBRARY_PATH=../../../../build/lib && ./test_phi4_npu --model phi4-mini-it -s 0

-include $(TEST_DEPS)
.PHONY: all clean test directories

else

# WSL build environment
# Use CMake to invoke the Visual Studio
PWSH := powershell.exe


TARGET = $(BUILD_DIR)/phi4_npu.dll

all: directories test

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(BIN_DIR)/xclbins
	mkdir -p $(LIB_DIR)

SOURCES += $(wildcard ../../common/AutoModel/automodel.cpp)
SOURCES += $(wildcard ../../common/AutoModel/modeling_phi4.cpp)
SOURCES += $(wildcard ../../include/AutoModel/automodel.hpp)
SOURCES += $(wildcard ../../include/AutoModel/modeling_phi4.hpp)
SOURCES += $(wildcard ../../include/phi4/phi4_npu.hpp)
SOURCES += $(wildcard ../../common/tokenizer/tokenizer.cpp)
SOURCES += $(wildcard ../../common/modules/sampler.cpp)
SOURCES += $(wildcard test.cpp)

$(BIN_DIR)/test_phi4_npu.exe: $(SOURCES)
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake ../../../test/phi4_npu"
	cd $(BUILD_DIR) && $(PWSH) -Command "cmake --build . --config Release --target test_phi4_npu_target"

clean:
	rm -rf $(BUILD_DIR)


test: directories $(BIN_DIR)/test_phi4_npu.exe
	cp ../../lib/*.dll $(BIN_DIR)
	cp -r ../../xclbins/Phi4* $(BIN_DIR)/xclbins/
	cp ../../model_list.json $(BIN_DIR)/model_list.json
	cd $(BIN_DIR) && ${PWSH} -Command ".\test_phi4_npu.exe -m phi4-mini-it -s 1 -p 0"

.PHONY: all clean test directories

endif 
