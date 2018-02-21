#pragma once

#include <mutex>
#include "../types.h"

namespace chip
{
	class OPNA
	{
	public:
		// [rate]
		// 0 = rate is 110933 (internal FM sample rate in 3993600 * 2 clock)
		OPNA(uint32 clock, uint32 rate);
		~OPNA() noexcept;
		void reset();
		void setRegister(uint32 offset, uint32 value);
		uint32 getRegister(uint32 offset) const;
		void setRate(uint32 rate);
		uint32 getRate() const;
		void setVolume(float dBFM, float dBPSG);	// NOT work
		void mix(int16* stream, size_t nSamples);	// Do mutex

	private:
		const int id_;
		/*float dBFM_, dBPSG_;*/
		float volumeRatioFM_, volumeRatioPSG_;
		//static const int MAX_AMP_;
		//static const int DEF_AMP_FM_, DEF_AMP_PSG_;
		sample *bufFM_[2], *bufPSG_[2], *tmpBuf_[2];		// [0]: left, [1]: right
		int internalRateFM_;
		int internalRatePSG_;
		int rate_;
		float rateRatioFM_, rateRatioPSG_;
		std::mutex mutex_;

		void init(uint32 clock, uint32 rate);
		void deinit();
		void setRate(uint32 rate, bool isInit);
		void sincResample(sample** dest, size_t intrSize, float rateRatio);

		inline size_t calculateInternalSampleSize(size_t nSamples, int intrRate)
		{
			return static_cast<size_t>(nSamples * intrRate / rate_);
		}
	};
}
