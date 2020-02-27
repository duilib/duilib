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
			${CMAKE_CURRENT_SOURCE_DIR}/.
        DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}"
	    FILES_MATCHING PATTERN "*.h"
    )

# Install cmake files
configure_file(${CMAKE_SOURCE_DIR}/CMake/DuilibConfig.cmake.in ${CMAKE_BINARY_DIR}/DuilibConfig.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/DuilibConfig.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/CMake")
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/DuilibConfigConfigVersion.cmake"
    VERSION ${BUILD_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_BINARY_DIR}/DuilibConfigConfigVersion.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/CMake")
