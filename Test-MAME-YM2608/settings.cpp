#include "settings.hpp"
#include <QSettings>
#include <QVariant>

Settings::Settings() :
    rate_(110933),
    duration_(40)
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    if (settings.contains("rate")) {
        rate_ = settings.value("rate").toUInt();
        if (settings.contains("duration")) {
            duration_ = settings.value("duration").toUInt();
        }
    }
}

Settings::~Settings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setValue("rate", QVariant::fromValue(rate_));
    settings.setValue("duration", QVariant::fromValue(duration_));
}

uint32 Settings::getRate() const
{
    return rate_;
}

void Settings::setRate(uint32 rate)
{
    rate_ = rate;
}

uint32 Settings::getDuration() const
{
    return duration_;
}

void Settings::setDuration(uint32 duration)
{
    duration_ = duration;
}
