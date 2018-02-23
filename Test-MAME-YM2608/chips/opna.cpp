#include "opna.hpp"
#include "../misc.hpp"

#ifdef  __cplusplus
extern "C"
{
#endif //  __cplusplus

#include "mame/mamedef.h"
#include "mame/2608intf.h"

	UINT8 CHIP_SAMPLING_MODE = 0x00;
	INT32 CHIP_SAMPLE_RATE;
	stream_sample_t* DUMMYBUF[] = { nullptr, nullptr };

#ifdef __cplusplus
}
#endif // __cplusplus

namespace chip
{
	//const int OPNA::MAX_AMP_ = 32767;	// half-max of int16
	//const int OPNA::DEF_AMP_FM_ = 11722;
	//const int OPNA::DEF_AMP_PSG_ = 7250;

	OPNA::OPNA(uint32 clock, uint32 rate)
		: id_(0)
	{
		bufFM_[0] = new stream_sample_t[SMPL_BUFSIZE];
		bufFM_[1] = new stream_sample_t[SMPL_BUFSIZE];
		bufPSG_[0] = new stream_sample_t[SMPL_BUFSIZE];
		bufPSG_[1] = new stream_sample_t[SMPL_BUFSIZE];
		tmpBuf_[0] = new stream_sample_t[SMPL_BUFSIZE];
		tmpBuf_[1] = new stream_sample_t[SMPL_BUFSIZE];

		init(clock, rate);
	}

	OPNA::~OPNA()
	{
		deinit();

		delete[] bufFM_[0];
		delete[] bufFM_[1];
		delete[] bufPSG_[0];
		delete[] bufPSG_[1];
		delete[] tmpBuf_[0];
		delete[] tmpBuf_[1];
	}

	void OPNA::init(uint32 clock, uint32 rate)
	{
		setRate(rate, true);

		UINT8 EmuCore = 0;
		ym2608_set_ay_emu_core(EmuCore);

		UINT8 AYDisable = 0;	// enable
		UINT8 AYFlags = 0;		// none
		internalRateFM_ = device_start_ym2608(id_, clock, AYDisable, AYFlags, reinterpret_cast<int*>(&internalRatePSG_));

		setRate(rate, false);	// Set rate ratio

		setVolume(0, 0);

		reset();
	}

	void OPNA::reset()
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex

		device_reset_ym2608(id_);
	}

	void OPNA::deinit()
	{
		device_stop_ym2608(id_);
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

	void OPNA::setRate(uint32 rate)
	{
		std::lock_guard<std::mutex> lg(mutex_);	// Do mutex
		setRate(rate, false);
	}

	void OPNA::setRate(uint32 rate, bool isInit)
	{
		rate_ = CHIP_SAMPLE_RATE = ((rate) ? rate : 110933);

		if (!isInit) {
			rateRatioFM_ = static_cast<float>(internalRateFM_) / rate_;
			rateRatioPSG_ = static_cast<float>(internalRatePSG_) / rate_;
		}
	}

	uint32 OPNA::getRate() const
	{
		return rate_;
	}

	// UNDONE: âπó ÇÃè⁄ç◊í≤êÆåªç›FM*6/PSG*3=3.1...
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
			size_t intrSize = calculateInternalSampleSize(nSamples, internalRateFM_);
			ym2608_stream_update(id_, tmpBuf_, intrSize);
			sincResample(bufFM_, intrSize, rateRatioFM_);
		}

		// Set PSG buffer
		if (internalRatePSG_ == rate_) {
			ym2608_stream_update_ay(id_, bufPSG_, nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, internalRatePSG_);
			ym2608_stream_update_ay(id_, tmpBuf_, intrSize);
			sincResample(bufPSG_, intrSize, rateRatioPSG_);
		}

		for (size_t i = 0; i < nSamples; ++i) {
			// Left mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[0][i] + volumeRatioPSG_ * bufPSG_[0][i]);
			// Right mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[1][i] + volumeRatioPSG_ * bufPSG_[1][i]);
		}
	}

	// UNDONE: çÇë¨âª
	void OPNA::sincResample(sample** dest, size_t intrSize, float rateRatio)
	{
		int offset = 16;
		for (size_t i = 0; i < 2; ++i) {
			for (size_t j = 0; j < intrSize; ++j) {
				int curn = static_cast<int>(j * rateRatio);
				int k = curn - offset;
				if (k < 0) k = 0;
				int end = curn + offset;
				if (end > intrSize) end = static_cast<int>(intrSize);
				sample samp = 0;
				for (; k < end; ++k) {
					samp += static_cast<sample>(tmpBuf_[i][k] * sinc(F_PI * (curn - k)));
				}
				dest[i][j] = samp;
			}
		}
	}
}