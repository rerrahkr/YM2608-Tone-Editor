#pragma once

#include "chip_def.hpp"
#include <mutex>
#include "../types.h"
#include "resampler.hpp"

namespace chip
{
	class Chip
	{
	public:
		// [rate]
		// 0 = auto-set mode (set internal chip rate)
		Chip(uint32 id, uint32 clock, uint32 rate, uint32 autoRate);
		virtual ~Chip();

		virtual void reset() = 0;
		virtual void setRegister(uint32 offset, uint32 value) = 0;
		virtual uint32 getRegister(uint32 offset) const = 0;

		virtual void setRate(uint32 rate);
		uint32 getRate() const;

		#ifdef SINC_INTERPOLATION
		void setMaxDuration(size_t maxDuration);
		#endif
		
		/*virtual void setVolume(float db) = 0;*/
		virtual void mix(int16* stream, size_t nSamples) = 0;

	protected:
		const int id_;
		std::mutex mutex_;

		int rate_;
		const int autoRate_;
		int internalRate_[2];

		/*float dB_[2];*/
		float volumeRatio_[2];
		/*static const int MAX_AMP_;*/

		sample* buffer_[2][2];
		Resampler resampler_[2];

		#ifdef SINC_INTERPOLATION
		void initResampler(size_t maxDuration);
		#else
		void initResampler();
		#endif

		void funcSetRate(uint32 rate);
	};
}
