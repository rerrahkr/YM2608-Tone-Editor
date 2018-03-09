#pragma once

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "chips/opna.hpp"

namespace thread
{
	class SoundThread
	{
	public:
		// bufferTime: miliseconds
		explicit SoundThread(chip::OPNA& chip, size_t bufferTime);
		~SoundThread();

		void stopSound();
		void restartSound();

	private:
		chip::OPNA& chip_;
		size_t bufferTime_;
		size_t readIntrCount_;

		std::atomic_bool shouldReset_;
		std::atomic_bool shouldContinue_;

		std::condition_variable cv;
		std::mutex mutex_;
		std::thread thread_;

		SoundThread(const SoundThread&) = delete;
		SoundThread& operator= (const SoundThread&) = delete;
		SoundThread(SoundThread&&) = delete;
		SoundThread& operator= (SoundThread&&) noexcept = delete;

		void threadFunction();
		static void soundCallback(void* userdata, uint8* stream, int len);

		struct CallBackArg
		{
			chip::OPNA& chip;
			size_t& readIntrCount;
		};
	};
}