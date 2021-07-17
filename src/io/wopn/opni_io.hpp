#pragma once

#include "../abstract_file_handler.hpp"

class OpniIo final : public AbstractSingleToneIo
{
public:
	OpniIo();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
	const BinaryContainer save(const Tone& tone) const override;
};
