extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

#include <QApplication>
#include <QPushButton>
#include "VideoPlayer.h"



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    VideoPlayer player;
    player.resize(800, 600);
    player.show();

    return app.exec();
}
