#CMake file for VirtualListDemo

set(source_files Main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(VirtualListDemo ${source_files})
add_dependencies(VirtualListDemo duilib)
target_link_libraries(VirtualListDemo duilib)
set_target_properties(VirtualListDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

install(
  TARGETS VirtualListDemo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
