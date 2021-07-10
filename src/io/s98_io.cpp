#include "s98_io.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include "file_io_error.hpp"
#include "register_recorder.hpp"

S98Io::S98Io() : AbstractSongFileIo("s98", "S98 file") {}

std::vector<TonePtr> S98Io::load(BinaryContainer& container) const
{
	if (container.size() < 0x20) throw FileCorruptionError(FileIo::FileType::SongFile);

	std::string ident = container.readString(0, 3);
	if (ident != "S98") throw FileCorruptionError(FileIo::FileType::SongFile);
	char version = container.readChar(3);
	if (version != '3') throw  FileUnsupportedError(FileIo::FileType::SongFile);

	size_t devCnt = container.readUint32(0x1c);
	std::vector<RegisterRecorder::Chip> chips;
	if (!devCnt) {
		chips.push_back(RegisterRecorder::YM2608);
	}
	else {
		static const std::unordered_map<uint8_t, RegisterRecorder::Chip> DEV_TYPE = {
			{ 0, RegisterRecorder::Unsupported },
			{ 1, RegisterRecorder::Unsupported },
			{ 2, RegisterRecorder::YM2203 },
			{ 3, RegisterRecorder::YM2612 },
			{ 4, RegisterRecorder::YM2608 },
			{ 5, RegisterRecorder::Unsupported },
			{ 6, RegisterRecorder::Unsupported },
			{ 7, RegisterRecorder::Unsupported },
			{ 8, RegisterRecorder::Unsupported },
			{ 9, RegisterRecorder::Unsupported },
			{ 15, RegisterRecorder::Unsupported },
			{ 6, RegisterRecorder::Unsupported }
		};
		for (size_t i = 0; i < devCnt; ++i) {
			chips.push_back(DEV_TYPE.at(container.readUint8(0x20 + i * 0x10)));
		}
	}

	RegisterRecorder rec(container.readUint32(8) / container.readUint8(4));

	size_t csr = container.readUint32(0x14);
	for (bool flag = true; flag;) {
		switch (uint8_t com = container.readUint8(csr++)) {
		case 0xff:	// Sync
			rec.elapse(1);
			break;
		case 0xfe:	// nSync
		{
			size_t count = 0;
			for (size_t s = 0; ; s += 7) {
				uint8_t l = container.readUint8(csr++);
				count |= (l & 0x7fu) << s;
				if (!(l & 0x80)) break;
			}
			rec.elapse(count);
			break;
		}
		case 0xfd:	// End
			flag = false;
			break;
		default:
		{
			RegisterRecorder::Chip chip = chips.at(com / 2);
			if (com % 2) rec.writeB(container.readUint8(csr), container.readUint8(csr + 1), chip);
			else rec.writeA(container.readUint8(csr), container.readUint8(csr + 1), chip);
			csr += 2;
			break;
		}
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
