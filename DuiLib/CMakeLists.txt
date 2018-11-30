# cmake file for duilib
#Author Qi Gao(monkgau@gmail.com)
#Created: 2012/09/16

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} Root_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Control Control_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Core Core_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Layout Layout_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils Utils_src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Control)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Layout)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Utils)

add_library(duilib ${Control_src} ${Core_src} ${Layout_src} ${Utils_src} ${Root_src})

add_definitions(-DUILIB_EXPORTS)
target_link_libraries(duilib comctl32)
set_target_properties(duilib PROPERTIES OUTPUT_NAME "duilib")
set_target_properties(duilib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(duilib PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Install library
install(TARGETS duilib
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/Control
			${CMAKE_CURRENT_SOURCE_DIR}/Core
			${CMAKE_CURRENT_SOURCE_DIR}/Layout
			${CMAKE_CURRENT_SOURCE_DIR}/Utils
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}"
	    FILES_MATCHING PATTERN "*.h"
    )
