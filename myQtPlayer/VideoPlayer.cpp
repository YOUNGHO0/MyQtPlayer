//
// Created by lee on 2024. 8. 24..
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QMainWindow(parent), isPaused(false) {
    // MediaPlayer와 VideoWidget 생성
    mediaPlayer = new QMediaPlayer(this);
    videoWidget = new QVideoWidget();

    // 재생 버튼 생성
    playButton = new QPushButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));  // 아이콘 설정
    connect(playButton, &QPushButton::clicked, this, &VideoPlayer::playVideo);

    // 일시 정지 버튼 생성
    pauseButton = new QPushButton(this);
    pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));  // 아이콘 설정
    connect(pauseButton, &QPushButton::clicked, this, &VideoPlayer::pauseVideo);

    // 앞으로 감기 버튼 생성
    forwardButton = new QPushButton(this);
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));  // 아이콘 설정
    connect(forwardButton, &QPushButton::clicked, this, &VideoPlayer::fastForward);

    // 뒤로 감기 버튼 생성
    rewindButton = new QPushButton(this);
    rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));  // 아이콘 설정
    connect(rewindButton, &QPushButton::clicked, this, &VideoPlayer::rewind);

    // 추가 버튼 생성 (파일 또는 폴더)
    addButton = new QPushButton("Add Video", this);
    connect(addButton, &QPushButton::clicked, this, &VideoPlayer::handleAddButtonClick);

    // 삭제 버튼 생성
    removeButton = new QPushButton("Remove Video", this);
    connect(removeButton, &QPushButton::clicked, this, &VideoPlayer::handleRemoveButtonClick);

    // 중지 버튼 생성
    stopButton = new QPushButton(this);  // 중지 버튼 추가
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));  // 아이콘 설정
    connect(stopButton, &QPushButton::clicked, this, &VideoPlayer::stopVideo);

    // 슬라이더 생성
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    connect(slider, &QSlider::valueChanged, this, &VideoPlayer::setPosition);

    // 파일 리스트 위젯 생성
    fileListWidget = new QListWidget(this);
    connect(fileListWidget, &QListWidget::itemClicked, this, &VideoPlayer::playSelectedFile);

    // 파일 경로를 저장할 QMap 생성
    fileMap = new QMap<QString, QString>();

    // 레이아웃 설정
    QHBoxLayout *mainLayout = new QHBoxLayout;

    QVBoxLayout *leftLayout = new QVBoxLayout;

    // 버튼을 1x2 가로 형태로 배치할 레이아웃
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    leftLayout->addLayout(buttonLayout); // 가로 버튼 레이아웃 추가
    leftLayout->addWidget(fileListWidget); // 파일 리스트 위젯 추가

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(videoWidget);

    // 버튼을 1x5 가로 형태로 배치할 레이아웃
    QHBoxLayout *controlButtonLayout = new QHBoxLayout;
    controlButtonLayout->addWidget(rewindButton);  // 뒤로 감기 버튼 추가
    controlButtonLayout->addWidget(playButton);    // 재생 버튼 추가
    controlButtonLayout->addWidget(stopButton);    // 중지 버튼 추가
    controlButtonLayout->addWidget(pauseButton);   // 일시 정지 버튼 추가
    controlButtonLayout->addWidget(forwardButton); // 앞으로 감기 버튼 추가

    rightLayout->addLayout(controlButtonLayout);  // 1x5 버튼 레이아웃 추가
    rightLayout->addWidget(slider);

    mainLayout->addLayout(leftLayout, 1);  // 왼쪽 부분 비율 1
    mainLayout->addLayout(rightLayout, 4); // 오른쪽 부분 비율 4

    QWidget *container = new QWidget;
    container->setLayout(mainLayout);
    setCentralWidget(container);

    // 미디어 플레이어 설정
    mediaPlayer->setVideoOutput(videoWidget);

    // 오디오 출력 설정
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);

    // 오디오 설정
    audioOutput->setMuted(false);
    audioOutput->setVolume(0.1);

    // 슬라이더 업데이트를 위한 타이머 설정
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoPlayer::updateSlider);
    timer->start(100);
}

void VideoPlayer::playVideo() {
    // 동영상 목록이 비어 있는지 확인
    if (fileListWidget->count() == 0) {
        return;  // 목록이 비어 있으면 아무 것도 실행하지 않음
    }

    if (isPaused) {
        mediaPlayer->play();
        isPaused = false;  // 플래그 리셋
    } else {
        // 현재 선택된 파일이 없으면 리스트의 첫 번째 파일 재생
        if (fileListWidget->currentItem() == nullptr && fileListWidget->count() > 0) {
            fileListWidget->setCurrentRow(0); // 첫 번째 파일 선택
        }

        // 현재 선택된 파일 재생
        QListWidgetItem *currentItem = fileListWidget->currentItem();
        if (currentItem) {
            playSelectedFile(currentItem);
        }
    }
}

void VideoPlayer::stopVideo() {
    mediaPlayer->stop();
    slider->setValue(0);  // 슬라이더를 0으로 초기화
    isPaused = false;  // 일시 정지 상태 리셋
}

void VideoPlayer::pauseVideo() {
    mediaPlayer->pause();
    isPaused = true;  // 플래그 설정
}

void VideoPlayer::handleAddButtonClick() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);  // 파일 및 폴더 선택 가능하게 설정
    dialog.setOption(QFileDialog::ShowDirsOnly, false); // 폴더 포함
    dialog.setFileMode(QFileDialog::ExistingFiles);  // 파일 선택 모드
    dialog.setViewMode(QFileDialog::List);

    if (dialog.exec()) {
        QStringList selectedPaths = dialog.selectedFiles();
        for (const QString &filePath : selectedPaths) {
            QFileInfo fileInfo(filePath);
            if (fileInfo.isDir()) {
                // 폴더의 경우 내부의 모든 비디오 파일 추가
                QDir dir(filePath);
                QStringList filters;
                filters << "*.mp4" << "*.avi" << "*.mov";
                QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
                for (const QFileInfo &fileInfo : fileList) {
                    addVideoToList(fileInfo.filePath());
                }
            } else {
                // 파일인 경우 바로 추가
                addVideoToList(filePath);
            }
        }
    }
}

void VideoPlayer::handleRemoveButtonClick() {
    QListWidgetItem *selectedItem = fileListWidget->currentItem();
    if (selectedItem) {
        QString fileName = selectedItem->text();
        fileMap->remove(fileName);  // 맵에서 파일 경로 제거
        delete fileListWidget->takeItem(fileListWidget->row(selectedItem));  // 선택된 항목 삭제

        // 동영상 목록이 비어 있으면 재생 상태를 리셋
        if (fileListWidget->count() == 0) {
            mediaPlayer->stop();
            isPaused = false;  // 일시 정지 상태 리셋
        }
    }
}

void VideoPlayer::playSelectedFile(QListWidgetItem *item) {
    QString fileName = item->text();
    QString filePath = fileMap->value(fileName);  // 파일명으로 경로를 조회
    if (!filePath.isEmpty()) {
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        mediaPlayer->play(); // 일시 정지된 지점에서 재생 시작
    }
}

void VideoPlayer::setPosition(int position) {
    if (mediaPlayer->duration() <= 0) return;

    qint64 duration = mediaPlayer->duration();
    qint64 newPosition = (position * duration) / 100;

    slider->blockSignals(true);
    mediaPlayer->setPosition(newPosition);
    slider->blockSignals(false);
}

void VideoPlayer::updateSlider() {
    if (mediaPlayer->duration() <= 0) return;

    qint64 currentPosition = mediaPlayer->position();
    qint64 duration = mediaPlayer->duration();
    int sliderPosition = (currentPosition * 100) / duration;

    slider->blockSignals(true);
    slider->setValue(sliderPosition);
    slider->blockSignals(false);
}

void VideoPlayer::fastForward() {
    qint64 currentPosition = mediaPlayer->position();
    mediaPlayer->setPosition(currentPosition + 10000);  // 10초 앞으로 감기
}

void VideoPlayer::rewind() {
    qint64 currentPosition = mediaPlayer->position();
    mediaPlayer->setPosition(currentPosition - 10000);  // 10초 뒤로 감기
}

void VideoPlayer::addVideoToList(const QString &filePath) {
    QString fileName = QFileInfo(filePath).fileName();  // 파일명만 추출
    if (!fileMap->contains(fileName)) {  // 중복 방지
        fileMap->insert(fileName, filePath);
        fileListWidget->addItem(fileName);
    }
}
