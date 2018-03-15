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
	
	//const int OPNA::MAX_AMP_ = 32767;	// half-max of int16
	//const int OPNA::DEF_AMP_FM_ = 11722;
	//const int OPNA::DEF_AMP_PSG_ = 7250;

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
		for (int i = 0; i < 2; ++i) {
			bufFM_[i] = new stream_sample_t[SMPL_BUFSIZE_];
			bufPSG_[i] = new stream_sample_t[SMPL_BUFSIZE_];
		}

		funcSetRate(rate);

		UINT8 EmuCore = 0;
		ym2608_set_ay_emu_core(EmuCore);

		UINT8 AYDisable = 0;	// enable
		UINT8 AYFlags = 0;		// none
		internalRateFM_ = device_start_ym2608(id_, clock, AYDisable, AYFlags, reinterpret_cast<int*>(&internalRatePSG_));

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
		for (int i = 0; i < 2; ++i) {
			delete[] bufFM_[i];
			delete[] bufPSG_[i];
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
		rateRatioFM_ = static_cast<float>(internalRateFM_) / rate_;
		rateRatioPSG_ = static_cast<float>(internalRatePSG_) / rate_;
	}

	#ifdef SINC_INTERPOLATION
	void OPNA::initSincTables(size_t maxTime)
	{
		size_t maxSamples = rate_ * maxTime / 1000;

		if (internalRateFM_ != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxSamples, rateRatioFM_);
			funcInitSincTables(sincTableFM_, maxSamples, intrSize, rateRatioFM_);
		}

		if (internalRatePSG_ != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxSamples, rateRatioPSG_);
			funcInitSincTables(sincTablePSG_, maxSamples, intrSize, rateRatioPSG_);
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
		volumeRatioFM_ = 1;
		volumeRatioPSG_ = 1;
	}

	void OPNA::mix(int16* stream, size_t nSamples)
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		// Set FM buffer
		if (internalRateFM_ == rate_) {
			ym2608_stream_update(id_, bufFM_, nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, rateRatioFM_);
			ym2608_stream_update(id_, tmpBuf_, intrSize);
			#ifdef SINC_INTERPOLATION
			sincInterpolate(tmpBuf_, bufFM_, nSamples, intrSize, sincTableFM_, rateRatioFM_);
			#else
            linearInterpolate(tmpBuf_, bufFM_, nSamples, rateRatioFM_);
			#endif
		}

		// Set PSG buffer
		if (internalRatePSG_ == rate_) {
			ym2608_stream_update_ay(id_, bufPSG_, nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, rateRatioPSG_);
			ym2608_stream_update_ay(id_, tmpBuf_, intrSize);
			#ifdef SINC_INTERPOLATION
			sincInterpolate(tmpBuf_, bufPSG_, nSamples, intrSize, sincTablePSG_, rateRatioPSG_);
			#else
            linearInterpolate(tmpBuf_, bufPSG_, nSamples, rateRatioPSG_);
			#endif
		}

		for (size_t i = 0; i < nSamples; ++i) {
			// Left mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[0][i] + volumeRatioPSG_ * bufPSG_[0][i]);
			// Right mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[1][i] + volumeRatioPSG_ * bufPSG_[1][i]);
		}
	}
}
