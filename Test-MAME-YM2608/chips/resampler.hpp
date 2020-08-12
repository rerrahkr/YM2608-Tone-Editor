#pragma once

#include <cstddef>
#include <cmath>
#include "chip_def.h"

namespace chip
{
	class Resampler
	{
	public:
		Resampler();
		~Resampler();

		void init(int srcRate, int destRate);

		void setDestRate(int destRate);

		sample** interpolate(sample** src, size_t nSamples);

		inline size_t calculateInternalSampleSize(size_t nSamples)
		{
			return static_cast<size_t>(std::ceil(nSamples * rateRatio_));
		}

	private:
		int srcRate_, destRate_;
		float rateRatio_;
		
		sample* destBuf_[2];
	};
}
