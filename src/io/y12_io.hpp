#pragma once

#include "file_io.hpp"

class Y12Io final : public AbstractSingleToneIo
{
public:
	Y12Io();
	Tone* load(const BinaryContainer& container) const override;
};
