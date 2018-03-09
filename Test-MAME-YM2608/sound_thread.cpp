#include "sound_thread.hpp"
#include "SDL.h"

namespace thread
{
	SoundThread::SoundThread(chip::OPNA& chip, size_t bufferTime)
		: chip_(chip),
		bufferTime_(bufferTime),
		readIntrCount_(0),
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

		SDL_AudioSpec desired, obtained;
		desired.freq = chip_.getRate();
		desired.format = AUDIO_S16SYS;	// int16
		desired.channels = 2;			// Stereo
		desired.samples = static_cast<uint16>(nSamples);
		desired.callback = &soundCallback;
		CallBackArg cba = { chip_, readIntrCount_ };
		desired.userdata = &cba;

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
			}
			SDL_CloseAudioDevice(dev);
		}
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	void SoundThread::soundCallback(void* userdata, uint8* stream, int len)
	{
		auto cba = reinterpret_cast<CallBackArg*>(userdata);
		chip::OPNA& chip = cba->chip;
		int rate = chip.getRate();
		size_t& readIntrCount = cba->readIntrCount;
		size_t requiredCount = static_cast<size_t>(len >> 2);	// len / sizeof(short) / stereo
		auto destPtr = reinterpret_cast<int16*>(stream);
		
		size_t count;
		while (requiredCount) {
			if (!readIntrCount) {	// Read data
				// UNDONE: ここにパターン読み込み処理
				// tempoはパターンで読み込む
				// 下の設定だとrowsize=6で16分音符
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
}