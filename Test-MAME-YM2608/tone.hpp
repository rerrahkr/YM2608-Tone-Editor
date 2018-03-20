#pragma once

#include <string>
#include "types.h"

struct Operator {
    uint8 AR;
    uint8 DR;
    uint8 SR;
    uint8 RR;
    uint8 SL;
    uint8 TL;
    uint8 KS;
    uint8 ML;
    uint8 DT;
    uint8 AM;
};

struct Tone {
    std::string name;
    std::string path;
    uint8 AL;
    uint8 FB;
    Operator op[4];
};
