#pragma once

#include <cstddef>

namespace chip
{
	enum class Emu
	{
		Mame,
		Nuked,
		First = Mame,
		Last = Nuked,
	};

	const size_t SMPL_BUF_SIZE_ = 0x10000;

	enum Stereo : int
	{
		LEFT = 0,
		RIGHT = 1,
	};
}
