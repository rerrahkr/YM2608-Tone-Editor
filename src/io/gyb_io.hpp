#pragma once

#include "file_io.hpp"

class GybIo final : public AbstractToneBankIo
{
public:
	GybIo();
	std::vector<TonePtr> load(BinaryContainer& container) const override;
};
