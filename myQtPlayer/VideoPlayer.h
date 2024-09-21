#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QApplication>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAudioOutput>
#include <QSlider>
#include <QTimer>
#include <QListWidget>
#include <QFileDialog>
#include <QMap>
#include <QDir>
#include <QStyle>
#include <QFileInfo>
#include <QFileInfoList>

class VideoPlayer : public QMainWindow {
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = nullptr);

private:
    void addVideoToList(const QString &filePath);

    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
    QAudioOutput *audioOutput;
    QTimer *timer;
    QListWidget *fileListWidget;
    QPushButton *playButton;
    QPushButton *pauseButton;  // 일시 정지 버튼
    QPushButton *forwardButton;
    QPushButton *rewindButton;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *stopButton;  // 중지 버튼 추가
    QMap<QString, QString> *fileMap;  // QMap을 포인터로 선언
    QSlider *slider;
    bool isPaused;  // 동영상 일시 정지 상태를 추적하는 플래그

public slots:
    void playVideo();
    void stopVideo();
    void pauseVideo();
    void handleAddButtonClick();
    void handleRemoveButtonClick();
    void playSelectedFile(QListWidgetItem *item);
    void setPosition(int position);
    void updateSlider();
    void fastForward();
    void rewind();
};

#endif // VIDEOPLAYER_H
