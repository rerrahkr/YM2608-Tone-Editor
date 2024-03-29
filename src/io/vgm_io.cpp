#include "vgm_io.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "file_io_error.hpp"
#include "register_recorder.hpp"

VgmIo::VgmIo() : AbstractSongFileIo("vgm", "VGM file") {}

std::vector<TonePtr> VgmIo::load(BinaryContainer& container) const
{
	if (container.size() < 0x40) throw FileCorruptionError(io::FileType::SongFile);

    std::string ident = container.readString(0, 4);
    if (ident != "Vgm ") throw FileCorruptionError(io::FileType::SongFile);

	container.setEndian(true);
    uint32_t eof = container.readUint32(4);
    if (container.size() - 4 != eof) throw FileCorruptionError(io::FileType::SongFile);

	uint32_t version = container.readUint32(0x08);
	size_t csr = (version < 0x150) ? 0x40 : (0x34 + container.readUint32(0x34));
	if (container.size() <= csr) throw FileCorruptionError(io::FileType::SongFile);

	RegisterRecorder rec(44100);

	for (bool flag = true; flag;) {
		switch (uint8_t com = container.readUint8(csr++)) {
		case 0x52:	// YM2612 Port A
			rec.writeA(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2612);
			csr += 2;
			break;
		case 0x53:	// YM2612 Port B
			rec.writeB(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2612);
			csr += 2;
			break;
		case 0x55:	// YM2203
			rec.writeA(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2203);
			csr += 2;
			break;
		case 0x56:	// YM2608 Port A
			rec.writeA(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2608);
			csr += 2;
			break;
		case 0x57:	// YM2608 Port B
			rec.writeB(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2608);
			csr += 2;
			break;
		case 0x58:	// YM2610 Port A
			rec.writeA(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2610);
			csr += 2;
			break;
		case 0x59:	// YM2610 Port B
			rec.writeB(container.readUint8(csr), container.readUint8(csr + 1), RegisterRecorder::YM2610);
			csr += 2;
			break;
		case 0x61:	// Wait
			rec.elapse(container.readUint16(csr));
			csr += 2;
			break;
		case 0x62:	// Wait 1/60s
			rec.elapse(735);
			break;
		case 0x63:	// Wait 1/50s
			rec.elapse(882);
			break;
		case 0x66:	// End of data
			flag = false;
			break;
		case 0x67:	// Data block
			csr += (container.readUint32(csr + 2) + 6);
			break;
		case 0x90:	// Setup stream control
		case 0x91:	// Set stream data
		case 0x92:	// Set stream frequency
		case 0x93:	// Start stream
		case 0x95:	// Stop stream (fast call)
		case 0xe0:	//
			csr += 4;
			break;
		case 0x94:	// Stop stream
			csr += 1;
			break;
		default:
			if (0x6f < com & com < 0x80) {	// Wait a bit
				rec.elapse(com - 0x6f);
			}
			else {
				throw FileUnsupportedError(io::FileType::SongFile);
			}
			break;
		}
	}

	std::vector<UniqueTone> recTones = rec.finish();
	std::vector<TonePtr> tones;
	tones.reserve(recTones.size());
	for (auto& recTone : recTones) {
		int msec10Time = static_cast<int>(std::round(recTone.time * 100));
		int msec10 = msec10Time % 100;
		int sec = (msec10Time / 100) % 60;
		int min = msec10Time / 6000;
		auto tone = recTone.tone;
		std::ostringstream oss;
		oss << min << ":"
			<< std::setfill('0') << std::setw(2) << sec << ":"
			<< std::setw(2) << msec10;
		oss << ", Ch" << std::setfill(' ') << recTone.channel;
		tone->name = oss.str();
		tones.push_back(TonePtr(tone));
	}

	return tones;
}
