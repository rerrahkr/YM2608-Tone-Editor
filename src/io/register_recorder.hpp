#pragma once

#include <vector>
#include <unordered_map>
#include <bitset>
#include <string>
#include <cstdint>

struct Tone;

struct UniqueTone
{
  Tone* tone;
  size_t channel;
  double time;
  struct RedundantTone
  {
	  size_t channel;
	  double time;
  };
  std::vector<RedundantTone> redundants;
};

class RegisterRecorder
{
public:
	explicit RegisterRecorder(uint32_t rate);

	enum Chip { YM2203, YM2608, YM2610, YM2612, LAST, FIRST = YM2203 };

	void writeA(uint8_t address, uint8_t data, Chip chip = YM2608);
	void writeB(uint8_t address, uint8_t data, Chip chip = YM2608);
	void capture();
	void elapse(size_t count);
	std::vector<UniqueTone> finish();

private:
	uint32_t rate_;
	std::vector<UniqueTone> tones_;
	size_t count_;

	static constexpr size_t MEM_SIZE = 0x84;
	struct Memory
	{
		uint8_t memory[2][MEM_SIZE];
		std::bitset<6> updated;
	} mem_[LAST];
	std::unordered_map<std::string, size_t> map_;
};
