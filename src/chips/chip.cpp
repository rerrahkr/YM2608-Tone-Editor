#include "chip.hpp"
#include "chip_misc.h"

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
	//const int Chip::MAX_AMP_ = 32767;	// half-max of int16

	Chip::Chip(int id, int clock, int rate, int autoRate) :
		id_(id),
		rate_(1),	// Dummy set
		autoRate_(autoRate)
	{
		(void)clock;
		(void)rate;

		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			for (auto& buf : buffer_) {
				buf[pan] = new stream_sample_t[SMPL_BUF_SIZE_];
			}
		}
	}

	Chip::~Chip()
	{
		for (int pan = LEFT; pan <= RIGHT; ++pan) {
			for (auto& buf : buffer_) {
				delete[] buf[pan];
			}
		}
	}

	void Chip::initResampler()
	{
		for (int snd = 0; snd < 2; ++snd) {
			resampler_[snd].init(internalRate_[snd], rate_);
		}
	}

	void Chip::setRate(int rate)
	{
		std::lock_guard<std::mutex> lg(mutex_);

		funcSetRate(rate);

		for (auto& rsmp : resampler_) {
			rsmp.setDestRate(rate);
		}
	}

	void Chip::funcSetRate(int rate)
	{
		rate_ = CHIP_SAMPLE_RATE = ((rate) ? rate : autoRate_);
	}

	int Chip::getRate() const
	{
		return rate_;
	}
}
