#include "opni_io.hpp"
#include "wopn_file.h"
#include "../file_io_error.hpp"
#include "wopn_common.hpp"

OpniIo::OpniIo() : AbstractSingleToneIo("opni", "OPN2BankEditor instrument", true, true) {}

Tone* OpniIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	OPNIFile opni;
	if (WOPN_LoadInstFromMem(&opni, const_cast<char*>(container.getPointer()),
							 static_cast<size_t>(container.size())) != 0)
		throw FileCorruptionError(FileIo::FileType::SingleTone);

	setWOPNInstrumentToTone(opni.inst, tone.get());

	return tone.release();
}

const BinaryContainer OpniIo::save(const Tone& tone) const
{
	BinaryContainer container(std::vector<char>(79));

	OPNIFile opni;
	opni.version = 2;
	opni.is_drum = false;

	setToneToWOPNInstrument(tone, opni.inst);

	if (WOPN_SaveInstToMem(&opni, const_cast<char*>(container.getPointer()),
						   static_cast<size_t>(container.size()), 2) != 0)
		throw FileOutputError(FileIo::FileType::SingleTone);

	return container;
}
