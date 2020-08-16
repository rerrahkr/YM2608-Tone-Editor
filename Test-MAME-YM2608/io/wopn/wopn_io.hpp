#pragma once

#include "../file_io.hpp"

class WopnIo final : public AbstractToneBankIo
{
public:
	WopnIo();
	std::vector<TonePtr> load(const BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
