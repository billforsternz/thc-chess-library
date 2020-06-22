cmake_minimum_required(VERSION 3.17)

project(thc-chess)

# specify the include directory for the outer project
set(THC_CHESS_INCLUDE ${PROJECT_SOURCE_DIR}/src PARENT_SCOPE)
# specify a release build
add_definitions(-DKILL_DEBUG_COMPLETELY)
# gather all sources
file(GLOB THC_CHESS_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
# don't compile twice the unified cpp objects, and remove testing from the final library
list(REMOVE_ITEM THC_CHESS_SRCS ${PROJECT_SOURCE_DIR}/src/thc.cpp ${PROJECT_SOURCE_DIR}/src/thc-regen.cpp ${PROJECT_SOURCE_DIR}/src/test-framework.cpp)
# define both a static and shared library
add_library(thc_chess SHARED ${THC_CHESS_SRCS})
add_library(thc_chess_static STATIC ${THC_CHESS_SRCS})
