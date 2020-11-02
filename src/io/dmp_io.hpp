#pragma once

#include "file_io.hpp"

class DmpIo final : public AbstractSingleToneIo
{
public:
	DmpIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
