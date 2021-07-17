#pragma once

#include "abstract_file_handler.hpp"

class GybIo final : public AbstractToneBankIo
{
public:
	GybIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
