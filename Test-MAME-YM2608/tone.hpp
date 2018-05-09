#pragma once

#include <string>
#include <cstdint>

struct Operator {
	uint8_t AR;
	uint8_t DR;
	uint8_t SR;
	uint8_t RR;
	uint8_t SL;
	uint8_t TL;
	uint8_t KS;
	uint8_t ML;
	uint8_t DT;
	uint8_t AM;
};

struct Tone {
    std::string name;
    std::string path;
	uint8_t AL;
	uint8_t FB;
    Operator op[4];
};
