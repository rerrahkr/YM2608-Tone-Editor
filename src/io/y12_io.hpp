#pragma once

#include "abstract_file_handler.hpp"

class Y12Io final : public AbstractSingleToneIo
{
public:
	Y12Io();
	[[nodiscard]] Tone* load(const BinaryContainer& container) const override;
};
