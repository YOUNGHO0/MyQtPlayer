cmake_minimum_required(VERSION 3.28)
project(myQtPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# myQtPlayer_SOURCE_DIR의 값을 출력
message(STATUS "myQtPlayer_SOURCE_DIR: ${myQtPlayer_SOURCE_DIR}")

# FFmpeg 경로 설정 (상대 경로 예시)
set(FFMPEG_INCLUDE_DIR "/opt/homebrew/lib/include")
set(FFMPEG_LIB_DIR "/opt/homebrew/lib")

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        REQUIRED)

add_executable(myQtPlayer main.cpp
        VideoPlayer.h
        VideoPlayer.cpp)
target_link_libraries(myQtPlayer
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        Qt::MultimediaWidgets
        avcodec
        avformat
        avutil
        swscale
)





