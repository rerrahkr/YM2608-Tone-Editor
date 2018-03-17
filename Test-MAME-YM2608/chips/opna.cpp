#include "opna.hpp"

#ifdef  __cplusplus
extern "C"
{
#endif //  __cplusplus

	#include "mame/2608intf.h"

	extern INT32 CHIP_SAMPLE_RATE;

#ifdef __cplusplus
}
#endif // __cplusplus

namespace chip
{
	size_t OPNA::count_ = 0;
	
	/*const int OPNA::DEF_AMP_FM_ = 11722;*/
	/*const int OPNA::DEF_AMP_PSG_ = 7250;*/

	#ifdef SINC_INTERPOLATION
	OPNA::OPNA(uint32 clock, uint32 rate, size_t maxTime) :
		Chip(clock, rate, maxTime),
	#else
	OPNA::OPNA(uint32 clock, uint32 rate) :
		Chip(clock, rate),
	#endif
		id_(count_++)
	{
		// New FM & PSG bufer
		for (int pan = Stereo::LEFT; pan <= Stereo::RIGHT; ++pan) {
			buffer_[FM][pan] = new stream_sample_t[SMPL_BUFSIZE_];
			buffer_[PSG][pan] = new stream_sample_t[SMPL_BUFSIZE_];
		}

		funcSetRate(rate);

		UINT8 EmuCore = 0;
		ym2608_set_ay_emu_core(EmuCore);

		UINT8 AYDisable = 0;	// enable
		UINT8 AYFlags = 0;		// none
		internalRate_[FM] = device_start_ym2608(id_, clock, AYDisable, AYFlags, reinterpret_cast<int*>(&internalRate_[PSG]));

		setRateRatio();

		#ifdef SINC_INTERPOLATION
		initSincTables(maxTime);
		#endif

		setVolume(0, 0);

		reset();
	}

	OPNA::~OPNA()
	{
		device_stop_ym2608(id_);
		
		// Delete FM & PSG buffer
		for (int pan = Stereo::LEFT; pan <= Stereo::RIGHT; ++pan) {
			delete[] buffer_[FM][pan];
			delete[] buffer_[PSG][pan];
		}

		--count_;
	}

	void OPNA::reset()
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		device_reset_ym2608(id_);
	}

	void OPNA::setRegister(uint32 offset, uint32 value)
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex in this method

		if (offset & 0x100) {
			ym2608_control_port_b_w(id_, 2, offset & 0xff);
			ym2608_data_port_b_w(id_, 3, value & 0xff);
		}
		else
		{
			ym2608_control_port_a_w(id_, 0, offset & 0xff);
			ym2608_data_port_a_w(id_, 1, value & 0xff);
		}
	}

	uint32 OPNA::getRegister(uint32 offset) const
	{
		if (offset & 0x100) {
			ym2608_control_port_b_w(id_, 2, offset & 0xff);
		}
		else
		{
			ym2608_control_port_a_w(id_, 0, offset & 0xff);
		}
		return ym2608_read_port_r(id_, 1);
	}

	#ifdef SINC_INTERPOLATION
	void OPNA::setRate(uint32 rate, size_t maxTime)
	#else
	void OPNA::setRate(uint32 rate)
	#endif
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		funcSetRate(rate);
		setRateRatio();

		#ifdef SINC_INTERPOLATION
		initSincTables(maxTime);
		#endif
	}

	void OPNA::funcSetRate(uint32 rate)
	{
		rate_ = CHIP_SAMPLE_RATE = ((rate) ? rate : 110933);
	}

	void OPNA::setRateRatio()
	{
		rateRatio_[FM] = static_cast<float>(internalRate_[FM]) / rate_;
		rateRatio_[PSG] = static_cast<float>(internalRate_[PSG]) / rate_;
	}

	#ifdef SINC_INTERPOLATION
	void OPNA::initSincTables(size_t maxTime)
	{
		size_t maxSamples = rate_ * maxTime / 1000;

		if (internalRate_[FM] != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxSamples, rateRatio_[FM]);
			funcInitSincTables(sincTable_[FM], maxSamples, intrSize, rateRatio_[FM]);
		}

		if (internalRate_[PSG] != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxSamples, rateRatio_[PSG]);
			funcInitSincTables(sincTable_[PSG], maxSamples, intrSize, rateRatio_[PSG]);
		}
	}
	#endif

	// TODO: Volume settings
	void OPNA::setVolume(float dBFM, float dBPSG)
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		//dBFM_ = dBFM;
		//dBPSG_ = dBPSG;

		/*fmVolumeRatio_ = maxAmplitude_ / defaultFMAmplitude_ * std::pow(10, fmdB / 20);
		psgVolumeRatio_ = maxAmplitude_ / defaultPSGAmplitude_ * std::pow(10, psgdB / 20);*/
		volumeRatio_[FM] = 1;
		volumeRatio_[PSG] = 1;
	}

	void OPNA::mix(int16* stream, size_t nSamples)
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		// Set FM buffer
		if (internalRate_[FM] == rate_) {
			ym2608_stream_update(id_, buffer_[FM], nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, rateRatio_[FM]);
			ym2608_stream_update(id_, tmpBuf_, intrSize);
			#ifdef SINC_INTERPOLATION
			sincInterpolate(tmpBuf_, buffer_[FM], nSamples, intrSize, sincTable_[FM], rateRatio_[FM]);
			#else
			linearInterpolate(tmpBuf_, buffer_[FM], nSamples, intrSize, rateRatio_[FM]);
			#endif
		}

		// Set PSG buffer
		if (internalRate_[PSG] == rate_) {
			ym2608_stream_update_ay(id_, buffer_[PSG], nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, rateRatio_[PSG]);
			ym2608_stream_update_ay(id_, tmpBuf_, intrSize);
			#ifdef SINC_INTERPOLATION
			sincInterpolate(tmpBuf_, buffer_[PSG], nSamples, intrSize, sincTable_[PSG], rateRatio_[PSG]);
			#else
			linearInterpolate(tmpBuf_, buffer_[PSG], nSamples, intrSize, rateRatio_[PSG]);
			#endif
		}

		for (size_t i = 0; i < nSamples; ++i) {
			*stream++ = static_cast<int16>(volumeRatio_[FM] * buffer_[FM][LEFT][i] + volumeRatio_[PSG] * buffer_[PSG][LEFT][i]);
			*stream++ = static_cast<int16>(volumeRatio_[FM] * buffer_[FM][RIGHT][i] + volumeRatio_[PSG] * buffer_[PSG][RIGHT][i]);
		}
	}
}