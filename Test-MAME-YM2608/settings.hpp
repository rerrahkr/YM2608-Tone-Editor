#pragma once

#include "types.h"

class Settings
{
public:
    Settings();
    ~Settings();

    uint32 getRate() const;
    void setRate(uint32 rate);
    uint32 getDuration() const;
    void setDuration(uint32 duration);

private:
    uint32 rate_;
    uint32 duration_;
};
