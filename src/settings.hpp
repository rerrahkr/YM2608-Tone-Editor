#pragma once

#include <vector>
#include <chips/chip_misc.h>
#include <QString>

class Settings
{
public:
    Settings();
    ~Settings();

	chip::Emu getEmulation() const;
	void setEmulation(chip::Emu emu);
	int getRate() const;
	void setRate(int rate);
	int getDuration() const;
	void setDuration(int duration);

private:
	static const QString SETTINGS_PATH_;
	chip::Emu emu_;
	int rate_;
	int duration_;
};
