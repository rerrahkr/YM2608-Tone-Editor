#pragma once

#include "../common.hpp"
#ifdef SINC_INTERPOLATION
#include <vector>
#include <cmath>
#endif
#include <mutex>
#include "../types.h"

namespace chip
{
	class Chip
	{
	public:
		// [rate]
		// 0 = auto-set mode (set internal chip rate)
		#ifdef SINC_INTERPOLATION
		Chip(uint32 id, uint32 clock, uint32 rate, uint32 autoRate, size_t maxTime);
		#else
		Chip(uint32 id, uint32 clock, uint32 rate, uint32 autoRate);
		#endif
		virtual ~Chip();

		virtual void reset() = 0;
		virtual void setRegister(uint32 offset, uint32 value) = 0;
		virtual uint32 getRegister(uint32 offset) const = 0;

		#ifdef SINC_INTERPOLATION
		virtual void setRate(uint32 rate, size_t maxTime);
		#else		
		virtual void setRate(uint32 rate);
		#endif
		uint32 getRate() const;
		
		//virtual void setVolume(float db) = 0;
		virtual void mix(int16* stream, size_t nSamples) = 0;

	protected:
		const int id_;
		std::mutex mutex_;

		int rate_;
		const int autoRate_;
		int internalRate_[2];
		float rateRatio_[2];

		sample* buffer_[2][2];
		sample* tmpBuf_[2];

		/*float dB_[2];*/
		float volumeRatio_[2];
		/*static const int MAX_AMP_;*/

		#ifdef SINC_INTERPOLATION
		std::vector<float> sincTable_[2];
		#endif

		void funcSetRate(uint32 rate);
		void setRateRatio();

		#ifdef SINC_INTERPOLATION
		void initSincTables(size_t maxTime);
		void sincInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, std::vector<float>& table, float rateRatio);
		#else
		void linearInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, float rateRatio);
		#endif

		enum Stereo : int
		{
			LEFT = 0,
			RIGHT = 1
		};

		inline size_t calculateInternalSampleSize(size_t nSamples, float rateRatio)
		{
			float f = nSamples * rateRatio;
			size_t i = static_cast<size_t>(f);
			return ((f - i) ? (i + 1) : i);
		}

	private:
		static const size_t SMPL_BUFSIZE_;

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
