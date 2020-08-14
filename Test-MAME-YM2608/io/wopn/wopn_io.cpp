#include "wopn_io.hpp"
#include "wopn_file.h"
#include "../file_io_error.hpp"

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

WopnIo::WopnIo() : AbstractToneBankIo("wopn", "OPN2BankEditor bank", true, false) {}

std::vector<TonePtr> WopnIo::load(const BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	struct WOPNDeleter {
		void operator()(WOPNFile *x) { WOPN_Free(x); }
	};

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
			tone->name = ent.inst->inst_name;
			tone->AL = ent.inst->fbalg & 7;
			tone->FB = (ent.inst->fbalg >> 3) & 7;

			const int refOffs[] = { 0, 2, 1, 3 };
			for (size_t o = 0; o < 4; ++o) {
				Operator& op = tone->op[o];
				const WOPNOperator* refOp = ent.inst->operators + refOffs[o];
				op.ML = refOp->dtfm_30 & 15;
				op.DT = (refOp->dtfm_30 >> 4) & 7;
				op.TL = refOp->level_40;
				op.KS = refOp->rsatk_50 >> 6;
				op.AR = refOp->rsatk_50 & 31;
				op.DR = refOp->amdecay1_60 & 31;
				op.SR = refOp->decay2_70 & 31;
				op.RR = refOp->susrel_80 & 15;
				op.SL = refOp->susrel_80 >> 4;
				op.SSGEG = refOp->ssgeg_90;
				op.AM = refOp->amdecay1_60 >> 7;
			}
			bank.push_back(std::move(tone));
		}
	}

	return bank;
}
