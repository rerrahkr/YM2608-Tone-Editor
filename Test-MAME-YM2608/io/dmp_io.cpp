#include "dmp_io.hpp"
#include "file_io_error.hpp"
#include "io_helper.hpp"

DmpIo::DmpIo() : AbstractSingleToneIo("dmp", "DefleMask preset", true, true) {}

Tone* DmpIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	size_t csr = 0;
	uint8_t insType = 1; // default to FM
	uint8_t fileVersion = container.readUint8(csr++);
	if (fileVersion == 0) { // older, unversioned dmp
		if (container.size() != 49) throw FileCorruptionError(FileIo::FileType::SingleTone);
	}
	else {
		if (fileVersion < 9) throw FileCorruptionError(FileIo::FileType::SingleTone);
		if (fileVersion == 9 && container.size() != 51) { // make sure it's not for that discontinued chip
			throw FileCorruptionError(FileIo::FileType::SingleTone);
		}
		uint8_t system = 2; // default to genesis
		if (fileVersion >= 11) system = container.readUint8(csr++);
		if (system != 2 && system != 8) { // genesis and arcade only
			throw FileUnsupportedError(FileIo::FileType::SingleTone);
		}
		insType = container.readUint8(csr++);
	}
	if (insType != 1)	// Fm only
		throw  FileUnsupportedError(FileIo::FileType::SingleTone);

	if (fileVersion == 9) csr++; // skip version 9's total operators field
	csr++;	// Skip PMS
	tone->FB = container.readUint8(csr++);
	tone->AL = container.readUint8(csr++);
	csr++;	// Skip AMS

	Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
	for (Operator* op : ops) {
		op->ML = container.readUint8(csr++);
		op->TL = container.readUint8(csr++);
		op->AR = container.readUint8(csr++);
		op->DR = container.readUint8(csr++);
		op->SL = container.readUint8(csr++);
		op->RR = container.readUint8(csr++);
		op->AM = container.readUint8(csr++);
		op->KS = container.readUint8(csr++);
		op->DT = DT_CONV_IN[container.readUint8(csr++) & 15];	// mask out OPM's DT2
		op->SR = container.readUint8(csr++);
		op->SSGEG = container.readUint8(csr++);
	}

	return tone.release();
}

const BinaryContainer DmpIo::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendUint8(11);	// Version
	container.appendUint8(2);	// System: genesis
	container.appendUint8(1);	// FM

	container.appendUint8(0);	// PMS
	container.appendUint8(tone.FB);
	container.appendUint8(tone.AL);
	container.appendUint8(0);	// AMS
	for (const size_t o : { 0, 2, 1, 3}) {
		const Operator& op = tone.op[o];
		container.appendUint8(op.ML);
		container.appendUint8(op.TL);
		container.appendUint8(op.AR);
		container.appendUint8(op.DR);
		container.appendUint8(op.SL);
		container.appendUint8(op.RR);
		container.appendUint8(op.AM);
		container.appendUint8(op.KS);
		container.appendUint8(DT_CONV_OUT[op.DT]);
		container.appendUint8(op.SR);
		container.appendUint8(op.SSGEG);
	}

	return container;
}
