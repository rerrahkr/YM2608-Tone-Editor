#include "tone_file.hpp"
#include <fstream>
#include "types.h"

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
            uint32 hdOffset;
            ifs.read(reinterpret_cast<char*>(&hdOffset), sizeof(uint32));
            size_t strLen;
            ifs.read(reinterpret_cast<char*>(&strLen), sizeof(uint32));
            ifs.read(buffer, strLen);
            tone->name = std::string(buffer, strLen);

            ifs.seekg(4 + sizeof(hdOffset) + hdOffset, std::ios_base::beg);
            ifs.read(reinterpret_cast<char*>(&tone->AL), sizeof(uint8));
            ifs.read(reinterpret_cast<char*>(&tone->FB), sizeof(uint8));
            for (auto& op : tone->op) {
                ifs.read(reinterpret_cast<char*>(&op.AR), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.DR), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.SR), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.RR), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.SL), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.TL), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.KS), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.ML), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.DT), sizeof(uint8));
                ifs.read(reinterpret_cast<char*>(&op.AM), sizeof(uint8));
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
        uint32 hdOffset = sizeof(uint32) + tone->name.size();
        ofs.write(reinterpret_cast<char*>(&hdOffset), sizeof(uint32));
        uint32 nameLen = tone->name.size();
        ofs.write(reinterpret_cast<char*>(&nameLen), sizeof(uint32));
        ofs.write(tone->name.c_str(), tone->name.size());
        ofs.write(reinterpret_cast<char*>(&tone->AL), sizeof(uint8));
        ofs.write(reinterpret_cast<char*>(&tone->FB), sizeof(uint8));
        for (auto& op : tone->op) {
            ofs.write(reinterpret_cast<char*>(&op.AR), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.DR), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.SR), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.RR), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.SL), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.TL), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.KS), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.ML), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.DT), sizeof(uint8));
            ofs.write(reinterpret_cast<char*>(&op.AM), sizeof(uint8));
        }
    }
}
