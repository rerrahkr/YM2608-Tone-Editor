#pragma once

#include "file_io.hpp"

class BnkIo final : public AbstractToneBankIo
{
public:
	BnkIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
