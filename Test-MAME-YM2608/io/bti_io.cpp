#include "bti_io.hpp"
#include "file_io_error.hpp"

Tone* BtiIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	size_t globCsr = 0;
	if (container.readString(globCsr, 16) != "BambooTrackerIst")
		throw FileCorruptionError(FileIo::FileType::SingleTone);
	globCsr += 16;
	/*size_t eofOfs = */container.readUint32(globCsr);
	globCsr += 8;	// Skip also file version

	/***** Instrument section *****/
	if (container.readString(globCsr, 8) != "INSTRMNT")
		throw FileCorruptionError(FileIo::FileType::SingleTone);
	globCsr += 8;
	size_t instOfs = container.readUint32(globCsr);
	size_t instCsr = globCsr + 4;
	size_t nameLen = container.readUint32(instCsr);
	instCsr += 4;
	std::string name = u8"";
	if (nameLen > 0) {
		name = container.readString(instCsr, nameLen);
		instCsr += nameLen;
	}
	if (container.readUint8(instCsr++) != 0x00)	// Not FM
		throw FileUnsupportedError(FileIo::FileType::SingleTone);
	globCsr += instOfs;

	/***** Instrument property section *****/
	if (container.readString(globCsr, 8) != "INSTPROP")
		throw FileCorruptionError(FileIo::FileType::SingleTone);
	else {
		globCsr += 8;
		size_t instPropOfs = container.readUint32(globCsr);
		size_t instPropCsr = globCsr + 4;
		globCsr += instPropOfs;
		while (instPropCsr < globCsr) {
			uint8_t secId = container.readUint8(instPropCsr++);
			if (secId == 0x00) {
				/*uint8_t ofs = container.readUint8(instPropCsr);*/
				size_t csr = instPropCsr + 1;
				uint8_t tmp = container.readUint8(csr++);
				tone->AL = tmp >> 4;
				tone->FB = tmp & 0x0f;
				// Operators
				for (size_t o = 0; o < 4; ++o) {
					Operator& op = tone->op[o];
					tmp = container.readUint8(csr++);
					op.AR = tmp & 0x1f;
					tmp = container.readUint8(csr++);
					op.KS = tmp >> 5;
					op.DR = tmp & 0x1f;
					tmp = container.readUint8(csr++);
					op.DT = tmp >> 5;
					op.SR = tmp & 0x1f;
					tmp = container.readUint8(csr++);
					op.SL = tmp >> 4;
					op.RR = tmp & 0x0f;
					tmp = container.readUint8(csr++);
					op.TL = tmp;
					tmp = container.readUint8(csr++);
					op.ML = tmp & 0x0f;
					// op.SSGEG = (tmp & 0x80) ? -1 : ((tmp >> 4) & 0x07);
				}
				break;
			}
			else if (secId == 0x01) {	// FM LFO
				uint8_t ofs = container.readUint8(instPropCsr);
				instPropCsr += ofs;
			}
			else if (secId == 0x40) {	// ADPCM sample
				uint32_t ofs = container.readUint32(instPropCsr);
				instPropCsr += ofs;
			}
			else if (secId <= 0x29 || (0x30 <= secId && secId <= 0x34)
					 || (0x41 <= secId && secId <= 0x43)) {	// Sequence
				uint16_t ofs = container.readUint16(instPropCsr);
				instPropCsr += ofs;
			}
			else {
				throw FileCorruptionError(FileIo::FileType::SingleTone);
			}
		}
	}

	return tone.release();
}

const BinaryContainer BtiIo::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendString("BambooTrackerIst");
	size_t eofOfs = container.size();
	container.appendUint32(0);	// Dummy EOF offset
	uint32_t fileVersion = 0x00010400;	// v1.4.0
	container.appendUint32(fileVersion);

	/***** Instrument section *****/
	container.appendString("INSTRMNT");
	size_t instOfs = container.size();
	container.appendUint32(0);	// Dummy instrument section offset

	container.appendUint32(tone.name.length());
	if (!tone.name.empty()) container.appendString(tone.name);
	container.appendUint8(0x00);	// FM
	container.appendUint8(0);
	for (int i = 0; i < 10; ++i) container.appendUint8(0x80);
	container.writeUint32(instOfs, container.size() - instOfs);

	/***** Instrument property section *****/
	container.appendString("INSTPROP");
	size_t instPropOfs = container.size();
	container.appendUint32(0);	// Dummy instrument property section offset

	// FM envelope
	{
		container.appendUint8(0x00);
		size_t ofs = container.size();
		container.appendUint8(0);	// Dummy offset
		container.appendUint8(tone.AL << 4 | tone.FB);
		// Operators
		for (size_t o = 0; o < 4; ++o) {
			const Operator& op = tone.op[o];
			container.appendUint8(0x20 | op.AR);
			container.appendUint8((op.KS << 5) | op.DR);
			container.appendUint8((op.DT << 5) | op.SR);
			container.appendUint8((op.SL << 4) | op.RR);
			container.appendUint8(op.TL);
			container.appendUint8(op.ML);
		}
		container.writeUint8(ofs, static_cast<uint8_t>(container.size() - ofs));
	}

	container.writeUint32(instPropOfs, container.size() - instPropOfs);

	container.writeUint32(eofOfs, container.size() - eofOfs);

	return container;
}
