#include "tfi_io.hpp"
#include "file_io_error.hpp"
#include "io_helper.hpp"

TfiIo::TfiIo() : AbstractSingleToneIo("tfi", "TFM Music Maker instrument", true, true) {}

Tone* TfiIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	if (container.size() != 42) throw FileCorruptionError(io::FileType::SingleTone);
	size_t csr = 0;
	tone->AL = container.readUint8(csr++);
	tone->FB = container.readUint8(csr++);
	Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
	for (Operator* op : ops) {
		op->ML = container.readUint8(csr++);
		op->DT = DT_CONV_IN[container.readUint8(csr++)];
		op->TL = container.readUint8(csr++);
		op->KS = container.readUint8(csr++);
		op->AR = container.readUint8(csr++);
		op->DR = container.readUint8(csr++);
		op->SR = container.readUint8(csr++);
		op->RR = container.readUint8(csr++);
		op->SL = container.readUint8(csr++);
		op->SSGEG = container.readUint8(csr++);
	}

	return tone.release();
}

const BinaryContainer TfiIo::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendUint8(tone.AL);
	container.appendUint8(tone.FB);
	for (const size_t o : { 0, 2, 1, 3}) {
		const Operator& op = tone.op[o];
		container.appendUint8(op.ML);
		container.appendUint8(DT_CONV_OUT[op.DT]);
		container.appendUint8(op.TL);
		container.appendUint8(op.KS);
		container.appendUint8(op.AR);
		container.appendUint8(op.DR);
		container.appendUint8(op.SR);
		container.appendUint8(op.RR);
		container.appendUint8(op.SL);
		container.appendUint8(op.SSGEG);
	}

	return container;
}
