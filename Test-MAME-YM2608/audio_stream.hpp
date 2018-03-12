#pragma once

#include <QAudioOutput>
#include <QAudioFormat>
#include <memory>
#include "types.h"
#include "chips/opna.hpp"
#include "audio_stream_mixier.hpp"

class AudioStream
{
public:
    // duration: miliseconds
    AudioStream(chip::OPNA& chip, uint32 duration);
	~AudioStream();

    void setRate(uint32 rate);
    void setDuration(uint32 duration);

private:
    chip::OPNA& chip_;
    QAudioFormat format_;
    std::unique_ptr<QAudioOutput> audio_;
    std::unique_ptr<AudioStreamMixier> mixer_;

    void start();
    void stop();
};
