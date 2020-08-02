#pragma once

#include <vector>
#include <QString>

class Settings
{
public:
    Settings();
    ~Settings();

	int getRate() const;
	void setRate(int rate);
	int getDuration() const;
	void setDuration(int duration);

private:
	static const QString SETTINGS_PATH_;
	int rate_;
	int duration_;
};
