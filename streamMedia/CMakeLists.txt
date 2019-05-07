cmake_minimum_required(VERSION 3.14)
project(streamMedia)

set(CMAKE_CXX_STANDARD 11)

include_directories(/aurora/ffmpeg/include /usr/local/include/SDL2)
link_directories(/aurora/ffmpeg/lib /usr/local/lib)
link_libraries(avformat avcodec avutil SDL2 swscale )

add_executable(streamMedia myplayer.cpp testFFmpeg.cpp)

target_link_libraries(streamMedia avformat avcodec avutil SDL2 swscale)

