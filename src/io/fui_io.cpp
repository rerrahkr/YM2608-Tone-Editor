#include "fui_io.hpp"
#include "file_io_error.hpp"

namespace
{
std::string readNullTerminatedString(const BinaryContainer& container, size_t pos)
{
	std::string str;
	while (true) {
		if (container.size() < pos) break;
		char c = container.readChar(pos++);
		if (c == '\0') break;
		str += c;
	}
	return str;
}

Tone* loadOldFormat(const BinaryContainer& container)
{
	if (container.size() < 24)
		throw FileCorruptionError(io::FileType::SingleTone);

	auto tone = std::make_unique<Tone>();

	size_t csr = 16;
	[[maybe_unused]] uint16_t version = container.readUint16(csr);
	csr += 4;
	uint32_t pointer = container.readUint32(csr);
	if (container.size() < pointer + 8)
		throw FileCorruptionError(io::FileType::SingleTone);
	csr = pointer;

	if (container.readString(csr, 4) != "INST")
		throw FileCorruptionError(io::FileType::SingleTone);
	csr += 4;
	uint32_t blockSize = container.readUint32(csr);
	csr += 4;
	if (container.size() < csr + blockSize)
		throw FileCorruptionError(io::FileType::SingleTone);
	[[maybe_unused]] uint16_t instVersion = container.readUint16(csr);
	csr += 2;
	uint8_t instType = container.readUint8(csr++);
	if (instType != 1)	// Only support OPN
		throw FileUnsupportedError(io::FileType::SingleTone);
	++csr;

	tone->name = readNullTerminatedString(container, csr);
	csr += (tone->name.size() + 1);

	tone->AL = container.readUint8(csr++);
	tone->FB = container.readUint8(csr++);
	tone->PMS_LFO = container.readUint8(csr++);
	tone->AMS_LFO = container.readUint8(csr++);
	csr+= 4;

	Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
	for (Operator* op : ops) {
		op->AM = container.readUint8(csr++);
		op->AR = container.readUint8(csr++);
		op->DR = container.readUint8(csr++);
		op->ML = container.readUint8(csr++);
		op->RR = container.readUint8(csr++);
		op->SL = container.readUint8(csr++);
		op->TL = container.readUint8(csr++);
		csr += 1;
		op->KS = container.readUint8(csr++);
		op->DT = container.readUint8(csr++);
		op->SR = container.readUint8(csr++);
		op->SSGEG = container.readUint8(csr++);
		csr += 20;
	}

	return tone.release();
}

Tone* loadNewFormat(const BinaryContainer& container)
{
	if (container.size() < 12)
		throw FileCorruptionError(io::FileType::SingleTone);

	auto tone = std::make_unique<Tone>();

	size_t csr = 4;
	[[maybe_unused]] uint16_t version = container.readUint16(csr);
	csr += 2;
	uint16_t instType = container.readUint16(csr);
	csr += 2;
	if (instType != 1)	// Only support OPN
		throw FileUnsupportedError(io::FileType::SingleTone);

	while (csr < container.size()) {
		if (container.size() < csr + 4)
			throw FileCorruptionError(io::FileType::SingleTone);

		std::string featureCode = container.readString(csr, 2);
		csr += 2;
		uint16_t blockLen = container.readUint16(csr);
		csr += 2;

		if (container.size() < csr + blockLen)
			throw FileCorruptionError(io::FileType::SingleTone);

		if (featureCode == "NA") {
			// Instrument name
			tone->name = readNullTerminatedString(container, csr);
			csr += (tone->name.size() + 1);
		}
		else if (featureCode == "FM") {
			// FM ins data
			uint8_t flags = container.readUint8(csr++);
			if ((flags & 0x0f) != 4)	// Operator count
				throw FileCorruptionError(io::FileType::SingleTone);

			uint8_t tmp;
			tmp = container.readUint8(csr++);
			tone->AL = tmp >> 4;
			tone->FB = tmp & 7;
			tmp = container.readUint8(csr++);
			tone->AMS_LFO = (tmp >> 3) & 3;
			tone->PMS_LFO = tmp & 7;
			++csr;

			Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
			for (Operator* op : ops) {
				tmp = container.readUint8(csr++);
				op->DT = (tmp >> 4) & 7;
				op->ML = tmp & 15;
				tmp = container.readUint8(csr++);
				op->TL = tmp & 127;
				tmp = container.readUint8(csr++);
				op->KS = tmp >> 6;
				op->AR = tmp & 31;
				tmp = container.readUint8(csr++);
				op->AM = tmp >> 7;
				op->DR = tmp & 31;
				tmp = container.readUint8(csr++);
				op->SR = tmp & 31;
				tmp = container.readUint8(csr++);
				op->SL = tmp >> 4;
				op->RR = tmp & 15;
				tmp = container.readUint8(csr++);
				op->SSGEG = tmp & 15;
				++csr;
			}
		}
		else if (featureCode == "EN") {
			break;
		}
	}

	return tone.release();
}
}

FuiIO::FuiIO() : AbstractSingleToneIo("fui", "Furnace instrument", true, true) {}

Tone* FuiIO::load(const BinaryContainer& container) const
{
	if (container.readString(0, 16) == "-Furnace instr.-") return loadOldFormat(container);
	else if (container.readString(0, 4) == "FINS") return loadNewFormat(container);
	else throw FileCorruptionError(io::FileType::SingleTone);
}

const BinaryContainer FuiIO::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendString("FINS");
	uint16_t version = 0x85;
	container.appendUint16(version);
	container.appendUint16(1);	// FM

	if (!tone.name.empty()) {
		container.appendString("NA");
		container.appendUint16(tone.name.size() + 1);
		container.appendString(tone.name);
		container.appendChar('\0');
	}

	container.appendString("FM");
	container.appendUint16(36);
	container.appendUint8(0xf4);

	container.appendUint8((tone.AL << 4) | tone.FB);
	container.appendUint8((tone.AMS_LFO << 3) | tone.PMS_LFO);
	container.appendUint8(0);

	for (const size_t o : { 0, 2, 1, 3}) {
		const Operator& op = tone.op[o];
		container.appendUint8((op.DT << 4) | op.ML);
		container.appendUint8(op.TL);
		container.appendUint8((op.KS << 6) | op.AR);
		container.appendUint8((op.AM << 7) | op.DR);
		container.appendUint8(op.SR | 0x20);
		container.appendUint8((op.SL << 4) | op.RR);
		container.appendUint8(op.SSGEG);
		container.appendUint8(0);
	}

	return container;
}
