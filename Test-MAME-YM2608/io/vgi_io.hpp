#pragma once
#include "file_io.hpp"

class VgiIo final : public AbstractSingleToneIo
{
public:
	VgiIo();
	Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
