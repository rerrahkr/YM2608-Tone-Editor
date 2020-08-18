#include "bnk_io.hpp"
#include "file_io_error.hpp"

BnkIo::BnkIo() : AbstractToneBankIo("bnk", "GEMS bank", true, false) {}

std::vector<TonePtr> BnkIo::load(BinaryContainer& container) const
{
	std::vector<TonePtr> bank;
	container.setEndian(false);
	if (container.size() < 100) throw FileCorruptionError(FileIo::FileType::ToneBank);
	size_t nMax = (container.size() - 100) / 84;
	bank.reserve(nMax);
	size_t csr = 100;
	for (size_t i = 0; i < nMax; ++i) {
		uint16_t type = container.readUint16(csr);
		if (type != 0)	{	// Not FM
			csr += 80;
			continue;
		}
		csr += 2;
		TonePtr tone(new Tone);
		size_t nameLen = 0;
		for (; nameLen <= 28 && container.readChar(csr + nameLen) != '\0'; ++nameLen)
			;
		tone->name = container.readString(csr, std::min<size_t>(nameLen, 28));
		csr += 30;
		uint8_t tmp = container.readUint8(csr++);
		tone->FB = (tmp >> 3) & 7;
		tone->AL = tmp & 7;
		csr++;
		for (size_t o : { 0, 2, 1, 3 }) {
			Operator& op = tone->op[o];
			tmp = container.readUint8(csr++);
			op.DT = (tmp >> 4) & 7;
			op.ML = tmp & 15;
			op.TL = container.readUint8(csr++) & 127;
			tmp = container.readUint8(csr++);
			op.KS = (tmp >> 5) & 3;
			op.AR = tmp & 31;
			tmp = container.readUint8(csr++);
			op.AM = (tmp >> 7);
			op.DR = tmp & 31;
			op.SR = container.readUint8(csr++) & 31;
			tmp = container.readUint8(csr++);
			op.SL = tmp >> 4;
			op.RR = tmp & 15;
		}
		csr += 22;
		bank.push_back(std::move(tone));
	}

	if (bank.empty()) throw FileUnsupportedError(FileIo::FileType::ToneBank);

	return bank;
}
