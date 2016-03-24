cmake_minimum_required(VERSION 3.3)
project(DirectUIDemo)

remove_definitions(-DDIRECTUI_EXPORTS)
link_directories(${THIRD_PARTY_DIRECTORY}/vld/lib)
aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY "${RUNTIME_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY "${ARCHIVE_OUTPUT_DIRECTORY}"
    DEBUG_POSTFIX "d"
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${THIRD_PARTY_DIRECTORY}/vld/include
    ${INCLUDE_OUTPUT_DIRECTORY}
)
add_dependencies(${PROJECT_NAME} DirectUI)
target_link_libraries(${PROJECT_NAME} DirectUI)
