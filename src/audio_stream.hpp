#pragma once

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QAudioSink>
#else
#include <QAudioOutput>
#endif
#include <QAudioFormat>
#include <memory>
#include "chips/chip_def.h"
#include "chips/chip.hpp"
#include "audio_stream_mixier.hpp"

class AudioStream
{
public:
    // duration: miliseconds
	AudioStream(chip::Chip& chip, uint32_t rate, uint32_t duration);
	~AudioStream();

	void setRate(uint32_t rate);
	void setDuration(uint32_t duration);

private:
    chip::Chip& chip_;
    QAudioFormat format_;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    std::unique_ptr<QAudioSink> audio_;
#else
    std::unique_ptr<QAudioOutput> audio_;
#endif
    std::unique_ptr<AudioStreamMixier> mixer_;

    void start();
    void stop();
};
