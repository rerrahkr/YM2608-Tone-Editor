#include "original_tone_io.hpp"

Tone* OriginalToneIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	size_t csr = 0;
	if (container.readString(csr, 4) != "TONE")
		return nullptr;
	csr += 4;

	uint32_t hdOffset = container.readUint32(csr);
	csr += 4;
	size_t hdCsr = csr;
	uint32_t strLen = container.readUint32(hdCsr);
	hdCsr += 4;
	tone->name = container.readString(hdCsr, strLen);
	csr += hdOffset;

	tone->AL = container.readUint8(csr++);
	tone->FB = container.readUint8(csr++);
	for (auto& op : tone->op) {
		op.AR = container.readUint8(csr++);
		op.DR = container.readUint8(csr++);
		op.SR = container.readUint8(csr++);
		op.RR = container.readUint8(csr++);
		op.SL = container.readUint8(csr++);
		op.TL = container.readUint8(csr++);
		op.KS = container.readUint8(csr++);
		op.ML = container.readUint8(csr++);
		op.DT = container.readUint8(csr++);
		op.AM = container.readUint8(csr++);
	}

	return tone.release();
}

const BinaryContainer OriginalToneIo::save(const Tone& tone) const
{
	BinaryContainer container;
	container.appendString("TONE");
	uint32_t nameLen = tone.name.size();
	uint32_t hdOffset = 4 + tone.name.length();
	container.appendUint32(hdOffset);
	container.appendUint32(nameLen);
	container.appendString(tone.name);
	container.appendUint8(tone.AL);
	container.appendUint8(tone.FB);
	for (auto& op : tone.op) {
		container.appendUint8(op.AR);
		container.appendUint8(op.DR);
		container.appendUint8(op.SR);
		container.appendUint8(op.RR);
		container.appendUint8(op.SL);
		container.appendUint8(op.TL);
		container.appendUint8(op.KS);
		container.appendUint8(op.ML);
		container.appendUint8(op.DT);
		container.appendUint8(op.AM);
	}
	return container;
}
