cmake_minimum_required(VERSION 3.23)
project(FaultLineClangTidyPlugin LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CTest for automated test execution
enable_testing()

# Find LLVM and Clang packages
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

# Include LLVM CMake modules
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Collect include directories
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

# Look for clang-tidy header directory explicitly if separate
find_path(
    CLANG_TIDY_INCLUDE_DIR
    NAMES clang-tidy/ClangTidy.h
    HINTS
        ${CLANG_INCLUDE_DIRS}
        ${LLVM_INCLUDE_DIRS}
        /usr/include
        /usr/local/include
)
if(CLANG_TIDY_INCLUDE_DIR)
    include_directories(SYSTEM ${CLANG_TIDY_INCLUDE_DIR})
endif()

add_definitions(${LLVM_DEFINITIONS})

# Match LLVM compile options (e.g., RTTI disabled if LLVM was built without RTTI)
if(NOT LLVM_ENABLE_RTTI)
    if(MSVC)
        add_compile_options(/GR-)
    else()
        add_compile_options(-fno-rtti)
    endif()
endif()

add_library(FaultLine MODULE)
target_include_directories(
    FaultLine
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_sources(
    FaultLine
    PRIVATE
        src/CallerMissingExceptionAttributeCheck.cpp
        src/FaultLineModule.cpp
        src/MissingExceptionAttributeCheck.cpp
        src/UselessExceptionAttributeCheck.cpp
        src/utilities.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS include
            FILES
                include/clang/tidy/fault_line/CallerMissingExceptionAttributeCheck.hpp
                include/clang/tidy/fault_line/MissingExceptionAttributeCheck.hpp
                include/clang/tidy/fault_line/UselessExceptionAttributeCheck.hpp
                include/clang/tidy/fault_line/utilities.hpp
)

# Disable "lib" prefix on Windows/macOS if desired, keeping standard library output
set_target_properties(
    FaultLine
    PROPERTIES CXX_VISIBILITY_PRESET default VISIBILITY_INLINES_HIDDEN OFF
)

option(
    FAULT_LINE_INCLUDE_TESTS
    "If FaultLine tests should be included"
    ${PROJECT_IS_TOP_LEVEL}
)
if(FAULT_LINE_INCLUDE_TESTS)
    add_subdirectory(test)
endif()
