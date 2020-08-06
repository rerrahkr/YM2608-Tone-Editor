#pragma once

#include "file_io.hpp"

class BtiIo final : public AbstractSingleToneIo
{
public:
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
