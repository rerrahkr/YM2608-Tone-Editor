#pragma once

#include "abstract_file_handler.hpp"

class S98Io final : public AbstractSongFileIo
{
public:
	S98Io();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
