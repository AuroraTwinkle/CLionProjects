cmake_minimum_required(VERSION 3.14)
project(linuxTest)

set(CMAKE_CXX_STANDARD 11)
link_libraries(dl)

#add_library(test SHARED a1.cpp a2.cpp)
#add_library(test1 SHARED function1.cpp)
#add_library(test2 SHARED function.cpp)

add_executable(linuxTest main.cpp CPluginEnumerator.cpp CPluginEnumerator.h CPluginController.h CPluginController.cpp IPrintPlugin.h IPrintPlugin.cpp)
#add_executable(linuxTest test.cpp a.h a2.cpp a1.cpp)
target_link_libraries(linuxTest dl)