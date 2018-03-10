#include "audio_stream.hpp"

AudioStream::AudioStream(chip::OPNA& chip, size_t bufferTime)
	: chip_(chip),
	bufferTime_(bufferTime),
	readIntrCount_(0),
	cba_(chip_, readIntrCount_),
	isNotInitSubSystem_(SDL_InitSubSystem(SDL_INIT_AUDIO))
{
	if (isNotInitSubSystem_) {
		dev_ = 0;
		return;
	}

	size_t nSamples = chip_.getRate() * bufferTime_ / 1000;

	SDL_AudioSpec desired, obtained;
	desired.freq = chip_.getRate();
	desired.format = AUDIO_S16SYS;	// int16
	desired.channels = 2;			// Stereo
	desired.samples = static_cast<uint16>(nSamples);
	desired.callback = &soundCallback;
	desired.userdata = &cba_;

	dev_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
}

AudioStream::~AudioStream()
{
	if (!isNotInitSubSystem_) {
		if (isOpendAudioDevice()) SDL_CloseAudioDevice(dev_);
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}

void AudioStream::start()
{
	if (isOpendAudioDevice()) SDL_PauseAudioDevice(dev_, 0);
}

void AudioStream::stop()
{
	if (isOpendAudioDevice()) SDL_PauseAudioDevice(dev_, 1);
}

void AudioStream::reset()
{
	if (isOpendAudioDevice()) SDL_ClearQueuedAudio(dev_);
}

bool AudioStream::isOpendAudioDevice() const
{
	return (dev_ != 0);
}

void AudioStream::soundCallback(void* userdata, uint8* stream, int len)
{
	auto cba = reinterpret_cast<CallBackArg*>(userdata);
	chip::OPNA& chip = cba->chip__;
	int rate = chip.getRate();
	size_t& readIntrCount = cba->readIntrCount__;
	size_t requiredCount = static_cast<size_t>(len >> 2);	// len / sizeof(short) / stereo
	auto destPtr = reinterpret_cast<int16*>(stream);

	size_t count;
	while (requiredCount) {
		if (!readIntrCount) {	// Read data
			// Read pattern data in here
			// 1 row = 6 counts = 1/16 notes
			int bpm = 120;
			readIntrCount = ((rate * 5) >> 2) / bpm;
		}

		count = (requiredCount > readIntrCount) ? readIntrCount : requiredCount;
		requiredCount -= count;
		readIntrCount -= count;

		chip.mix(destPtr, count);

		destPtr += (count << 1);	// Move head
	}
}