#pragma once

#include <QObject>
#include <QIODevice>
#include "chips/opna.hpp"

class AudioStreamMixier : public QIODevice
{
public:
    AudioStreamMixier(chip::OPNA& chip, size_t duration, QObject* parent = nullptr);
    ~AudioStreamMixier();

    void start();
    void stop();
    bool hasRun();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    chip::OPNA& chip_;
    size_t duration_;
    size_t bufferSize_;
    size_t readIntrCount_;
    bool isFirstRead_;
};
