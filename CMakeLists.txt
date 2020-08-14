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
## multipart
# -----------------------------------------------------------------------------
set(MULTIPART_SOURCES ${CMAKE_SOURCE_DIR}/gen/multipart_core.c ${CMAKE_SOURCE_DIR}/gen/multipart_parser.c)
set(MULTIPART_HEADERS ${CMAKE_SOURCE_DIR}/gen/multipart_core.h ${CMAKE_SOURCE_DIR}/gen/multipart_parser.h)
add_custom_command(OUTPUT ${MULTIPART_SOURCES} ${MULTIPART_HEADERS}
    COMMAND /data/server/node/bin/node ${CMAKE_SOURCE_DIR}/build.js multipart
    DEPENDS ${CMAKE_SOURCE_DIR}/src/multipart/multipart.js)
add_library(multipart STATIC ${MULTIPART_SOURCES} src/multipart/multipart.cpp)
set_property(TARGET multipart PROPERTY PUBLIC_HEADER ${MULTIPART_HEADERS})
install(TARGETS multipart LIBRARY
    PUBLIC_HEADER DESTINATION "include/parser")

add_executable(multipart_test_raw test/multipart_raw.cpp ${MULTIPART_SOURCES})
target_include_directories(multipart_test_raw
    PUBLIC ${CMAKE_SOURCE_DIR}/gen)

add_executable(multipart_test test/multipart.cpp)
target_include_directories(multipart_test
        PUBLIC ${CMAKE_SOURCE_DIR}/gen
        PUBLIC ${CMAKE_SOURCE_DIR}/src/multipart)
target_link_libraries(multipart_test multipart)
## keyvalue
# -----------------------------------------------------------------------------
set(KEYVALUE_SOURCES src/keyvalue/keyvalue.cpp src/keyvalue/keyvalue_parser.c)
set(KEYVALUE_HEADERS src/keyvalue/keyvalue.h src/keyvalue/keyvalue_parser.h)
add_library(keyvalue STATIC ${KEYVALUE_SOURCES})
set_property(TARGET keyvalue PROPERTY PUBLIC_HEADER ${KEYVALUE_HEADERS})
install(TARGETS keyvalue LIBRARY
    PUBLIC_HEADER DESTINATION "include/parser")

# 测试原始解析器
add_executable(keyvalue_test_raw test/keyvalue_raw.cpp src/keyvalue/keyvalue_parser.c)
target_include_directories(keyvalue_test_raw
    PUBLIC ${CMAKE_SOURCE_DIR}/src/keyvalue)
# 测试上层包装
add_executable(keyvalue_test test/keyvalue.cpp)
target_include_directories(keyvalue_test
    PUBLIC ${CMAKE_SOURCE_DIR}/src/keyvalue)
target_link_libraries(keyvalue_test keyvalue)

## toml
# -----------------------------------------------------------------------------
set(TOML_CORE_SOURCES ${CMAKE_SOURCE_DIR}/gen/toml_parser.c)
set(TOML_CORE_HEADERS ${CMAKE_SOURCE_DIR}/gen/toml_parser.h ${CMAKE_SOURCE_DIR}/toml.h)
add_custom_command(OUTPUT ${TOML_CORE_SOURCES} ${TOML_CORE_HEADERS}
    COMMAND /data/server/node/bin/node ${CMAKE_SOURCE_DIR}/build.js toml
    DEPENDS
    ${CMAKE_SOURCE_DIR}/src/toml/toml.js
    ${CMAKE_SOURCE_DIR}/src/toml/toml_core.js
    ${CMAKE_SOURCE_DIR}/src/toml/toml_string.js)
add_library(toml_core STATIC ${TOML_CORE_SOURCES})
set_property(TARGET toml_core PROPERTY PUBLIC_HEADER ${TOML_CORE_HEADERS})
install(TARGETS toml_core LIBRARY
    PUBLIC_HEADER DESTINATION "include/parser")

add_executable(test_toml test/toml.cpp)
target_include_directories(test_toml
    PUBLIC ${CMAKE_SOURCE_DIR}/gen)
target_link_libraries(test_toml toml_core)