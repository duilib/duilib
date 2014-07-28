#Readme file for CMake 
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/19

1. Open SDK or VS Command prompts at where your duilib located,eg: d:\dev\duilib
2. create a directory for build. eg: mkdir build
3. begin to create makefiles at build dir. eg: d:\dev\cmake\bin\cmake.exe ..
4. build all apps. eg: nmake
5. enjoy your dui apps at bin directory. eg: d:\dev\duilib\bin

process listed have been tested under vc2010 and winsdk7.1

ps: for a successful cmake build,you may need to delete the lib link code from stdafx.h of each project.
