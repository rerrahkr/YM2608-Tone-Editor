#pragma once

#include <string>
#include "tone.hpp"

class ToneConverter
{
public:
    ToneConverter();
    void loadFormat(std::string path);
    std::string convert(const Tone* tone);

private:
    std::string format_;

    struct Manip
    {
        bool isFill0 = false;
        int setw = 3;
        void reset()
        {
            isFill0 = false;
            setw = 3;
        }
    };

    std::string replace(std::string src, const std::string regex, const int value);
    std::string replace(std::string src, const std::string regex, const std::string str);
    void parse(std::string src, Manip& manip);
};
