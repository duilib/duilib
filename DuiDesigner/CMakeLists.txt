#Cmake file for DuiDesigner 
#Author: Qi Gao(monkgau@gmail.com)
#Date: 2012/9/17


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyxml TINYXML_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyxml/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(DuiDesigner WIN32 ${SRC} ${TINYXML_SRC} DuiDesigner.rc)



set_target_properties(DuiDesigner PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

#used for mfc project
add_definitions(-D_AFXDLL)
set_target_properties(DuiDesigner PROPERTIES COMPILE_DEFINITIONS _BIND_TO_CURRENT_CRT_VERSION,_BIND_TO_CURRENT_MFC_VERSION
                                  LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")
target_link_libraries(DuiDesigner duilib)


add_custom_command(TARGET DuiDesigner POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${PROJECT_BINARY_DIR}/bin/DuiDesigner.exe ${PROJECT_SOURCE_DIR}/bin/DuiDesigner.exe)

