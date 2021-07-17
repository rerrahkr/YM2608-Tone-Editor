#include "y12_io.hpp"
#include "file_io_error.hpp"

Y12Io::Y12Io() : AbstractSingleToneIo("y12", "Gens KMod dump", true, false) {}

Tone* Y12Io::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	if (container.size() != 128) throw FileCorruptionError(io::FileType::SingleTone);
	size_t csr = 0;
	Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
	for (Operator* op : ops) {
		uint8_t tmp = container.readUint8(csr++);
		op->ML = tmp & 15;
		op->DT = (tmp >> 4) & 7;
		op->TL = container.readUint8(csr++);
		tmp = container.readUint8(csr++);
		op->KS = (tmp >> 6) & 3;
		op->AR = tmp & 31;
		tmp = container.readUint8(csr++);
		op->DR = tmp & 31;
		op->AM = tmp >> 7;
		op->SR = container.readUint8(csr++);
		tmp = container.readUint8(csr++);;
		op->RR = tmp & 15;
		op->SL = (tmp >> 4) & 15;
		op->SSGEG = container.readUint8(csr++);
		csr += 9;
	}
	tone->AL = container.readUint8(csr++);
	tone->FB = container.readUint8(csr++);
	csr += 14;

	return tone.release();
}
