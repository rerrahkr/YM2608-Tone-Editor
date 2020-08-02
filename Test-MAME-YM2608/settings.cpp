#include "settings.hpp"
#include <QSettings>

const QString Settings::SETTINGS_PATH_ = "settings.ini";

Settings::Settings()
{
	QSettings settings(SETTINGS_PATH_, QSettings::IniFormat);

	rate_ = settings.value("rate", 110933).toInt();
	duration_ = settings.value("duration", 40).toInt();
}

Settings::~Settings()
{
	QSettings settings(SETTINGS_PATH_, QSettings::IniFormat);
	settings.setValue("rate", rate_);
	settings.setValue("duration", duration_);
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
