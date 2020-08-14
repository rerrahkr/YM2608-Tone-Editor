#pragma once

#include "file_io.hpp"

class FfIo final : public AbstractToneBankIo
{
public:
	FfIo();
	std::vector<TonePtr> load(const BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
