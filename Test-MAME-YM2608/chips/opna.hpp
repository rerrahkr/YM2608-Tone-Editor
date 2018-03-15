#pragma once

#include "chip.hpp"

namespace chip
{
	class OPNA : public Chip
	{
	public:
		// [rate]
		// 0 = rate is 110933 (internal FM sample rate in 3993600 * 2 clock)
		#ifdef SINC_INTERPOLATION
		OPNA(uint32 clock, uint32 rate, size_t maxTime);
		#else
		OPNA(uint32 clock, uint32 rate);
		#endif
		~OPNA();

		void reset() override;
		void setRegister(uint32 offset, uint32 value) override;
		uint32 getRegister(uint32 offset) const override;

		#ifdef SINC_INTERPOLATION
		void setRate(uint32 rate, size_t maxTime) override;
		#else
		void setRate(uint32 rate) override;
		#endif
		
		void setVolume(float dBFM, float dBPSG);	// NOT work
		void mix(int16* stream, size_t nSamples) override;

	private:
		const int id_;
		static size_t count_;

		int internalRateFM_, internalRatePSG_;
		float rateRatioFM_, rateRatioPSG_;
		sample *bufFM_[2], *bufPSG_[2];		// [0]: left, [1]: right

		/*float dBFM_, dBPSG_;*/
		float volumeRatioFM_, volumeRatioPSG_;
		//static const int MAX_AMP_;
		//static const int DEF_AMP_FM_, DEF_AMP_PSG_;

		#ifdef SINC_INTERPOLATION
		std::vector<float> sincTableFM_, sincTablePSG_;
		#endif

		void funcSetRate(uint32 rate);
		void setRateRatio();

		#ifdef SINC_INTERPOLATION
		void initSincTables(size_t maxTime) override;
		#endif
	};
}
