#Cmake file for FlashDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

set(source_files App.cpp StdAfx.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(FlashDemo ${source_files})
target_link_libraries(FlashDemo duilib)
set_target_properties(FlashDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(FlashDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install executable
install(TARGETS FlashDemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )