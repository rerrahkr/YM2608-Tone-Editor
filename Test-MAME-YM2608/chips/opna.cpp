#include "../common.hpp"
#include "opna.hpp"

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
	size_t OPNA::count_ = 0;
	const size_t OPNA::SMPL_BUFSIZE_ = 0x10000;
	//const int OPNA::MAX_AMP_ = 32767;	// half-max of int16
	//const int OPNA::DEF_AMP_FM_ = 11722;
	//const int OPNA::DEF_AMP_PSG_ = 7250;
	const float OPNA::F_PI_ = 3.14159265f;
	const int OPNA::SINC_OFFSET_ = 16;

	OPNA::OPNA(uint32 clock, uint32 rate, size_t maxTime)
		: id_(count_++)
	{
		for (int i = 0; i < 2; ++i) {
			bufFM_[i] = new stream_sample_t[SMPL_BUFSIZE_];
			bufPSG_[i] = new stream_sample_t[SMPL_BUFSIZE_];
			tmpBuf_[i] = new stream_sample_t[SMPL_BUFSIZE_];
		}
		setRate(rate, true);

		UINT8 EmuCore = 0;
		ym2608_set_ay_emu_core(EmuCore);

		UINT8 AYDisable = 0;	// enable
		UINT8 AYFlags = 0;		// none
		internalRateFM_ = device_start_ym2608(id_, clock, AYDisable, AYFlags, reinterpret_cast<int*>(&internalRatePSG_));

		setRate(rate, false);	// Set rate ratio

		setVolume(0, 0);

		initSincTables(maxTime);

		reset();
	}

	void OPNA::initSincTables(size_t maxTime)
	{
		if (internalRateFM_ != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxTime * internalRateFM_ / 1000, internalRateFM_);
			funcInitSincTables(sincTableFM_, intrSize, rateRatioFM_);
		}

		if (internalRatePSG_ != rate_) {
			size_t intrSize = calculateInternalSampleSize(maxTime * internalRatePSG_ / 1000, internalRatePSG_);
			funcInitSincTables(sincTablePSG_, intrSize, rateRatioPSG_);
		}
	}

	void OPNA::funcInitSincTables(std::vector<float>& vector, size_t intrSize, float rateRatio)
	{
		if (vector.size()) vector.clear();

		for (size_t j = 0; j < intrSize; ++j) {
			float rcurn = j * rateRatio;
			int curn = static_cast<int>(rcurn);
			int k = curn - SINC_OFFSET_;
			if (k < 0) k = 0;
			int end = curn + SINC_OFFSET_;
			if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
			for (; k < end; ++k) {
				float dif = rcurn - k;
				vector.push_back(sinc(F_PI_ * dif));
			}
		}
	}

	OPNA::~OPNA()
	{
		device_stop_ym2608(id_);

		for (int i = 0; i < 2; ++i) {
			delete[] bufFM_[i];
			delete[] bufPSG_[i];
			delete[] tmpBuf_[i];
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
			sincResample(bufFM_, intrSize, sincTableFM_, rateRatioFM_);
		}

		// Set PSG buffer
		if (internalRatePSG_ == rate_) {
			ym2608_stream_update_ay(id_, bufPSG_, nSamples);
		}
		else {
			size_t intrSize = calculateInternalSampleSize(nSamples, internalRatePSG_);
			ym2608_stream_update_ay(id_, tmpBuf_, intrSize);
			sincResample(bufPSG_, intrSize, sincTablePSG_, rateRatioPSG_);
		}

		for (size_t i = 0; i < nSamples; ++i) {
			// Left mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[0][i] + volumeRatioPSG_ * bufPSG_[0][i]);
			// Right mix
			*stream++ = static_cast<int16>(volumeRatioFM_ * bufFM_[1][i] + volumeRatioPSG_ * bufPSG_[1][i]);
		}
	}

	void OPNA::sincResample(sample** dest, size_t intrSize, std::vector<float>& vector, float rateRatio)
	{
		for (size_t i = 0; i < 2; ++i) {
			int vecIndex = 0;
			for (size_t j = 0; j < intrSize; ++j) {
				int curn = static_cast<int>(j * rateRatio);
				int k = curn - SINC_OFFSET_;
				if (k < 0) k = 0;
				int end = curn + SINC_OFFSET_;
				if (static_cast<size_t>(end) > intrSize) end = static_cast<int>(intrSize);
				sample samp = 0;
				for (; k < end; ++k) {
				#ifdef DEBUG
					samp += static_cast<sample>(tmpBuf_[i][k] * sinc(F_PI_ * (curn - k)));
				#else
					samp += static_cast<sample>(tmpBuf_[i][k] * vector[vecIndex]);
					++vecIndex;
				#endif
				}
				dest[i][j] = samp;
			}
		}
	}
}