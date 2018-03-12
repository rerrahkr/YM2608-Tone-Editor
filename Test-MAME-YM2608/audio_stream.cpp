#include "audio_stream.hpp"
#include <QAudioFormat>
#include <QSysInfo>

AudioStream::AudioStream(chip::OPNA& chip, size_t duration)
{
    QAudioFormat format;
    format.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
    format.setChannelCount(2); // Stereo
    format.setCodec("audio/pcm");
    format.setSampleRate(chip.getRate());
    format.setSampleSize(16);   // int16
    format.setSampleType(QAudioFormat::SignedInt);

    audio_ = std::make_unique<QAudioOutput>(format);
    mixer_ = std::make_unique<AudioStreamMixier>(chip, duration);
    mixer_->start();
    audio_->start(mixer_.get());
}

AudioStream::~AudioStream()
{
    if (mixer_->hasRun()) mixer_->stop();
    if (audio_->state() != QAudio::StoppedState) audio_->stop();
}
