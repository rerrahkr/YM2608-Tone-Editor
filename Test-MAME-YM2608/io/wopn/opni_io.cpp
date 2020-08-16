#include "opni_io.hpp"
#include <cstring>
#include <algorithm>
#include "wopn_file.h"
#include "../file_io_error.hpp"

OpniIo::OpniIo() : AbstractSingleToneIo("opni", "OPN2BankEditor instrument", true, true) {}

Tone* OpniIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	OPNIFile opni;
	if (WOPN_LoadInstFromMem(&opni, const_cast<char*>(container.getPointer()),
							 static_cast<size_t>(container.size())) != 0)
		throw FileCorruptionError(FileIo::FileType::SingleTone);

	tone->name = opni.inst.inst_name;
	tone->AL = opni.inst.fbalg & 7;
	tone->FB = (opni.inst.fbalg >> 3) & 7;

	const int refOffs[] = { 0, 2, 1, 3 };
	for (size_t o = 0; o < 4; ++o) {
		Operator& op = tone->op[o];
		const WOPNOperator* refOp = opni.inst.operators + refOffs[o];
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

	return tone.release();
}

const BinaryContainer OpniIo::save(const Tone& tone) const
{
	BinaryContainer container(std::vector<char>(79));

	OPNIFile opni;
	opni.version = 2;
	opni.is_drum = false;
	WOPNInstrument& inst = opni.inst;

	size_t nameLen = std::min<size_t>(tone.name.length(), sizeof(inst.inst_name) - 1);
	std::strncpy(inst.inst_name, tone.name.c_str(), nameLen);
	inst.inst_name[nameLen] = '\0';
	inst.note_offset = 0;
	inst.percussion_key_number = 0;
	inst.lfosens = 0;
	inst.inst_flags = WOPN_ChipType::WOPN_Chip_OPNA;

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

	if (WOPN_SaveInstToMem(&opni, const_cast<char*>(container.getPointer()),
						   static_cast<size_t>(container.size()), 2) != 0)
		throw FileOutputError(FileIo::FileType::SingleTone);

	return container;
}
