#pragma once

#include "abstract_file_handler.hpp"

class FmpIo final : public AbstractSongFileIo
{
public:
	FmpIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
