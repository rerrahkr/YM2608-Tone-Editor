#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "chips/opna.hpp"

namespace thread
{
	class SoundThread
	{
	public:
		// bufferTime: miliseconds
		explicit SoundThread(chip::OPNA& opna, size_t bufferTime);
		~SoundThread() noexcept;

		void stopSound();
		void restartSound();

	private:
		chip::OPNA& chip_;
		size_t bufferTime_;

		std::atomic_bool shouldReset_;
		std::atomic_bool shouldContinue_;

		std::thread thread_;
		std::condition_variable cv;
		std::mutex mutex_;

		SoundThread(const SoundThread&) = delete;
		SoundThread& operator= (const SoundThread&) = delete;
		SoundThread(SoundThread&&) = delete;
		SoundThread& operator= (SoundThread&&) noexcept = delete;

		void threadFunction();
		static void soundCallback(void* userdata, uint8* stream, int len);
	};
}