#pragma once

#include "abstract_file_handler.hpp"

class InsIo final : public AbstractSingleToneIo
{
public:
	InsIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
