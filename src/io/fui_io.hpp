#pragma once

#include "abstract_file_handler.hpp"

class FuiIO final : public AbstractSingleToneIo
{
public:
	FuiIO();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};

