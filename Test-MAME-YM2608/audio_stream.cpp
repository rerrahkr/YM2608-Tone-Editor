#include "audio_stream.hpp"
#include <QSysInfo>

AudioStream::AudioStream(chip::Chip& chip, uint32 rate, uint32 duration) :
    chip_(chip)
{
    format_.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
    format_.setChannelCount(2); // Stereo
    format_.setCodec("audio/pcm");
    format_.setSampleRate(rate);
    format_.setSampleSize(16);   // int16
    format_.setSampleType(QAudioFormat::SignedInt);

    audio_ = std::make_unique<QAudioOutput>(format_);
    mixer_ = std::make_unique<AudioStreamMixier>(chip_, rate, duration);
    start();
}

AudioStream::~AudioStream()
{
    stop();
}

void AudioStream::setRate(uint32 rate)
{
    stop();
    format_.setSampleRate(rate);
    audio_ = std::make_unique<QAudioOutput>(format_);
    mixer_->setRate(rate);
    start();
}

void AudioStream::setDuration(uint32 duration)
{
    stop();
    mixer_->setDuration(duration);
    start();
}

void AudioStream::start()
{
    if (!mixer_->hasRun()) mixer_->start();
    if (audio_->state() != QAudio::ActiveState) audio_->start(mixer_.get());
}

void AudioStream::stop()
{
    if (mixer_->hasRun()) mixer_->stop();
    if (audio_->state() != QAudio::StoppedState) audio_->stop();
}
