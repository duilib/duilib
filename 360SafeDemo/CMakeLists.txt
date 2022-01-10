#Cmake file for 360SafeDemo
#Author: Qi Gao(monkgau@gmail.com)
#Date: 2012/9/17

set(source_files 360safe.cpp stdafx.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(360safedemo ${source_files})
set_target_properties(360safedemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
add_dependencies(360safedemo duilib)
target_link_libraries(360safedemo duilib)

install(
  TARGETS 360safedemo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
