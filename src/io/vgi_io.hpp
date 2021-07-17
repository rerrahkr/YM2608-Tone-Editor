#pragma once

#include "abstract_file_handler.hpp"

class VgiIo final : public AbstractSingleToneIo
{
public:
	VgiIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
