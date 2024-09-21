//
// Created by lee on 2024. 8. 24..
//

#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget *parent) : QMainWindow(parent), isPaused(false) {
    // MediaPlayer�� VideoWidget ����
    mediaPlayer = new QMediaPlayer(this);
    videoWidget = new QVideoWidget();

    // ��� ��ư ����
    playButton = new QPushButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));  // ������ ����
    connect(playButton, &QPushButton::clicked, this, &VideoPlayer::playVideo);

    // �Ͻ� ���� ��ư ����
    pauseButton = new QPushButton(this);
    pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));  // ������ ����
    connect(pauseButton, &QPushButton::clicked, this, &VideoPlayer::pauseVideo);

    // ������ ���� ��ư ����
    forwardButton = new QPushButton(this);
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));  // ������ ����
    connect(forwardButton, &QPushButton::clicked, this, &VideoPlayer::fastForward);

    // �ڷ� ���� ��ư ����
    rewindButton = new QPushButton(this);
    rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));  // ������ ����
    connect(rewindButton, &QPushButton::clicked, this, &VideoPlayer::rewind);

    // �߰� ��ư ���� (���� �Ǵ� ����)
    addButton = new QPushButton("Add Video", this);
    connect(addButton, &QPushButton::clicked, this, &VideoPlayer::handleAddButtonClick);

    // ���� ��ư ����
    removeButton = new QPushButton("Remove Video", this);
    connect(removeButton, &QPushButton::clicked, this, &VideoPlayer::handleRemoveButtonClick);

    // ���� ��ư ����
    stopButton = new QPushButton(this);  // ���� ��ư �߰�
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));  // ������ ����
    connect(stopButton, &QPushButton::clicked, this, &VideoPlayer::stopVideo);

    // �����̴� ����
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    connect(slider, &QSlider::valueChanged, this, &VideoPlayer::setPosition);

    // ���� ����Ʈ ���� ����
    fileListWidget = new QListWidget(this);
    connect(fileListWidget, &QListWidget::itemClicked, this, &VideoPlayer::playSelectedFile);

    // ���� ��θ� ������ QMap ����
    fileMap = new QMap<QString, QString>();

    // ���̾ƿ� ����
    QHBoxLayout *mainLayout = new QHBoxLayout;

    QVBoxLayout *leftLayout = new QVBoxLayout;

    // ��ư�� 1x2 ���� ���·� ��ġ�� ���̾ƿ�
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);

    leftLayout->addLayout(buttonLayout); // ���� ��ư ���̾ƿ� �߰�
    leftLayout->addWidget(fileListWidget); // ���� ����Ʈ ���� �߰�

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(videoWidget);

    // ��ư�� 1x5 ���� ���·� ��ġ�� ���̾ƿ�
    QHBoxLayout *controlButtonLayout = new QHBoxLayout;
    controlButtonLayout->addWidget(rewindButton);  // �ڷ� ���� ��ư �߰�
    controlButtonLayout->addWidget(playButton);    // ��� ��ư �߰�
    controlButtonLayout->addWidget(stopButton);    // ���� ��ư �߰�
    controlButtonLayout->addWidget(pauseButton);   // �Ͻ� ���� ��ư �߰�
    controlButtonLayout->addWidget(forwardButton); // ������ ���� ��ư �߰�

    rightLayout->addLayout(controlButtonLayout);  // 1x5 ��ư ���̾ƿ� �߰�
    rightLayout->addWidget(slider);

    mainLayout->addLayout(leftLayout, 1);  // ���� �κ� ���� 1
    mainLayout->addLayout(rightLayout, 4); // ������ �κ� ���� 4

    QWidget *container = new QWidget;
    container->setLayout(mainLayout);
    setCentralWidget(container);

    // �̵�� �÷��̾� ����
    mediaPlayer->setVideoOutput(videoWidget);

    // ����� ��� ����
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);

    // ����� ����
    audioOutput->setMuted(false);
    audioOutput->setVolume(0.1);

    // �����̴� ������Ʈ�� ���� Ÿ�̸� ����
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoPlayer::updateSlider);
    timer->start(100);
}

void VideoPlayer::playVideo() {
    // ������ ����� ��� �ִ��� Ȯ��
    if (fileListWidget->count() == 0) {
        return;  // ����� ��� ������ �ƹ� �͵� �������� ����
    }

    if (isPaused) {
        mediaPlayer->play();
        isPaused = false;  // �÷��� ����
    } else {
        // ���� ���õ� ������ ������ ����Ʈ�� ù ��° ���� ���
        if (fileListWidget->currentItem() == nullptr && fileListWidget->count() > 0) {
            fileListWidget->setCurrentRow(0); // ù ��° ���� ����
        }

        // ���� ���õ� ���� ���
        QListWidgetItem *currentItem = fileListWidget->currentItem();
        if (currentItem) {
            playSelectedFile(currentItem);
        }
    }
}

void VideoPlayer::stopVideo() {
    mediaPlayer->stop();
    slider->setValue(0);  // �����̴��� 0���� �ʱ�ȭ
    isPaused = false;  // �Ͻ� ���� ���� ����
}

void VideoPlayer::pauseVideo() {
    mediaPlayer->pause();
    isPaused = true;  // �÷��� ����
}

void VideoPlayer::handleAddButtonClick() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);  // ���� �� ���� ���� �����ϰ� ����
    dialog.setOption(QFileDialog::ShowDirsOnly, false); // ���� ����
    dialog.setFileMode(QFileDialog::ExistingFiles);  // ���� ���� ���
    dialog.setViewMode(QFileDialog::List);

    if (dialog.exec()) {
        QStringList selectedPaths = dialog.selectedFiles();
        for (const QString &filePath : selectedPaths) {
            QFileInfo fileInfo(filePath);
            if (fileInfo.isDir()) {
                // ������ ��� ������ ��� ���� ���� �߰�
                QDir dir(filePath);
                QStringList filters;
                filters << "*.mp4" << "*.avi" << "*.mov";
                QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
                for (const QFileInfo &fileInfo : fileList) {
                    addVideoToList(fileInfo.filePath());
                }
            } else {
                // ������ ��� �ٷ� �߰�
                addVideoToList(filePath);
            }
        }
    }
}

void VideoPlayer::handleRemoveButtonClick() {
    QListWidgetItem *selectedItem = fileListWidget->currentItem();
    if (selectedItem) {
        QString fileName = selectedItem->text();
        fileMap->remove(fileName);  // �ʿ��� ���� ��� ����
        delete fileListWidget->takeItem(fileListWidget->row(selectedItem));  // ���õ� �׸� ����

        // ������ ����� ��� ������ ��� ���¸� ����
        if (fileListWidget->count() == 0) {
            mediaPlayer->stop();
            isPaused = false;  // �Ͻ� ���� ���� ����
        }
    }
}

void VideoPlayer::playSelectedFile(QListWidgetItem *item) {
    QString fileName = item->text();
    QString filePath = fileMap->value(fileName);  // ���ϸ����� ��θ� ��ȸ
    if (!filePath.isEmpty()) {
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        mediaPlayer->play(); // �Ͻ� ������ �������� ��� ����
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
    mediaPlayer->setPosition(currentPosition + 10000);  // 10�� ������ ����
}

void VideoPlayer::rewind() {
    qint64 currentPosition = mediaPlayer->position();
    mediaPlayer->setPosition(currentPosition - 10000);  // 10�� �ڷ� ����
}

void VideoPlayer::addVideoToList(const QString &filePath) {
    QString fileName = QFileInfo(filePath).fileName();  // ���ϸ� ����
    if (!fileMap->contains(fileName)) {  // �ߺ� ����
        fileMap->insert(fileName, filePath);
        fileListWidget->addItem(fileName);
    }
}
