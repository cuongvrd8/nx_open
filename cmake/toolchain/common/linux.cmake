## Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

if("${COMPILE_TARGET}" STREQUAL "")
    message(FATAL_ERROR "COMPILE_TARGET must be set before including this file.")
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILE_TARGET ${COMPILE_TARGET})
set(CMAKE_CXX_COMPILE_TARGET ${COMPILE_TARGET})

if("$ENV{TOOLCHAIN_DIR}" STREQUAL "")
    message(FATAL_ERROR
        "TOOLCHAIN_DIR environment variable is not set. Check cmake/conan_dependencies.cmake.")
endif()

if("$ENV{USE_CLANG}")
    if("$ENV{CLANG_DIR}" STREQUAL "")
        message(FATAL_ERROR
            "CLANG_DIR environment variable is not set. Check cmake/conan_dependencies.cmake.")
    endif()

    set(CMAKE_C_COMPILER $ENV{CLANG_DIR}/bin/clang CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER $ENV{CLANG_DIR}/bin/clang++ CACHE INTERNAL "")

    set(flags "--gcc-toolchain=$ENV{TOOLCHAIN_DIR}")
    string(APPEND flags " --sysroot=$ENV{TOOLCHAIN_DIR}/${COMPILE_TARGET}/sysroot")
    string(APPEND flags " --target=${COMPILE_TARGET}")
    set(CMAKE_C_FLAGS ${flags})
    set(CMAKE_CXX_FLAGS ${flags})
else()
    set(cross_prefix $ENV{TOOLCHAIN_DIR}/bin/${COMPILE_TARGET})
    set(CMAKE_C_COMPILER "${cross_prefix}-gcc" CACHE INTERNAL "")
    set(CMAKE_CXX_COMPILER "${cross_prefix}-g++" CACHE INTERNAL "")
endif()
