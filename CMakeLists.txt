cmake_minimum_required(VERSION 3.0.0)

project(chip8 VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SRCS 
        ${SRC_DIR}/main.c
        ${SRC_DIR}/cpu.c 
        ${SRC_DIR}/ins.c 
        ${SRC_DIR}/io.c 
        ${SRC_DIR}/logging.c)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(DEPS ${CMAKE_SOURCE_DIR}/deps/win64)
    else(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(DEPS ${CMAKE_SOURCE_DIR}/deps/win32)
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

    set(SDL2DIR ${DEPS}/SDL2-2.0.14)
    set(LIBS "")

    add_executable(chip8 WIN32 ${SRCS})
else(WIN32)
    add_executable(chip8 ${SRCS})
    set(LIBS m)        
endif(WIN32)

find_package(SDL2 REQUIRED)
list(APPEND LIBS ${SDL2_LIBRARY})

include_directories(
    ${SDL2_INCLUDE_DIR}
    ${SRC_DIR}
) 
target_link_libraries(chip8 PRIVATE ${LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
