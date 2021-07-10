#pragma once

#include "file_io.hpp"

class VgmIo final : public AbstractSongFileIo
{
public:
    VgmIo();
    std::vector<TonePtr> load(BinaryContainer& container) const override;
};
