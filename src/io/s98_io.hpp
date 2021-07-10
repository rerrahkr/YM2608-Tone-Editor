#pragma once

#include "file_io.hpp"

class S98Io final : public AbstractSongFileIo
{
public:
	S98Io();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
