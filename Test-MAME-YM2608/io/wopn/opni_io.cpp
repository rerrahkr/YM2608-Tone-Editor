#include "opni_io.hpp"
#include "wopn_file.h"
#include "../file_io_error.hpp"

OpniIo::OpniIo() : AbstractSingleToneIo("opni", "OPN2BankEditor instrument", true, false) {}

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
