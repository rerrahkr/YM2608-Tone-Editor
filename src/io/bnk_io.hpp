#pragma once

#include "abstract_file_handler.hpp"

class BnkIo final : public AbstractToneBankIo
{
public:
	BnkIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
