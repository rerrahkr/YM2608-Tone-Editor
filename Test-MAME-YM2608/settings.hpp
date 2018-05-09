#pragma once

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
	int rate_;
	int duration_;
};
