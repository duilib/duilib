#Cmake file for ListDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

set(source_files Main.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ListDemo ${source_files})
target_link_libraries(ListDemo duilib)
set_target_properties(ListDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(ListDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install executable
install(TARGETS ListDemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
