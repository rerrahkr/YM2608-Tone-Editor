#include "wopn_io.hpp"
#include <algorithm>
#include <cmath>
#include "wopn_file.h"
#include "../file_io_error.hpp"
#include "wopn_common.hpp"

struct InstEntry
{
	WOPNInstrument *inst;
	struct ValuesType
	{
		bool percussive : 1;
		unsigned msb : 7;
		unsigned lsb : 7;
		unsigned nth : 7;
	} vals;
};

struct WOPNDeleter {
	void operator()(WOPNFile *x) { WOPN_Free(x); }
};

WopnIo::WopnIo() : AbstractToneBankIo("wopn", "OPN2BankEditor bank", true, true) {}

std::vector<TonePtr> WopnIo::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	std::unique_ptr<WOPNFile, WOPNDeleter> wopn;
	wopn.reset(WOPN_LoadBankFromMem(const_cast<char*>(container.getPointer()), container.size(), nullptr));
	if (!wopn)
		throw FileCorruptionError(FileIo::FileType::ToneBank);

	unsigned numM = wopn->banks_count_melodic;
	unsigned numP = wopn->banks_count_percussion;

	size_t instMax = 128 * (numP + numM);
	std::vector<InstEntry> entries;
	entries.reserve(instMax);

	for (size_t i = 0; i < instMax; ++i) {
		InstEntry ent;
		ent.vals.percussive = (i / 128) >= numM;
		WOPNBank& wbank = ent.vals.percussive
						  ? wopn->banks_percussive[(i / 128) - numM]
						  : wopn->banks_melodic[i / 128];
		//ent.vals.msb = bank.bank_midi_msb;
		//ent.vals.lsb = bank.bank_midi_lsb;
		ent.vals.nth = i % 128;
		ent.inst = &wbank.ins[ent.vals.nth];
		if ((ent.inst->inst_flags & WOPN_Ins_IsBlank) == 0) {
			TonePtr tone(new Tone);
			setWOPNInstrumentToTone(*ent.inst, tone.get());
			bank.push_back(std::move(tone));
		}
	}

	return bank;
}

const BinaryContainer WopnIo::save(const std::vector<TonePtr>& bank) const
{
	const size_t maxNBank = 128;
	uint16_t nBank = std::ceil(static_cast<double>(bank.size()) / maxNBank);
	BinaryContainer container(std::vector<char>(18 + (34 + 69 * maxNBank) * (nBank + 1)));

	std::unique_ptr<WOPNFile, WOPNDeleter> wopn(WOPN_Init(nBank, 0));
	wopn->version = 2;
	wopn->lfo_freq = 0;
	wopn->chip_type = WOPN_Chip_OPNA;

	for (size_t o = 0; o < nBank; ++o) {
		WOPNBank& wbank = wopn->banks_melodic[o];
		wbank.bank_midi_msb = o >> 7;
		wbank.bank_midi_lsb = o & 127;
		size_t max = std::min<size_t>(bank.size() - maxNBank * o, maxNBank);
		for (size_t i = 0; i < max; ++i) {
			setToneToWOPNInstrument(*bank.at(maxNBank * o + i), wbank.ins[i]);
		}
	}

	if (WOPN_SaveBankToMem(wopn.get(), const_cast<char*>(container.getPointer()),
						   static_cast<size_t>(container.size()), 2, 0) != 0)
		throw FileOutputError(FileIo::FileType::ToneBank);

	return container;
}
