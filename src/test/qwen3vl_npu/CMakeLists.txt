cmake_minimum_required(VERSION 3.22)
project(qwen3vl_npu VERSION 1.0.0 LANGUAGES CXX)

# Set build type to Release
set(CMAKE_BUILD_TYPE Release)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and enable OpenMP for multi-threading
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found - enabling multi-threading support for test")
else()
    message(WARNING "OpenMP not found - some optimizations will run single-threaded")
endif()

# Force x64 architecture on Windows
if(WIN32)
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/qwen3vl_npu/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/qwen3vl_npu/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/qwen3vl_npu/lib)

# Force output directories to be absolute
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# ———————————————————————————————————————————————
# User-tweakable XRT settings (override on cmake command-line)
# ———————————————————————————————————————————————
set(XRT_INCLUDE_DIR C:/dev/XRT/src/runtime_src/core/include CACHE PATH   "Where XRT headers live")
set(XRT_LIB_DIR     C:/dev/xrtNPUfromDLL     CACHE PATH   "Where XRT libs live")

# ———————————————————————————————————————————————
# Test executable target
# ———————————————————————————————————————————————
# Set test output directory
set(TEST_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/../../build/qwen3vl_npu/test/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TEST_OUTPUT_DIR})

# Create test executable with basic settings
add_executable(test_qwen3vl_npu
    test.cpp
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}/../../common/AutoModel/automodel.cpp
    ${CMAKE_SOURCE_DIR}/../../common/AutoModel/modeling_qwen3vl.cpp
    ${CMAKE_SOURCE_DIR}/../../common/AutoModel/modeling_qwen3vl_image.cpp
    ${CMAKE_SOURCE_DIR}/../../common/tokenizer/tokenizer.cpp
    ${CMAKE_SOURCE_DIR}/../../common/modules/sampler.cpp
    ${CMAKE_SOURCE_DIR}/../../common/image_process_utils/imageproc.cpp
    ${CMAKE_SOURCE_DIR}/../../common/image_process_utils/imageprocAVX512.cpp
)

# add_subdirectory(${CMAKE_SOURCE_DIR}/../../tokenizer_cpp tokenizers EXCLUDE_FROM_ALL)
# Apply the same AVX2 settings to the test executable
target_compile_definitions(test_qwen3vl_npu PUBLIC
    DISABLE_ABI_CHECK=1
    USEAVX2=1
    USEAVX512=1
    _ENABLE_EXTENDED_ALIGNED_STORAGE
    __WINDOWS__
)

# Optimization flags with AVX-512 support
target_compile_options(test_qwen3vl_npu PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX512 /O2 /fp:fast>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-mavx512f -mavx512dq -mavx512bw -mavx512vl -march=native -O3 -ffast-math>
)

# Add OpenMP compiler flags if available
if(OpenMP_CXX_FOUND)
    target_compile_options(test_qwen3vl_npu PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:/openmp>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fopenmp>
    )
endif()

if(MSVC)
    set_target_properties(test_qwen3vl_npu PROPERTIES
        LINK_FLAGS "/MACHINE:X64"
    )
endif()

# Basic include directories for test
target_include_directories(test_qwen3vl_npu PUBLIC
    ${CMAKE_SOURCE_DIR}/../../include
    ${XRT_INCLUDE_DIR}
    C:/dev/boost_1_88_0
    C:/dev/vcpkg/installed/x64-windows/include/
)

# Basic link settings for test
target_link_directories(test_qwen3vl_npu PUBLIC
    ${CMAKE_SOURCE_DIR}/../../lib
    ${XRT_LIB_DIR}
    ${CMAKE_SOURCE_DIR}/../../build/tokenizer/lib
    C:/dev/boost_1_88_0/stage/lib
    C:/dev/vcpkg/installed/x64-windows/lib
)

target_link_libraries(test_qwen3vl_npu PUBLIC
    qwen3vl_npu
    q4_npu_eXpress
    lm_head
    dequant
    gemm
    xrt_coreutil
    avformat
    avcodec
    avutil
    swscale
    zlib
)

# Link OpenMP if available
if(OpenMP_CXX_FOUND)
    target_link_libraries(test_qwen3vl_npu PUBLIC OpenMP::OpenMP_CXX)
endif()

target_link_libraries(test_qwen3vl_npu PRIVATE
    tokenizers_cpp
    tokenizers_c
    sentencepiece
    ntdll wsock32 ws2_32 Bcrypt
    iphlpapi userenv psapi
)

# Add test target
add_custom_target(test_qwen_npu_target
    DEPENDS test_qwen3vl_npu
    COMMENT "Building test_qwen3vl_npu executable"
)

