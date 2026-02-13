cmake_minimum_required(VERSION 3.22)
project(gemma_embedding VERSION 1.0.0 LANGUAGES CXX)

# Set build type to Release
set(CMAKE_BUILD_TYPE Release)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force x64 architecture on Windows
if(WIN32)
    set(CMAKE_GENERATOR_PLATFORM x64)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/gemma_embedding/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/gemma_embedding/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../build/gemma_embedding/lib)

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
set(TEST_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/../../build/gemma_embedding/test/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TEST_OUTPUT_DIR})

# Create test executable with basic settings
add_executable(test_gemma_embedding
    test.cpp
    ${HEADERS}
    ${CMAKE_SOURCE_DIR}/../../common/AutoEmbeddingModel/auto_embedding_model.cpp
    ${CMAKE_SOURCE_DIR}/../../common/AutoEmbeddingModel/modeling_gemma_embedding.cpp
    ${CMAKE_SOURCE_DIR}/../../common/tokenizer/tokenizer.cpp
)

# add_subdirectory(${CMAKE_SOURCE_DIR}/../../tokenizer_cpp tokenizers EXCLUDE_FROM_ALL)
# Apply the same AVX2 settings to the test executable
target_compile_definitions(test_gemma_embedding PUBLIC
    DISABLE_ABI_CHECK=1
    USEAVX2=1
    _ENABLE_EXTENDED_ALIGNED_STORAGE
    __WINDOWS__
)

target_compile_options(test_gemma_embedding PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2 /O2 /fp:fast>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-mavx2 -mfma -march=native -O2>
)

if(MSVC)
    set_target_properties(test_gemma_embedding PROPERTIES
        LINK_FLAGS "/MACHINE:X64"
    )
endif()

# Basic include directories for test
target_include_directories(test_gemma_embedding PUBLIC
    ${CMAKE_SOURCE_DIR}/../../include
    ${XRT_INCLUDE_DIR}
    C:/dev/boost_1_88_0
    C:/dev/vcpkg/installed/x64-windows/include/
)

# Basic link settings for test
target_link_directories(test_gemma_embedding PUBLIC
    ${CMAKE_SOURCE_DIR}/../../lib
    ${XRT_LIB_DIR}
    ${CMAKE_SOURCE_DIR}/../../build/tokenizer/lib
    C:/dev/boost_1_88_0/stage/lib
    C:/dev/vcpkg/installed/x64-windows/lib
)

target_link_libraries(test_gemma_embedding PUBLIC
    gemma_embedding
    q4_npu_eXpress
    dequant
    gemm
    xrt_coreutil
)
target_link_libraries(test_gemma_embedding PRIVATE
    tokenizers_cpp
    tokenizers_c
    sentencepiece
    ntdll wsock32 ws2_32 Bcrypt
    iphlpapi userenv psapi
)

# Add test target
add_custom_target(test_gemma_embedding_target
    DEPENDS test_gemma_embedding
    COMMENT "Building test_gemma_embedding executable"
)

