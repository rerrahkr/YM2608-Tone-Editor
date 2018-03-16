#pragma once

#include <QObject>
#include <QIODevice>
#include "types.h"
#include "chips/opna.hpp"

class AudioStreamMixier : public QIODevice
{
public:
    AudioStreamMixier(chip::OPNA& chip, uint32 rate, uint32 duration, QObject* parent = nullptr);
    ~AudioStreamMixier();

    void start();
    void stop();
    bool hasRun();

    void setRate(uint32 rate);
    void setDuration(uint32 duration);

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    chip::OPNA& chip_;
    size_t rate_;
    size_t duration_;
    qint64 bufferSampleSize_;
    size_t tickIntrCountNumer_;
    size_t tickIntrCount_;
    bool isFirstRead_;

    void setBufferSampleSize(uint32 rate, uint32 duration);
};
