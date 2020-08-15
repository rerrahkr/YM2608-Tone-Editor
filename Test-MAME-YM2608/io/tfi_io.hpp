#pragma once

#include "file_io.hpp"

class TfiIo final : public AbstractSingleToneIo
{
public:
	TfiIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
