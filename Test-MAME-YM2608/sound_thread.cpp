#include "sound_thread.hpp"
#include <memory>
#include <condition_variable>
#include <mutex>
#include "SDL.h"

namespace thread
{
	SoundThread::SoundThread(chip::OPNA& opna, size_t bufferTime)
		: chip_(opna),
		bufferTime_(bufferTime),
		shouldReset_(false),
		shouldContinue_(true),
		thread_(&SoundThread::threadFunction, this)
	{
	}

	SoundThread::~SoundThread() noexcept
	{
		shouldContinue_.store(false);

		if (thread_.joinable()) thread_.join();
	}

	void SoundThread::stopSound()
	{
		shouldReset_.store(true);
	}

	void SoundThread::restartSound()
	{
		shouldReset_.store(false);

		cv.notify_one();
	}

	void SoundThread::threadFunction()
	{
		if (SDL_InitSubSystem(SDL_INIT_AUDIO)) return;

		size_t nSamples = chip_.getRate() * bufferTime_ / 1000;
		size_t bufferSize = nSamples << 2;	// nSamples * 2(stereo) * 2(sizeof(int16))
		size_t supplyLine = bufferSize + (bufferSize >> 1);

		std::unique_ptr<int16[]> buffer = std::make_unique<int16[]>(nSamples << 1);

		SDL_AudioSpec desired, obtained;
		desired.freq = chip_.getRate();
		desired.format = AUDIO_S16SYS;	// int16
		desired.channels = 2;			// Stereo
		desired.samples = static_cast<uint16>(nSamples);
		desired.callback = nullptr;
		desired.userdata = nullptr;

		SDL_AudioDeviceID dev = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
		if (dev != 0) {
			SDL_PauseAudioDevice(dev, 0);
			while (shouldContinue_.load())
			{
				if (shouldReset_.load()) {	// Reset sound
					SDL_PauseAudioDevice(dev, 1);
					SDL_ClearQueuedAudio(dev);
					{
						std::unique_lock<std::mutex> ul(mutex_);
						cv.wait(ul, [&] { return !shouldReset_.load(); });
					}
					SDL_PauseAudioDevice(dev, 0);
				}
				else if (SDL_GetQueuedAudioSize(dev) < supplyLine) {	// Load sound
					chip_.mix(buffer.get(), nSamples);
					SDL_QueueAudio(dev, buffer.get(), bufferSize);
				}
			}
			SDL_CloseAudioDevice(dev);
		}
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}