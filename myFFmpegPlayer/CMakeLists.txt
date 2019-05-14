cmake_minimum_required(VERSION 3.14)
project(myFFmpegPlayer)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/ffmpeg/include /usr/local/include/SDL2)
link_directories(/usr/local/ffmpeg/lib /usr/local/lib)
link_libraries(avformat avcodec avutil SDL2 postproc swresample pthread)

add_executable(myFFmpegPlayer main.cpp Demuxer.cpp Demuxer.h MyLog.cpp MyLog.h PacketQueue.cpp PacketQueue.h Decoder.cpp Decoder.h SDLDisplay.cpp SDLDisplay.h AudioConverter.cpp AudioConverter.h)

target_link_libraries(myFFmpegPlayer avformat avcodec avutil SDL2 postproc swresample pthread)