#include "settings.hpp"
#include <QSettings>
#include <QVariant>

Settings::Settings() :
    rate_(110933),
    duration_(40)
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    if (settings.contains("rate")) {
		rate_ = settings.value("rate").toInt();
        if (settings.contains("duration")) {
			duration_ = settings.value("duration").toInt();
        }
    }
}

Settings::~Settings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setValue("rate", QVariant::fromValue(rate_));
    settings.setValue("duration", QVariant::fromValue(duration_));
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
