#Cmake file for QQDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})                           

add_executable(QQDemo ${SRC} QQDemo.rc)
set_target_properties(QQDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(QQDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(QQDemo duilib)

# Install executable
install(TARGETS QQDemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
