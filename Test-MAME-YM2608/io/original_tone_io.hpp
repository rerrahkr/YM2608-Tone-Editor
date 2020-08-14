#pragma once

#include "file_io.hpp"

class OriginalToneIo final : public AbstractSingleToneIo
{
public:
	OriginalToneIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
