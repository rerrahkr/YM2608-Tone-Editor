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
	#ifdef SINC_INTERPOLATION
	const float Chip::F_PI_ = 3.14159265f;
	const int Chip::SINC_OFFSET_ = 16;
	#endif

	#ifdef SINC_INTERPOLATION
	Chip::Chip(uint32 clock, uint32 rate, size_t maxTime)
	#else
	Chip::Chip(uint32 clock, uint32 rate)
	#endif
	{
		for (int i = 0; i < 2; ++i) {
			tmpBuf_[i] = new stream_sample_t[SMPL_BUFSIZE_];
		}
	}

	Chip::~Chip()
	{
		for (int i = 0; i < 2; ++i) {
			delete[] tmpBuf_[i];
		}
	}

	uint32 Chip::getRate() const
	{
		return rate_;
	}

	#ifdef SINC_INTERPOLATION
	void Chip::funcInitSincTables(std::vector<float>& table, size_t maxSamples, size_t intrSize, float rateRatio)
	{
		size_t offsetx2 = SINC_OFFSET_ << 1;
		table.resize(maxSamples * offsetx2);

		for (size_t j = 0; j < maxSamples; ++j) {
			size_t seg = j * offsetx2;
			float rcurn = j * rateRatio;
			int curn = static_cast<int>(rcurn);
			int k = curn - SINC_OFFSET_;
			if (k < 0) k = 0;
			int end = curn + SINC_OFFSET_;
			if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
			for (; k < end; ++k) {
				table[seg + SINC_OFFSET_ + (k - curn)] = sinc(F_PI_ * (rcurn - k));
			}
		}
	}

	void Chip::sincInterpolate(sample** src, sample** dest, size_t nSamples, size_t intrSize, std::vector<float>& table, float rateRatio)
	{
		size_t offsetx2 = SINC_OFFSET_ << 1;
		for (size_t i = 0; i < 2; ++i) {
			for (size_t j = 0; j < nSamples; ++j) {
				size_t seg = j * offsetx2;
				int curn = static_cast<int>(j * rateRatio);
				int k = curn - SINC_OFFSET_;
				if (k < 0) k = 0;
				int end = curn + SINC_OFFSET_;
				if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
				sample samp = 0;
				for (; k < end; ++k) {
					samp += static_cast<sample>(src[i][k] * table[seg + SINC_OFFSET_ + (k - curn)]);
				}
				dest[i][j] = samp;
			}
		}
	}
	#else
    void Chip::linearInterpolate(sample** src, sample** dest, size_t nSamples, float rateRatio)
	{
		for (size_t i = 0; i < 2; ++i) {
			for (size_t j = 0; j < nSamples; ++j) {
				float curnf = j * rateRatio;
				int curni = static_cast<int>(curnf);
				float sub = curnf - curni;
				if (sub) {	// Linear interpolation
					dest[i][j] = static_cast<sample>(src[i][curni] + (src[i][curni + 1] - src[i][curni]) * sub);
				}
				else /* if (sub == 0) */ {
					dest[i][j] = src[i][curni];
				}
			}
		}
	}
	#endif
}
