#pragma once

#include "abstract_file_handler.hpp"

class PmdIo final : public AbstractSongFileIo
{
public:
	PmdIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
