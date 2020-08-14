#include "original_tone_io.hpp"
#include "file_io_error.hpp"

OriginalToneIo::OriginalToneIo() : AbstractSingleToneIo("tone", "FM tone file") {}

Tone* OriginalToneIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	size_t csr = 0;
	std::string ident = container.readString(csr, 4);
	csr += 4;
	if (ident == "TONE") {
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
	}
	else if (ident == "TNV2") {
		uint8_t strLen = container.readUint8(csr++);
		tone->name = container.readString(csr, strLen);
		csr += strLen;

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
			op.SSGEG = container.readUint8(csr++);
		}
	}
	else {
		throw FileCorruptionError(FileIo::FileType::SingleTone);
	}

	return tone.release();
}

const BinaryContainer OriginalToneIo::save(const Tone& tone) const
{
	BinaryContainer container;
	container.appendString("TNV2");
	uint8_t nameLen = static_cast<uint8_t>(tone.name.size());
	container.appendUint8(nameLen);
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
		container.appendUint8(op.SSGEG);
	}
	return container;
}
