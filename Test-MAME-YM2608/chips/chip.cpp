#include "chip.hpp"

#ifdef  __cplusplus
extern "C"
{
#endif //  __cplusplus

	#include "mame/mamedef.h"

	UINT8 CHIP_SAMPLING_MODE = 0x00;
	INT32 CHIP_SAMPLE_RATE;
	stream_sample_t* DUMMYBUF[] = { nullptr, nullptr };

#ifdef __cplusplus
}
#endif // __cplusplus

namespace chip
{
	const size_t Chip::SMPL_BUFSIZE_ = 0x10000;
	//const int Chip::MAX_AMP_ = 32767;	// half-max of int16
#ifdef SINC_INTERPOLATION
	const float Chip::F_PI_ = 3.14159265f;
	const int Chip::SINC_OFFSET_ = 16;
	#endif

	#ifdef SINC_INTERPOLATION
	Chip::Chip(uint32 id, uint32 clock, uint32 rate, uint32 autoRate, size_t maxTime) :
	#else
	Chip::Chip(uint32 id, uint32 clock, uint32 rate, uint32 autoRate) :
	#endif
		id_(id),
		rate_(1),	// Dummy set
		autoRate_(autoRate)
	{
		for (int snd = 0; snd < 2; ++snd) {
			for (int pan = LEFT; pan <= RIGHT; ++pan) {
				buffer_[snd][pan] = new stream_sample_t[SMPL_BUFSIZE_];
			}
			tmpBuf_[snd] = new stream_sample_t[SMPL_BUFSIZE_];
		}
	}

	Chip::~Chip()
	{
		for (int snd = 0; snd < 2; ++snd) {
			for (int pan = LEFT; pan <= RIGHT; ++pan) {
				delete[] buffer_[snd][pan];
			}
			delete[] tmpBuf_[snd];
		}
	}

	#ifdef SINC_INTERPOLATION
	void Chip::setRate(uint32 rate, size_t maxTime)
	#else
	void Chip::setRate(uint32 rate)
	#endif
	{
		std::lock_guard<std::mutex> lg(mutex_);

		funcSetRate(rate);
		setRateRatio();

		#ifdef SINC_INTERPOLATION
		initSincTables(maxTime);
		#endif
	}

	void Chip::funcSetRate(uint32 rate)
	{
		rate_ = CHIP_SAMPLE_RATE = ((rate) ? rate : autoRate_);
	}

	void Chip::setRateRatio()
	{
		for (int snd = 0; snd < 2; ++snd) {
			rateRatio_[snd] = static_cast<float>(internalRate_[snd]) / rate_;
		}
	}

	uint32 Chip::getRate() const
	{
		return rate_;
	}

	#ifdef SINC_INTERPOLATION
	void Chip::initSincTables(size_t maxTime)
	{
		size_t maxSamples = rate_ * maxTime / 1000;

		for (int snd = 0; snd < 2; ++snd) {
			if (internalRate_[snd] != rate_) {
				size_t intrSize = calculateInternalSampleSize(maxSamples, rateRatio_[snd]);
				size_t offsetx2 = SINC_OFFSET_ << 1;
				sincTable_[snd].resize(maxSamples * offsetx2);
				for (size_t n = 0; n < maxSamples; ++n) {
					size_t seg = n * offsetx2;
					float rcurn = n * rateRatio_[snd];
					int curn = static_cast<int>(rcurn);
					int k = curn - SINC_OFFSET_;
					if (k < 0) k = 0;
					int end = curn + SINC_OFFSET_;
					if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
					for (; k < end; ++k) {
						sincTable_[snd][seg + SINC_OFFSET_ + (k - curn)] = sinc(F_PI_ * (rcurn - k));
					}
				}
			}
		}
	}

	void Chip::sincInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, std::vector<float>& table, float rateRatio)
	{
		size_t offsetx2 = SINC_OFFSET_ << 1;
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			for (size_t n = 0; n < nSamples; ++n) {
				size_t seg = n * offsetx2;
				int curn = static_cast<int>(n * rateRatio);
				int k = curn - SINC_OFFSET_;
				if (k < 0) k = 0;
				int end = curn + SINC_OFFSET_;
				if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
				sample samp = 0;
				for (; k < end; ++k) {
					samp += static_cast<sample>(src[pan][k] * table[seg + SINC_OFFSET_ + (k - curn)]);
				}
				dest[pan][n] = samp;
			}
		}
	}
	#else
	void Chip::linearInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, float rateRatio)
	{
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			for (size_t n = 0; n < nSamples; ++n) {
				float curnf = n * rateRatio;
				int curni = static_cast<int>(curnf);
				float sub = curnf - curni;
				if (sub) {	// Linear interpolation
					dest[pan][n] = static_cast<sample>(src[pan][curni] + (src[pan][curni + 1] - src[pan][curni]) * sub);
				}
				else /* if (sub == 0) */ {
					dest[pan][n] = src[pan][curni];
				}
			}
		}
	}
	#endif
}
