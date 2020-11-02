#pragma once

#include "../file_io.hpp"

class OpniIo final : public AbstractSingleToneIo
{
public:
	OpniIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
