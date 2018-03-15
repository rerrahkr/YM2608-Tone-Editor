#pragma once

#include "../common.hpp"
#include <mutex>
#include <vector>
#ifdef SINC_INTERPOLATION
#include <cmath>
#endif
#include "../types.h"

namespace chip
{
	class Chip
	{
	public:
		// [rate]
		// 0 = auto-set mode (set internal chip rate)
		#ifdef SINC_INTERPOLATION
		Chip(uint32 clock, uint32 rate, size_t maxTime);
		#else
		Chip(uint32 clock, uint32 rate);
		#endif
		virtual ~Chip();

		virtual void reset() = 0;
		virtual void setRegister(uint32 offset, uint32 value) = 0;
		virtual uint32 getRegister(uint32 offset) const = 0;

		#ifdef SINC_INTERPOLATION
		virtual void setRate(uint32 rate, size_t maxTime) = 0;
		#else		
		virtual void setRate(uint32 rate) = 0;
		#endif
		uint32 getRate() const;
		
		//virtual void setVolume(float db) = 0;
		virtual void mix(int16* stream, size_t nSamples) = 0;

	protected:
		const int id_;
		std::mutex mutex_;
		int rate_;
		static const size_t SMPL_BUFSIZE_;
		sample *tmpBuf_[2];		// [0]: left, [1]: right

		#ifdef SINC_INTERPOLATION
		virtual void initSincTables(size_t maxTime) = 0;
		void funcInitSincTables(std::vector<float>& table, size_t maxSamples, size_t intrSize, float rateRatio);
		void sincInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, std::vector<float>& table, float rateRatio);
		#else
		void linearInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, float rateRatio);
		#endif

		inline size_t calculateInternalSampleSize(size_t nSamples, float rateRatio)
		{
			float f = nSamples * rateRatio;
			size_t i = static_cast<size_t>(f);
			return ((f - i) ? (i + 1) : i);
		}

	private:

		#ifdef SINC_INTERPOLATION
		static const float F_PI_;
		static const int SINC_OFFSET_;

		static inline float sinc(float x)
		{
			return ((!x) ? 1.0f : (std::sin(x) / x));
		}
		#endif
	};
}
