#include "vgi_io.hpp"
#include "file_io_error.hpp"
#include "io_helper.hpp"

VgiIo::VgiIo() : AbstractSingleToneIo("vgi", "VGM Music Maker instrument", true, true) {}

Tone* VgiIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	if (container.size() != 43) throw FileCorruptionError(io::FileType::SingleTone);
	size_t csr = 0;
	tone->AL = container.readUint8(csr++);
	tone->FB = container.readUint8(csr++);
	uint8_t tmp = container.readUint8(csr++);
	tone->PMS_LFO = tmp & 7;
	tone->AMS_LFO = (tmp >> 4) & 3;
	if (tmp) tone->FREQ_LFO = 8;
	for (size_t o = 0; o < 4; ++o) {
		Operator& op = tone->op[o];
		op.ML = container.readUint8(csr++);
		op.DT = DT_CONV_IN[container.readUint8(csr++)];
		op.TL = container.readUint8(csr++);
		op.KS = container.readUint8(csr++);
		op.AR = container.readUint8(csr++);
		uint8_t tmp = container.readUint8(csr++);
		op.DR = tmp & 31;
		op.AM = tmp >> 7;
		op.SR = container.readUint8(csr++);
		op.RR = container.readUint8(csr++);
		op.SL = container.readUint8(csr++);
		op.SSGEG = container.readUint8(csr++);
	}

	return tone.release();
}

const BinaryContainer VgiIo::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendUint8(tone.AL);
	container.appendUint8(tone.FB);
	container.appendUint8((tone.AMS_LFO << 4) | tone.PMS_LFO);
	for (size_t o = 0; o < 4; ++o) {
		const Operator& op = tone.op[o];
		container.appendUint8(op.ML);
		container.appendUint8(DT_CONV_OUT[op.DT]);
		container.appendUint8(op.TL);
		container.appendUint8(op.KS);
		container.appendUint8(op.AR);
		container.appendUint8((op.AM << 7) | op.DR);
		container.appendUint8(op.SR);
		container.appendUint8(op.RR);
		container.appendUint8(op.SL);
		container.appendUint8(op.SSGEG);
	}

	return container;
}
