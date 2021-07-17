#pragma once

#include "abstract_file_handler.hpp"

class OriginalToneIo final : public AbstractSingleToneIo
{
public:
	OriginalToneIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
