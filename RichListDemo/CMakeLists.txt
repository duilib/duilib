#Cmake file for RichListDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})                           

add_executable(RichListDemo ${SRC_LIST} RichListDemo.rc)
set_target_properties(RichListDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(RichListDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(RichListDemo duilib)

# Install executable
install(TARGETS RichListDemo
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
