#pragma once

#include <cstring>
#include <algorithm>
#include "../../tone.hpp"
#include "wopn_file.h"

inline void setWOPNInstrumentToTone(const WOPNInstrument& inst, Tone* tone)
{
	tone->name = inst.inst_name;
	tone->AL = inst.fbalg & 7;
	tone->FB = (inst.fbalg >> 3) & 7;

	const int refOffs[] = { 0, 2, 1, 3 };
	for (size_t o = 0; o < 4; ++o) {
		Operator& op = tone->op[o];
		const WOPNOperator* refOp = inst.operators + refOffs[o];
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
}

inline void setToneToWOPNInstrument(const Tone& tone, WOPNInstrument& inst)
{
	size_t nameLen = std::min<size_t>(tone.name.length(), sizeof(inst.inst_name) - 1);
	std::strncpy(inst.inst_name, tone.name.c_str(), nameLen);
	inst.inst_name[nameLen] = '\0';
	inst.note_offset = 0;
	inst.percussion_key_number = 0;
	inst.lfosens = 0;
	inst.inst_flags = 0;
	inst.delay_on_ms = 0;
	inst.delay_off_ms = 0;

	const int refOffs[] = { 0, 2, 1, 3 };
	for (size_t o = 0; o < 4; ++o) {
		WOPNOperator& op = inst.operators[o];
		const Operator* refOp = tone.op + refOffs[o];
		op.dtfm_30 = (refOp->DT << 4) | refOp->ML;
		op.level_40 = refOp->TL;
		op.rsatk_50 = (refOp->KS << 6) | refOp->AR;
		op.decay2_70 = refOp->SR;
		op.susrel_80 = (refOp->SL << 4) | refOp->RR;
		op.amdecay1_60 = (refOp->AM << 7) | refOp->DR;
		op.ssgeg_90 = refOp->SSGEG;
	}
	inst.fbalg = (tone.FB << 3) | tone.AL;
}
