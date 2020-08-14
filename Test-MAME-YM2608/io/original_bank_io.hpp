#pragma once

#include "file_io.hpp"

class OriginalBankIo final : public AbstractToneBankIo
{
public:
	OriginalBankIo();
	std::vector<TonePtr> load(const BinaryContainer& container) const override;
	const BinaryContainer save(const std::vector<TonePtr>& bank) const override;
};
