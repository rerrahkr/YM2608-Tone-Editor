#pragma once

#include "abstract_file_handler.hpp"

class BtiIo final : public AbstractSingleToneIo
{
public:
	BtiIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
