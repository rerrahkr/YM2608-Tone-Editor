#include "pmd_io.hpp"
#include "file_io_error.hpp"

namespace
{
size_t OP_OFFS[] = { 0, 2, 1, 3 };
}

PmdIo::PmdIo() : AbstractSongFileIo({"m", "m2"}, "PMD song file") {}

std::vector<TonePtr> PmdIo::load(BinaryContainer& container) const
{
	container.setEndian(true);

	if (container.size() < 0x1b) throw FileCorruptionError(io::FileType::SongFile);
	if (container.readUint8(0) != 0) throw FileUnsupportedError(io::FileType::SongFile);

	uint16_t csr = container.readUint16(0x19) + 1;
	if (container.size() < csr) throw FileCorruptionError(io::FileType::SongFile);
	//uint8_t version = container.readUint8(csr - 2);
	uint16_t footerOffs = container.readUint16(csr - 4) + 1;
	if (container.size() < footerOffs) throw FileCorruptionError(io::FileType::SongFile);

	std::vector<TonePtr> tones;
	while (true) {
		uint8_t id = container.readUint8(csr++);

		uint8_t tmp = container.readUint8(csr);
		if (!id && tmp == 0xff) break;

		TonePtr tone(new Tone);
		for (int o = 0; o < 4; ++o) {
			Operator& op = tone->op[o];
			size_t opCsr = csr + OP_OFFS[o];
			tmp = container.readUint8(opCsr);
			opCsr += 4;
			uint8_t ssgeg = (tmp & 0x80) ? 8 : 0;
			op.DT = (tmp & 0x70) >> 4;
			op.ML = tmp & 0x0f;
			tmp = container.readUint8(opCsr);
			opCsr += 4;
			if (tmp & 0x80) ssgeg |= 4;
			op.TL = tmp & 0x7f;
			tmp = container.readUint8(opCsr);
			opCsr += 4;
			op.KS = tmp >> 6;
			op.AR = tmp & 0x1f;
			tmp = container.readUint8(opCsr);
			opCsr += 4;
			op.AM = (tmp & 0x80) >> 7;
			ssgeg |= ((tmp & 0x60) >> 5);
			op.SSGEG = ssgeg;
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

		tone->name = "@" + std::to_string(id);
		tones.push_back(std::move(tone));
	}

	return tones;
}
