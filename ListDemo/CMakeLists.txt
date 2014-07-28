#Cmake file for ListDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

set(source_files Main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(ListDemo ${source_files})
target_link_libraries(ListDemo duilib)
set_target_properties(ListDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

add_custom_command(TARGET ListDemo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${PROJECT_BINARY_DIR}/bin/ListDemo.exe ${PROJECT_SOURCE_DIR}/bin/ListDemo.exe)
