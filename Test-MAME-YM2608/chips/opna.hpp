#pragma once

#include <cmath>
#include <mutex>
#include <vector>
#include "../common.hpp"
#include "../types.h"

namespace chip
{
	class OPNA
	{
	public:
		// [rate]
		// 0 = rate is 110933 (internal FM sample rate in 3993600 * 2 clock)
		OPNA(uint32 clock, uint32 rate, size_t maxTime);
		~OPNA() noexcept;
		void reset();
		void setRegister(uint32 offset, uint32 value);
		uint32 getRegister(uint32 offset) const;
		void setRate(uint32 rate, size_t maxTime);
		uint32 getRate() const;
		void setVolume(float dBFM, float dBPSG);	// NOT work
		void mix(int16* stream, size_t nSamples);

	private:
		const int id_;
		static size_t count_;
		static const size_t SMPL_BUFSIZE_;
		/*float dBFM_, dBPSG_;*/
		float volumeRatioFM_, volumeRatioPSG_;
		//static const int MAX_AMP_;
		//static const int DEF_AMP_FM_, DEF_AMP_PSG_;
		sample *bufFM_[2], *bufPSG_[2], *tmpBuf_[2];		// [0]: left, [1]: right
		int internalRateFM_, internalRatePSG_;
		int rate_;
		float rateRatioFM_, rateRatioPSG_;
		#ifdef SINC_INTERPOLATION
		std::vector<float> sincTableFM_, sincTablePSG_;
		static const float F_PI_;
		static const int SINC_OFFSET_;
		#endif
		std::mutex mutex_;

		void setRate(uint32 rate);
		#ifdef SINC_INTERPOLATION
		void initSincTables(size_t maxTime);
		void funcInitSincTables(std::vector<float>& vector, size_t maxSamples, size_t intrSize, float rateRatio);
		void sincInterpolate(sample** dest, size_t nSamples, size_t intrSize, std::vector<float>& map, float rateRatio);
		#else
		void linearInterpolate(sample** dest, size_t nSamples, size_t intrSize, float rateRatio);
		#endif

		inline size_t calculateInternalSampleSize(size_t nSamples, float rateRatio)
		{
			float f = nSamples * rateRatio;
			size_t i = static_cast<size_t>(f);
			return ((f - i) ? (i + 1) : i);
		}

		#ifdef SINC_INTERPOLATION
		static inline float sinc(float x)
		{
			return ((!x) ? 1.0f : (std::sin(x) / x));
		}
		#endif
	};
}
