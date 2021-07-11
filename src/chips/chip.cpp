#include "chip.hpp"
#include "chip_misc.h"

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
				buf[pan] = new sample[SMPL_BUF_SIZE_];
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
		rate_ = (rate) ? rate : autoRate_;
	}

	int Chip::getRate() const
	{
		return rate_;
	}
}
