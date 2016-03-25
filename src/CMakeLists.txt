cmake_minimum_required(VERSION 3.3)
project(DirectUI)

if (DUILIB_STATIC)
    set(DUILIB_LIBRARY_TYPE STATIC)
    message(FATAL_ERROR "Not supported to build as static library")
else()
    add_definitions(-DDIRECTUI_EXPORTS)
    set(DUILIB_LIBRARY_TYPE SHARED)
    set(USE_DUILIB_STATICLIB 0)
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Base SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Control SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Define SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Layout SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Render SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/TinyXml SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Utils SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Window SRC_LIST)

add_library(${PROJECT_NAME} ${DUILIB_LIBRARY_TYPE} ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${THIRD_PARTY_DIRECTORY}/vld/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY "${ARCHIVE_OUTPUT_DIRECTORY}"
    DEBUG_POSTFIX "d"
    IMPORT_SUFFIX "_imp.lib"
)
if (DUILIB_STATICLIB)
    set_target_properties(${PROJECT_NAME} PROPERTIES IMPORT_SUFFIX "_imp.lib")
endif()
string(REPLACE "/" "\\" COPY_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
string(REPLACE "/" "\\" COPY_DESTITION "${INCLUDE_OUTPUT_DIRECTORY}/duilib/")
message("xcopy ${COPY_SOURCE} ${COPY_DESTITION}")
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND xcopy "${COPY_SOURCE}" "${COPY_DESTITION}" /S /Y /D
)
