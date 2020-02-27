#Cmake file for MenuDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})                           

add_executable(MenuDemo ${SRC} MenuDemo.rc)
set_target_properties(MenuDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(MenuDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(MenuDemo duilib)
