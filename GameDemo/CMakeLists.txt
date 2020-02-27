#Cmake file for GameDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

set(source_files GameDemo.cpp MiniDumper.cpp stdAfx.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(GameDemo ${source_files})
target_link_libraries(GameDemo duilib)
set_target_properties(GameDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(GameDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install executable
install(TARGETS GameDemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
