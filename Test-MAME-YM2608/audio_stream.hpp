#pragma once

#include <QAudioOutput>
#include <memory>
#include "types.h"
#include "chips/opna.hpp"
#include "audio_stream_mixier.hpp"

class AudioStream
{
public:
    // duration: miliseconds
    AudioStream(chip::OPNA& chip, size_t duration);
	~AudioStream();

private:
    std::unique_ptr<QAudioOutput> audio_;
    std::unique_ptr<AudioStreamMixier> mixer_;
};
