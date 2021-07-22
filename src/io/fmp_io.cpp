#include "fmp_io.hpp"
#include "file_io_error.hpp"

namespace
{
size_t OP_OFFS[] = { 0, 2, 1, 3 };
}

FmpIo::FmpIo() : AbstractSongFileIo({"opi", "ovi", "ozi", "m26", "m86"}, "FMP/PLAY6 song file") {}

// Refer to https://github.com/takamichih/fmplayer/blob/df38031f493c0cdb35218fda566077103ee1a68e/fmdriver/fmdriver_fmp.c
std::vector<TonePtr> FmpIo::load(BinaryContainer& container) const
{
	container.setEndian(true);

	uint16_t footOffs = container.readUint16(0);
	if (container.size() < footOffs + 4) throw FileCorruptionError(io::FileType::SongFile);

	uint16_t csr;
	std::string magic = container.readString(footOffs, 3);
	if (magic == "FMC") {
		uint8_t version = container.readUint8(footOffs + 3);
		if (version <= 0x29) csr = 0x1c;
		else if (version <= 0x49) csr = 0x32;
		else if (version <= 0x69) csr = 0x66;
		else throw FileUnsupportedError(io::FileType::SongFile);
	}
	else if (magic == "ELF") {
		csr = 0x2e;
	}
	else {
		throw FileCorruptionError(io::FileType::SongFile);
	}

	uint16_t fm1Offs = container.readUint16(2);
	if (container.size() < fm1Offs) throw FileCorruptionError(io::FileType::SongFile);

	std::vector<TonePtr> tones;
	uint8_t id = 0;
	while (csr < fm1Offs) {
		uint8_t tmp = container.readUint8(csr);
		if (!id && tmp == 0xff) break;

		TonePtr tone(new Tone);
		for (int o = 0; o < 4; ++o) {
			Operator& op = tone->op[o];
			size_t opCsr = csr + OP_OFFS[o];
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
		csr += 24;
		tmp = container.readUint8(csr++);
		tone->FB = tmp >> 3;
		tone->AL = tmp & 0x07;

		tone->name = "@" + std::to_string(id++);
		tones.push_back(std::move(tone));
	}

	return tones;
}
