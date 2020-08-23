cmake_minimum_required(VERSION 3.13)
# -----------------------------------------------------------------------------
# 环境配置
# -----------------------------------------------------------------------------
project(parser)
## 编译器
set(GCC_TOOLCHAIN /data/server/gcc CACHE PATH "Compiler: gcc compiler prefix")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
## CLANG 编译器（指定 GCC 工具链）
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options("--gcc-toolchain=${GCC_TOOLCHAIN}")
    add_link_options("--gcc-toolchain=${GCC_TOOLCHAIN}")
endif()

# -----------------------------------------------------------------------------
# 项目
# -----------------------------------------------------------------------------
set(CMAKE_STATIC_LIBRARY_PREFIX "libparser_")
# -----------------------------------------------------------------------------
# 项目 - keyvalue
# -----------------------------------------------------------------------------
set(KEYVALUE_SOURCES
    src/keyvalue.cpp
    src/keyvalue_parser.c
    src/keyvalue_parser.cpp)
set(KEYVALUE_HEADERS
    src/keyvalue.hpp
    src/keyvalue_parser.h
    src/keyvalue_parser.hpp)
add_library(keyvalue STATIC ${KEYVALUE_SOURCES})
set_property(TARGET keyvalue PROPERTY PUBLIC_HEADER ${KEYVALUE_HEADERS})
install(TARGETS keyvalue LIBRARY
    PUBLIC_HEADER DESTINATION "include/parser")

# 测试原始解析器
add_executable(keyvalue_test_raw
    test/keyvalue_raw.cpp
    src/keyvalue_parser.c)
target_include_directories(keyvalue_test_raw
    PUBLIC ${CMAKE_SOURCE_DIR})
# 测试上层包装
add_executable(keyvalue_test test/keyvalue.cpp)
target_include_directories(keyvalue_test
    PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(keyvalue_test keyvalue)

# -----------------------------------------------------------------------------
# 项目 - multipart
# -----------------------------------------------------------------------------
set(MULTIPART_PARSER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/multipart_core.c
    ${CMAKE_SOURCE_DIR}/src/multipart_parser.c)
set(MULTIPART_PARSER_HEADERS
    ${CMAKE_SOURCE_DIR}/src/multipart_core.h
    ${CMAKE_SOURCE_DIR}/src/multipart_parser.h)
add_custom_command(OUTPUT ${MULTIPART_PARSER_SOURCES} ${MULTIPART_PARSER_HEADERS}
    COMMAND /data/server/node/bin/node ${CMAKE_SOURCE_DIR}/build.js multipart
    DEPENDS ${CMAKE_SOURCE_DIR}/src/multipart.js)
set(MULTIPART_SOURCES src/multipart.cpp src/multipart_parser.cpp)
set(MULTIPART_HEADERS src/multipart.hpp src/multipart_parser.hpp)
add_library(multipart STATIC
    ${MULTIPART_PARSER_SOURCES}
    ${MULTIPART_SOURCES})
target_include_directories(multipart
    PUBLIC ${CMAKE_SOURCE_DIR})
set_property(TARGET multipart PROPERTY PUBLIC_HEADER
    ${MULTIPART_PARSER_HEADERS}
    ${MULTIPART_HEADERS})
install(TARGETS multipart LIBRARY
    PUBLIC_HEADER DESTINATION "include/parser")
# 测试原始解析过程
add_executable(multipart_test_raw test/multipart_raw.cpp ${MULTIPART_PARSER_SOURCES})
target_include_directories(multipart_test_raw
    PUBLIC ${CMAKE_SOURCE_DIR})
# 测试上层封装
add_executable(multipart_test test/multipart.cpp)
target_include_directories(multipart_test
    PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(multipart_test multipart)
