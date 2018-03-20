#pragma once

#include <string>
#include <memory>
#include "tone.hpp"

class ToneFile
{
public:
    static std::unique_ptr<Tone> read(const std::string file);
    static void write(const std::string file, Tone* tone);

private:
    ToneFile();
};
