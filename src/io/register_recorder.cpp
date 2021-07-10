#include "register_recorder.hpp"
#include <algorithm>
#include "tone.hpp"

namespace
{
enum EnvelopeRegister
{
	DTML1,	DTML2,	DTML3,	DTML4,
	TL1,	TL2,	TL3,	TL4,
	KSAR1,	KSAR2,	KSAR3,	KSAR4,
	AMDR1,	AMDR2,	AMDR3,	AMDR4,
	SR1,	SR2,	SR3,	SR4,
	SLRR1,	SLRR2,	SLRR3,	SLRR4,
	SSGEG1,	SSGEG2,	SSGEG3,	SSGEG4,
	FBAL,
	LAST, FIRST = DTML1
};

constexpr uint8_t ADDR_OFFS = 0x30;
constexpr uint8_t REG_ADDRS[] =
{
	0x30 - ADDR_OFFS,	// DTML1
	0x38 - ADDR_OFFS,	// DTML2
	0x34 - ADDR_OFFS,	// DTML3
	0x3c - ADDR_OFFS,	// DTML4
	0x40 - ADDR_OFFS,	// TL1
	0x48 - ADDR_OFFS,	// TL2
	0x44 - ADDR_OFFS,	// TL3
	0x4c - ADDR_OFFS,	// TL4
	0x50 - ADDR_OFFS,	// KSAR1
	0x58 - ADDR_OFFS,	// KSAR2
	0x54 - ADDR_OFFS,	// KSAR3
	0x5c - ADDR_OFFS,	// KSAR4
	0x60 - ADDR_OFFS,	// AMDR1
	0x68 - ADDR_OFFS,	// AMDR2
	0x64 - ADDR_OFFS,	// AMDR3
	0x6c - ADDR_OFFS,	// AMDR4
	0x70 - ADDR_OFFS,	// SR1
	0x78 - ADDR_OFFS,	// SR2
	0x74 - ADDR_OFFS,	// SR3
	0x7c - ADDR_OFFS,	// SR4
	0x80 - ADDR_OFFS,	// SLRR1
	0x88 - ADDR_OFFS,	// SLRR2
	0x84 - ADDR_OFFS,	// SLRR3
	0x8c - ADDR_OFFS,	// SLRR4
	0x90 - ADDR_OFFS,	// SSGEG1
	0x98 - ADDR_OFFS,	// SSGEG2
	0x94 - ADDR_OFFS,	// SSGEG3
	0x9c - ADDR_OFFS,	// SSGEG4
	0xb0 - ADDR_OFFS	// FBAL
};

Tone* createTone(uint8_t* regs)
{
	auto tone = new Tone;

	auto& op1 = tone->op[0];
	op1.DT = regs[DTML1] >> 4;
	op1.ML = regs[DTML1] & 0x0f;
	op1.TL = regs[TL1];
	op1.KS = regs[KSAR1] >> 6;
	op1.AR = regs[KSAR1] & 0x1f;
	op1.AM = regs[AMDR1] >> 7;
	op1.DR = regs[AMDR1] & 0x1f;
	op1.SR = regs[SR1];
	op1.SL = regs[SLRR1] >> 4;
	op1.RR = regs[SLRR1] & 0x0f;
	op1.SSGEG = regs[SSGEG1];

	auto& op2 = tone->op[1];
	op2.DT = regs[DTML2] >> 4;
	op2.ML = regs[DTML2] & 0x0f;
	op2.TL = regs[TL2];
	op2.KS = regs[KSAR2] >> 6;
	op2.AR = regs[KSAR2] & 0x1f;
	op2.AM = regs[AMDR2] >> 7;
	op2.DR = regs[AMDR2] & 0x1f;
	op2.SR = regs[SR2];
	op2.SL = regs[SLRR2] >> 4;
	op2.RR = regs[SLRR2] & 0x0f;
	op2.SSGEG = regs[SSGEG2];

	auto& op3 = tone->op[2];
	op3.DT = regs[DTML3] >> 4;
	op3.ML = regs[DTML3] & 0x0f;
	op3.TL = regs[TL3];
	op3.KS = regs[KSAR3] >> 6;
	op3.AR = regs[KSAR3] & 0x1f;
	op3.AM = regs[AMDR3] >> 7;
	op3.DR = regs[AMDR3] & 0x1f;
	op3.SR = regs[SR3];
	op3.SL = regs[SLRR3] >> 4;
	op3.RR = regs[SLRR3] & 0x0f;
	op3.SSGEG = regs[SSGEG3];

	auto& op4 = tone->op[3];
	op4.DT = regs[DTML4] >> 4;
	op4.ML = regs[DTML4] & 0x0f;
	op4.TL = regs[TL4];
	op4.KS = regs[KSAR4] >> 6;
	op4.AR = regs[KSAR4] & 0x1f;
	op4.AM = regs[AMDR4] >> 7;
	op4.DR = regs[AMDR4] & 0x1f;
	op4.SR = regs[SR4];
	op4.SL = regs[SLRR4] >> 4;
	op4.RR = regs[SLRR4] & 0x0f;
	op4.SSGEG = regs[SSGEG4];

	tone->FB = regs[FBAL] >> 3;
	tone->AL = regs[FBAL] & 0x07;

	return tone;
}

inline constexpr void write(uint8_t address, uint8_t data, uint8_t* mem,
							std::bitset<6>& updated, size_t portB)
{
	constexpr uint32_t ADDR_OFFS = 0x30;
	address &= 0xff;
	if (0x2f < address && address < 0xa0) {
		size_t ch = portB * 3 + address % 4;
		updated.set(ch);
	}
	else if (0xaf < address && address < 0xb3){
		size_t ch = portB * 3 + address % 3;
		updated.set(ch);
	}
	else {
		return;
	}
	mem[address - ADDR_OFFS] = data;
}
}

RegisterRecorder::RegisterRecorder(uint32_t rate)
	: rate_(rate), count_(0)
{
	for (auto& mem : mem_) {
		std::fill_n(mem.memory[0], MEM_SIZE, 0);
		std::fill_n(mem.memory[1], MEM_SIZE, 0);
	}
}

void RegisterRecorder::writeA(uint8_t address, uint8_t data, Chip chip)
{
	write(address, data, mem_[chip].memory[0], mem_[chip].updated, 0);
}

void RegisterRecorder::writeB(uint8_t address, uint8_t data, Chip chip)
{
	write(address, data, mem_[chip].memory[1], mem_[chip].updated, 1);
}

void RegisterRecorder::capture()
{
	for (int chip = Chip::FIRST; chip < Chip::LAST; ++chip) {
		auto& mem = mem_[chip];
		if (mem.updated.none()) continue;

		size_t max = (chip == Chip::YM2203) ? 3 : 6;
		for (size_t ch = 0; ch < max; ++ch) {
			if (!mem.updated.test(ch)) continue;

			const uint8_t* pmem = mem.memory[ch / 3];
			uint8_t offs = ch % 3;
			uint8_t env[EnvelopeRegister::LAST];
			for (int i = EnvelopeRegister::FIRST; i < EnvelopeRegister::LAST; ++i) {
				env[i] = pmem[REG_ADDRS[i] + offs];
			}

			std::string key(env, env + sizeof(env));
			if (map_.count(key)) {
				UniqueTone& tone = tones_.at(map_[key]);
				tone.redundants.push_back({ ch, static_cast<double>(count_) / rate_ });
			}
			else {
				map_[key] = tones_.size();
				tones_.push_back({ createTone(env), ch, static_cast<double>(count_) / rate_, {} });
			}
		}

		mem.updated.reset();
	}
}

void RegisterRecorder::elapse(size_t count)
{
	capture();
	count_ += count;
}

std::vector<UniqueTone> RegisterRecorder::finish()
{
	return std::move(tones_);
}
