#pragma once

#include "file_io.hpp"

class BtbIo final : public AbstractToneBankIo
{
public:
	BtbIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
