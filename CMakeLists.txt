cmake_minimum_required(VERSION 3.3)
project(duilib)

option(DUILIB_STATICLIB "Set to ON to build duilib with static linking." OFF)
option(DUILIB_BUILD_DEMOS "Build the demos" ON)

add_definitions(-DUNICODE -D_UNICODE)

set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
)
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set(THIRD_PARTY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(INCLUDE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/include")
set(RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
add_subdirectory(src)

if (DUILIB_BUILD_DEMOS)
    add_subdirectory(demos/DirectUIDemo)
endif()
