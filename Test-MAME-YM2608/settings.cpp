#include "settings.hpp"
#include <QSettings>
#include <QVariant>
#include <QList>
//#include <QMetaType>

Q_DECLARE_METATYPE(QList<int>)

Settings::Settings()
{
//	qRegisterMetaTypeStreamOperators< QList<int> >("QList<int>");

    QSettings settings("settings.ini", QSettings::IniFormat);

	rate_ = settings.contains("rate") ? settings.value("rate").toInt() : 110933;
//	duration_ = settings.contains("duration") ? settings.value("duration").toInt() : 40;
//	if (settings.contains("input_order")) {
//		for (auto n : settings.value("input_order").value<QList<int>>())
//		inputOrder_.push_back(n);
//	}
//	else {
		inputOrder_ = {
			0, 1, 2,
			3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
			23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
			33, 34, 35, 36, 37, 38, 39, 40, 41, 42
		};
//	}
}

Settings::~Settings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    settings.setValue("rate", QVariant::fromValue(rate_));
    settings.setValue("duration", QVariant::fromValue(duration_));
//	QList<int> l;
//	for (auto n : inputOrder_)
//		l.append(n);
//	settings.setValue("input_order", QVariant::fromValue(l));
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

void Settings::setinputOrder(std::vector<int> order)
{
	inputOrder_ = order;
}

std::vector<int> Settings::getInputOrder() const
{
	return inputOrder_;
}
