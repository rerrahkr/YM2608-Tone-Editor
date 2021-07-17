#include "mucom88_io.hpp"
#include <QTextCodec>
#include "file_io_error.hpp"
#include "../text_conversion.hpp"

Mucom88Io::Mucom88Io() : AbstractToneBankIo("dat", "MUCOM88 voice data", true, true) {}

std::vector<TonePtr> Mucom88Io::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;

	size_t csr = 0;
	// File size check
	size_t ctrSize = container.size();
	if (!ctrSize || ctrSize != 0x2000)
		throw FileCorruptionError(io::FileType::ToneBank);

	size_t opOffs[] = { 0, 2, 1, 3 };
	QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
	for (int i = 0; i < 256; ++i) {
		TonePtr tone(new Tone);
		uint8_t tmp;
		csr++;	// Skip first byte (unknown byte)
		for (int o = 0; o < 4; ++o) {
			Operator& op = tone->op[o];
			size_t opCsr = csr + opOffs[o];
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
			op.AM = (tmp & 0x80) >> 7;
			op.DR = tmp & 0x1f;
			op.SR = container.readUint8(opCsr);
			opCsr += 4;
			tmp = container.readUint8(opCsr);
			opCsr += 4;
			op.SL = tmp >> 4;
			op.RR = tmp & 0x0f;
		}
		size_t toneBegin = csr;
		csr += 24;
		tmp = container.readUint8(csr++);
		tone->FB = tmp >> 3;
		tone->AL = tmp & 0x07;

		std::string name = "";
		for (size_t j = 0; j < 6; ++j) {
			if (char c = container.readChar(csr + j)) name += c;
			else break;
		}
		tone->name = codec->toUnicode(name.c_str(), name.length()).toStdString();
		csr += 6;

		// Empty
		if (std::all_of(container.getPointer() + toneBegin, container.getPointer() + toneBegin + 25,
						[](const char c) { return c == 0; }) && name.empty())
			continue;

		bank.push_back(std::move(tone));
	}

	return bank;
}

const BinaryContainer Mucom88Io::save(const std::vector<TonePtr>& bank) const
{
	BinaryContainer container;

	QTextCodec* codec = QTextCodec::codecForName("Shift-JIS");
	for (const TonePtr& tone : bank) {
		container.appendUint8(0);
		Operator* ops[] = { tone->op, tone->op + 2, tone->op + 1, tone->op + 3 };
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
		container.appendUint8((tone->FB << 3) | tone->AL);

		std::string name = codec->fromUnicode(utf8ToQString(tone->name)).toStdString();
		size_t nameLen = std::min<size_t>(name.length(), 6);
		container.appendString(name.substr(0, nameLen));
		for (size_t l = nameLen; l < 6; ++l) container.appendUint8(0);
	}

	return container;
}
