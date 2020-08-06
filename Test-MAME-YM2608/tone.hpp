#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

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
	std::string name = "name";
	std::string path = "./";
	uint8_t AL = 4;
	uint8_t FB = 0;
	Operator op[4] = {
		{ 31, 0, 0, 7, 0, 32, 0, 0, 0, 0 },
		{ 31, 0, 0, 7, 0, 0, 0, 0, 0, 0 },
		{ 31, 0, 0, 7, 0, 32, 0, 0, 0, 0 },
		{ 31, 0, 0, 7, 0, 0, 0, 0, 0, 0 }
	};
};

using TonePtr = std::shared_ptr<Tone>;
