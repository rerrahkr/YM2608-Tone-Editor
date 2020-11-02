#include "ins_io.hpp"
#include "file_io_error.hpp"

InsIo::InsIo() : AbstractSingleToneIo("ins", "MVSTracker instrument", true, true) {}

Tone* InsIo::load(const BinaryContainer& container) const
{
	auto tone = std::make_unique<Tone>();

	size_t csr = 0;
	if (container.readString(csr, 4) != "MVSI") throw FileInputError(FileIo::FileType::SingleTone);
	csr += 4;
	/*uint8_t fileVersion = */std::stoi(container.readString(csr++, 1));
	size_t nameCsr = 0;
	while (container.readChar(nameCsr++) != '\0')
		;
	tone->name = container.readString(csr, nameCsr - csr);
	csr = nameCsr;

	if (container.size() - csr != 25) throw FileInputError(FileIo::FileType::SingleTone);

	uint8_t tmp;
	for (size_t o = 0; o < 4; ++o) {
		Operator& op = tone->op[o];
		size_t opCsr = csr + o;
		tmp = container.readUint8(opCsr);
		opCsr += 4;
		op.DT = (tmp & 0x70) >> 4;
		op.ML = tmp & 0x0f;
		tmp = container.readUint8(opCsr);
		opCsr += 4;
		op.TL = tmp & 0x7f;
		tmp = container.readUint8(opCsr);
		opCsr += 4;
		op.KS = tmp >> 6;
		op.AR = tmp & 0x1f;
		tmp = container.readUint8(opCsr);
		opCsr += 4;
		op.DR = tmp & 0x1f;
		op.SR = container.readUint8(opCsr);
		opCsr += 4;
		tmp = container.readUint8(opCsr);
		opCsr += 4;
		op.SL = tmp >> 4;
		op.RR = tmp & 0x0f;
	}
	csr += 24;
	tmp = container.readUint8(csr++);
	tone->FB = tmp >> 3;
	tone->AL = tmp & 0x07;

	return tone.release();
}

const BinaryContainer InsIo::save(const Tone& tone) const
{
	BinaryContainer container;

	container.appendString("MVSI");
	container.appendChar('1');
	container.appendString(tone.name);

	const Operator* ops[] = { tone.op, tone.op + 1, tone.op + 2, tone.op + 3 };
	for (const auto op : ops)
		container.appendUint8((op->DT << 4) | op->ML);
	for (const auto op : ops)
		container.appendUint8(op->TL);
	for (const auto op : ops)
		container.appendUint8((op->KS << 6) | op->AR);
	for (const auto op : ops)
		container.appendUint8((op->AM << 7) | op->DR);
	for (const auto op : ops)
		container.appendUint8(op->SR);
	for (const auto op : ops)
		container.appendUint8((op->SL << 4) | op->RR);
	container.appendUint8((tone.FB << 3) | tone.AL);

	return container;
}
