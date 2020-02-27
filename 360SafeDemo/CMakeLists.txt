#Cmake file for 360SafeDemo
#Author: Qi Gao(monkgau@gmail.com)
#Date: 2012/9/17

set(source_files 360safe.cpp stdafx.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(360safedemo ${source_files})
set_target_properties(360safedemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(360safedemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(360safedemo duilib)

# Install executable
install(TARGETS 360safedemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
