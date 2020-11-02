#include "settings.hpp"
#include <QSettings>

const QString Settings::SETTINGS_PATH_ = "settings.ini";

Settings::Settings()
{
	QSettings settings(SETTINGS_PATH_, QSettings::IniFormat);

	emu_ = static_cast<chip::Emu>(settings.value("emulation", static_cast<int>(chip::Emu::Nuked)).toInt());
	rate_ = settings.value("rate", 110933).toInt();
	duration_ = settings.value("duration", 40).toInt();
}

Settings::~Settings()
{
	QSettings settings(SETTINGS_PATH_, QSettings::IniFormat);
	settings.setValue("emulation", static_cast<int>(emu_));
	settings.setValue("rate", rate_);
	settings.setValue("duration", duration_);
}

chip::Emu Settings::getEmulation() const
{
	return emu_;
}

void Settings::setEmulation(chip::Emu emu)
{
	emu_ = emu;
}

int Settings::getRate() const
{
    return rate_;
}

void Settings::setRate(int rate)
{
    rate_ = rate;
}

int Settings::getDuration() const
{
    return duration_;
}

void Settings::setDuration(int duration)
{
    duration_ = duration;
}
