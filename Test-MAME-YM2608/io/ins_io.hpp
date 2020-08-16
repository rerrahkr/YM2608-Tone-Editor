#pragma once

#include "file_io.hpp"

class InsIo final : public AbstractSingleToneIo
{
public:
	InsIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
