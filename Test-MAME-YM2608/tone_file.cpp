#include "tone_file.hpp"
#include <fstream>
#include <cstdint>

ToneFile::ToneFile() {}

std::unique_ptr<Tone> ToneFile::read(const std::string file)
{
    std::unique_ptr<Tone> tone = std::make_unique<Tone>();

    std::ifstream ifs(file, std::ios::in | std::ios::binary);

    if (ifs) {
        tone->path = file;

        char buffer[100];
        std::string str;
        ifs.read(buffer, 4);
        str = std::string(buffer, 4);
        if (str == "TONE") {
			uint32_t hdOffset;
			ifs.read(reinterpret_cast<char*>(&hdOffset), sizeof(uint32_t));
			uint32_t strLen;
			ifs.read(reinterpret_cast<char*>(&strLen), sizeof(uint32_t));
            ifs.read(buffer, strLen);
            tone->name = std::string(buffer, strLen);

            ifs.seekg(4 + sizeof(hdOffset) + hdOffset, std::ios_base::beg);
			ifs.read(reinterpret_cast<char*>(&tone->AL), sizeof(uint8_t));
			ifs.read(reinterpret_cast<char*>(&tone->FB), sizeof(uint8_t));
            for (auto& op : tone->op) {
				ifs.read(reinterpret_cast<char*>(&op.AR), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.DR), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.SR), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.RR), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.SL), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.TL), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.KS), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.ML), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.DT), sizeof(uint8_t));
				ifs.read(reinterpret_cast<char*>(&op.AM), sizeof(uint8_t));
            }
        }
    }

    return tone;
}

void ToneFile::write(const std::string file, Tone* tone)
{
    std::ofstream ofs(file, std::ios::out | std::ios::binary);

    if (ofs) {
        ofs.write("TONE", 4);
		uint32_t hdOffset = sizeof(uint32_t) + tone->name.size();
		ofs.write(reinterpret_cast<char*>(&hdOffset), sizeof(uint32_t));
		uint32_t nameLen = tone->name.size();
		ofs.write(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
        ofs.write(tone->name.c_str(), tone->name.size());
		ofs.write(reinterpret_cast<char*>(&tone->AL), sizeof(uint8_t));
		ofs.write(reinterpret_cast<char*>(&tone->FB), sizeof(uint8_t));
        for (auto& op : tone->op) {
			ofs.write(reinterpret_cast<char*>(&op.AR), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.DR), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.SR), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.RR), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.SL), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.TL), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.KS), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.ML), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.DT), sizeof(uint8_t));
			ofs.write(reinterpret_cast<char*>(&op.AM), sizeof(uint8_t));
        }
    }
}
