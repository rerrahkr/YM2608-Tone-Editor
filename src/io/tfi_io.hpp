#pragma once

#include "abstract_file_handler.hpp"

class TfiIo final : public AbstractSingleToneIo
{
public:
	TfiIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
