/*
 * Copyright (C) 2021 Rerrah
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mame_2608.hpp"
#include <algorithm>
#include "../chip_misc.h"

extern "C"
{
#include "fmopn.h"
}

namespace chip
{
namespace // SSG callbacks
{
void setSsgClock(void* param, uint32_t clock)
{
	auto state = reinterpret_cast<Mame2608State*>(param);
	if (state->ssg) PSG_set_clock(state->ssg, clock);
}

void writeSsg(void* param, uint8_t address, uint8_t data)
{
	auto state = reinterpret_cast<Mame2608State*>(param);
	if (state->ssg) PSG_writeIO(state->ssg, address, data);
}

uint8_t readSsg(void* param)
{
	auto state = reinterpret_cast<Mame2608State*>(param);
	return (state->ssg ? PSG_readIO(state->ssg) : 0);
}

void resetSsg(void* param)
{
	auto state = reinterpret_cast<Mame2608State*>(param);
	if (state->ssg) PSG_reset(state->ssg);
}

const ssg_callbacks SSG_INTF =
{
	setSsgClock,
	writeSsg,
	readSsg,
	resetSsg
};
}

Mame2608::~Mame2608()
{
	stopDevice();
}

int Mame2608::startDevice(int clock, int& rateSsg, uint32_t dramSize)
{
	int clockSsg = clock / 4;
	rateSsg = clockSsg / 8;
	state_.ssg = PSG_new(clockSsg, rateSsg);
	if (!state_.ssg) return 0;
	PSG_setVolumeMode(state_.ssg, 1);	// YM2149 volume mode

	int rate = clock / 144;	// FM synthesis rate is clock / 2 / 72
	state_.chip = ym2608_init(&state_, clock, rate, nullptr, nullptr);
	if (!state_.chip) return 0;
	ym2608_link_ssg(state_.chip, &SSG_INTF, &state_);
	ym2608_alloc_pcmromb(state_.chip, dramSize);

	return rate;
}

void Mame2608::stopDevice()
{
	if (state_.chip) {
		ym2608_shutdown(state_.chip);
		state_.chip = nullptr;
	}
	if (state_.ssg)
	{
		PSG_delete(state_.ssg);
		state_.ssg = nullptr;
	}
}

void Mame2608::resetDevice()
{
	ym2608_reset_chip(state_.chip);	// Also reset SSG
}

void Mame2608::writeAddressToPortA(uint8_t address)
{
	ym2608_write(state_.chip, 0, address);
}

void Mame2608::writeAddressToPortB(uint8_t address)
{
	ym2608_write(state_.chip, 2, address);
}

void Mame2608::writeDataToPortA(uint8_t data)
{
	ym2608_write(state_.chip, 1, data);
}

void Mame2608::writeDataToPortB(uint8_t data)
{
	ym2608_write(state_.chip, 3, data);
}

uint8_t Mame2608::readData()
{
	return ym2608_read(state_.chip, 1);
}

void Mame2608::updateStream(sample** outputs, int nSamples)
{
	ym2608_update_one(state_.chip, nSamples, outputs);
}

void Mame2608::updateSsgStream(sample** outputs, int nSamples)
{
	if (state_.ssg) {
		sample* bufl = outputs[Stereo::LEFT];
		sample* bufr = outputs[Stereo::RIGHT];
		for (int i = 0; i < nSamples; ++i) {
			int16_t s = PSG_calc(state_.ssg) << 1;
			*bufl++ = s;
			*bufr++ = s;
		}
	}
	else {
		std::fill_n(outputs[Stereo::LEFT], nSamples, 0);
		std::fill_n(outputs[Stereo::RIGHT], nSamples, 0);
	}
}
}

namespace
{
sample* DUMMY_BUF[] = { nullptr, nullptr };
}

void ym2608_update_request(void* param)
{
	auto state = reinterpret_cast<chip::Mame2608State*>(param);
	ym2608_update_one(state->chip, 0, DUMMY_BUF);
}
