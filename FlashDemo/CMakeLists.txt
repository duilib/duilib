#Cmake file for FlashDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

set(source_files App.cpp StdAfx.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(FlashDemo ${source_files})
add_dependencies(FlashDemo duilib)
target_link_libraries(FlashDemo duilib)
set_target_properties(FlashDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

install(
  TARGETS FlashDemo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
