#pragma once

#include "SDL.h"
#include "chips/opna.hpp"

class AudioStream
{
public:
	// bufferTime: miliseconds
	AudioStream(chip::OPNA& chip, size_t bufferTime);
	~AudioStream();

	void start();
	void stop();
	void reset();
	bool isOpendAudioDevice() const;

private:
	chip::OPNA& chip_;
	size_t bufferTime_;
	size_t readIntrCount_;

	struct CallBackArg
	{
		CallBackArg(chip::OPNA& chip, size_t& readIntrCount)
			: chip__(chip), readIntrCount__(readIntrCount) {}
		chip::OPNA& chip__;
		size_t& readIntrCount__;
	} cba_;

	int isNotInitSubSystem_;
	SDL_AudioDeviceID dev_;

	static void soundCallback(void* userdata, uint8* stream, int len);
};