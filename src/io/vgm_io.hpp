#pragma once

#include "abstract_file_handler.hpp"

class VgmIo final : public AbstractSongFileIo
{
public:
    VgmIo();
    std::vector<TonePtr> load(BinaryContainer& container) const override;
};
