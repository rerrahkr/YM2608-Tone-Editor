#include "opna.hpp"
#include <cstdint>
#include <algorithm>
#include "chip_misc.h"

#ifdef  __cplusplus
extern "C"
{
#endif //  __cplusplus

#include "mame/2608intf.h"
#include "nuked/nuke2608intf.h"

#ifdef __cplusplus
}
#endif // __cplusplus

namespace chip
{
	size_t OPNA::count_ = 0;
	
	/*const int OPNA::DEF_AMP_FM_ = 11722;*/
	/*const int OPNA::DEF_AMP_SSG_ = 7250;*/

	OPNA::OPNA(Emu emu, int clock, int rate) :
		Chip(count_++, clock, rate, 110933)	// autoRate = 110933: FM internal rate
	{
		switch (emu) {
		default:
			fprintf(stderr, "Unknown emulator choice. Using the default.\n");
			/* fall through */
		case Emu::Mame:
			fprintf(stderr, "Using emulator: MAME YM2608\n");
			intf_ = &mame_intf2608;
			break;
		case Emu::Nuked:
			fprintf(stderr, "Using emulator: Nuked OPN-Mod\n");
			intf_ = &nuked_intf2608;
			break;
		}

		funcSetRate(rate);

		uint8_t EmuCore = 0;
		intf_->set_ay_emu_core(EmuCore);

		uint8_t AYDisable = 0;	// Enable
		uint8_t AYFlags = 0;	// None
		size_t dramSize = 4;	// Dummy
		internalRate_[FM] = intf_->device_start(
								id_, clock, AYDisable, AYFlags,
								reinterpret_cast<int*>(&internalRate_[SSG]), dramSize);

		initResampler();

		setVolume(0, 0);

		reset();
	}

	OPNA::~OPNA()
	{
		intf_->device_stop(id_);

		--count_;
	}

	void OPNA::reset()
	{
		std::lock_guard<std::mutex> lg(mutex_);

		intf_->device_reset(id_);
	}

	void OPNA::setRegister(uint32_t offset, uint8_t value)
	{
		std::lock_guard<std::mutex> lg(mutex_);

		if (offset & 0x100) {
			intf_->control_port_b_w(id_, 2, offset & 0xff);
			intf_->data_port_b_w(id_, 3, value & 0xff);
		}
		else
		{
			intf_->control_port_a_w(id_, 0, offset & 0xff);
			intf_->data_port_a_w(id_, 1, value & 0xff);
		}
	}

	uint8_t OPNA::getRegister(uint32_t offset) const
	{
		if (offset & 0x100) {
			intf_->control_port_b_w(id_, 2, offset & 0xff);
		}
		else
		{
			intf_->control_port_a_w(id_, 0, offset & 0xff);
		}
		return intf_->read_port_r(id_, 1);
	}

	// TODO: Volume settings
	void OPNA::setVolume(float dBFM, float dBSSG)
	{
		(void)dBFM;
		(void)dBSSG;
		std::lock_guard<std::mutex> lg(mutex_);

		/*dB_[FM] = dBFM;*/
		/*dB_[SSG] = dBSSG;*/

		/*VolumeRatio_[FM] = maxAmplitude_ / defaultFMAmplitude_ * std::pow(10, fmdB / 20);
		VolumeRatio_[SSG] = maxAmplitude_ / defaultSSGAmplitude_ * std::pow(10, ssgdB / 20);*/
		volumeRatio_[FM] = 0.25;
		volumeRatio_[SSG] = 0.25;
	}

	void OPNA::mix(int16_t* stream, size_t nSamples)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		sample **bufFM, **bufPSG;

		// Set FM buffer
		if (internalRate_[FM] == rate_) {
			intf_->stream_update(id_, buffer_[FM], nSamples);
			bufFM = buffer_[FM];
		}
		else {
			size_t intrSize = resampler_[FM].calculateInternalSampleSize(nSamples);
			intf_->stream_update(id_, buffer_[FM], intrSize);
			bufFM = resampler_[FM].interpolate(buffer_[FM], nSamples);
		}

		// Set PSG buffer
		if (internalRate_[SSG] == rate_) {
			intf_->stream_update_ay(id_, buffer_[SSG], nSamples);
			bufPSG = buffer_[SSG];
		}
		else {
			size_t intrSize = resampler_[SSG].calculateInternalSampleSize(nSamples);
			intf_->stream_update_ay(id_, buffer_[SSG], intrSize);
			bufPSG = resampler_[SSG].interpolate(buffer_[SSG], nSamples);
		}
		for (size_t i = 0; i < nSamples; ++i) {
			for (int pan = LEFT; pan <= RIGHT; ++pan) {
				float s = volumeRatio_[FM] * bufFM[pan][i] + volumeRatio_[SSG] * bufPSG[pan][i];
				*stream++ = static_cast<int16_t>(std::clamp(s, -32768.0f, 32767.0f));
			}
		}
	}
}
