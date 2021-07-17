#pragma once

#include "abstract_file_handler.hpp"

class DmpIo final : public AbstractSingleToneIo
{
public:
	DmpIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
