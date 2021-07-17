#pragma once

#include "abstract_file_handler.hpp"

class FfIo final : public AbstractToneBankIo
{
public:
	FfIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
