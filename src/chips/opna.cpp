#include "opna.hpp"
#include <cstdint>
#include <algorithm>
#include "chip_misc.h"
#include "mame/mame_2608.hpp"
#include "nuked/nuked_2608.hpp"

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
			[[fallthrough]];
		case Emu::Mame:
			fprintf(stderr, "Using emulator: MAME YM2608\n");
			intf_ = std::make_unique<Mame2608>();
			break;
		case Emu::Nuked:
			fprintf(stderr, "Using emulator: Nuked OPN-Mod\n");
			intf_ = std::make_unique<Nuked2608>();
			break;
		}

		funcSetRate(rate);

		constexpr size_t dramSize = 4;	// Dummy
		internalRate_[FM] = intf_->startDevice(clock, internalRate_[SSG], dramSize);

		initResampler();

		setVolume(0, 0);

		reset();
	}

	OPNA::~OPNA()
	{
		intf_->stopDevice();
		--count_;
	}

	void OPNA::reset()
	{
		std::lock_guard<std::mutex> lg(mutex_);
		intf_->resetDevice();
	}

	void OPNA::setRegister(uint32_t offset, uint8_t value)
	{
		std::lock_guard<std::mutex> lg(mutex_);

		if (offset & 0x100) {
			intf_->writeAddressToPortB(offset & 0xff);
			intf_->writeDataToPortB(value & 0xff);
		}
		else {
			intf_->writeAddressToPortA(offset & 0xff);
			intf_->writeDataToPortA(value & 0xff);
		}
	}

	uint8_t OPNA::getRegister(uint32_t offset) const
	{
		if (offset & 0x100) intf_->writeAddressToPortB(offset & 0xff);
		else intf_->writeAddressToPortA(offset & 0xff);
		return intf_->readData();
	}

	// TODO: Volume settings
	void OPNA::setVolume(float /*dBFM*/, float /*dBSSG*/)
	{
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
			intf_->updateStream(buffer_[FM], nSamples);
			bufFM = buffer_[FM];
		}
		else {
			size_t intrSize = resampler_[FM].calculateInternalSampleSize(nSamples);
			intf_->updateStream(buffer_[FM], intrSize);
			bufFM = resampler_[FM].interpolate(buffer_[FM], nSamples);
		}

		// Set PSG buffer
		if (internalRate_[SSG] == rate_) {
			intf_->updateSsgStream(buffer_[SSG], nSamples);
			bufPSG = buffer_[SSG];
		}
		else {
			size_t intrSize = resampler_[SSG].calculateInternalSampleSize(nSamples);
			intf_->updateSsgStream(buffer_[SSG], intrSize);
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
