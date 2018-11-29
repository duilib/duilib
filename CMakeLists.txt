# CMake root file for duilib related apps
#Date 2012-09-16
#Author: Qi Gao(monkgau@gmail.com)
#
#  CMake files for duilib
#  file for each app is under the related sub-dir
#

cmake_minimum_required(VERSION 2.8)

project(dui)

message(STATUS," CMake project files for duilib")

include(GNUInstallDirs OPTIONAL)

# this line is for UNICODE release,which is required by DuiDesigner 
add_definitions(-DUNICODE -D_UNICODE)

IF(MSVC)
    # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
    OPTION(WIN32_USE_MP "Set to ON to build with the /MP option (Visual Studio 2005 and above)." ON)
    MARK_AS_ADVANCED(WIN32_USE_MP)
    IF(WIN32_USE_MP)
        add_compile_options(/MP)
    ENDIF(WIN32_USE_MP)
ENDIF(MSVC)

# add each CMake file
add_subdirectory(duilib)

OPTION(BUILD_DEMO "Build demos" ON)
if(BUILD_DEMO)
	add_subdirectory(360SafeDemo)
	add_subdirectory(FlashDemo)
	add_subdirectory(GameDemo)
	add_subdirectory(ListDemo)
	#add_subdirectory(MenuDemo) #the sources in this project has error
	add_subdirectory(QQDemo)
	add_subdirectory(RichListDemo)
endif(BUILD_DEMO)
