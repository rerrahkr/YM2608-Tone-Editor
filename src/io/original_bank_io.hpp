#pragma once

#include "abstract_file_handler.hpp"

class OriginalBankIo final : public AbstractToneBankIo
{
public:
	OriginalBankIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
