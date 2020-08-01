#include "resampler.hpp"
#include "chip_misc.h"

namespace chip
{
	Resampler::Resampler()
	{
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			destBuf_[pan] = new sample[SMPL_BUF_SIZE_]();
		}
	}

	Resampler::~Resampler()
	{
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			delete[] destBuf_[pan];
		}
	}

	void Resampler::init(int srcRate, int destRate)
	{
		srcRate_ = srcRate;
		setDestRate(destRate);
	}

	void Resampler::setDestRate(int destRate)
	{
		destRate_ = destRate;
		rateRatio_ = static_cast<float>(srcRate_) / destRate;
	}

	sample** Resampler::interpolate(sample** src, size_t nSamples)
	{
		// Linear interplation
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			for (size_t n = 0; n < nSamples; ++n) {
				float curnf = n * rateRatio_;
				int curni = static_cast<int>(curnf);
				float sub = curnf - curni;
				if (sub) {
					destBuf_[pan][n] = static_cast<sample>(src[pan][curni] + (src[pan][curni + 1] - src[pan][curni]) * sub);
				}
				else /* if (sub == 0) */ {
					destBuf_[pan][n] = src[pan][curni];
				}
			}
		}

		return destBuf_;
	}
}
