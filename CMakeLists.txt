cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RFS)

add_library(RFS STATIC rfs/rfs.c)

project(Encoder)
add_executable(Encoder encoder/main.cpp)
target_link_libraries(Encoder RFS)

project(Browser)
add_custom_command(
  OUTPUT entries
  COMMAND Encoder "${CMAKE_SOURCE_DIR}/test_folder" entries
  DEPENDS Encoder "${CMAKE_SOURCE_DIR}/test_folder"
)
add_custom_command(
  OUTPUT entries.o
  COMMAND ld -r -b binary -o entries.o entries
  DEPENDS entries
)
add_executable(Browser browser/main.cpp entries.o)
target_link_libraries(Browser RFS)

include_directories(rfs)