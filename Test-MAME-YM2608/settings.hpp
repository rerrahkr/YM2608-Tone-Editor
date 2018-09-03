#pragma once

#include <vector>

class Settings
{
public:
    Settings();
    ~Settings();

	int getRate() const;
	void setRate(int rate);
	int getDuration() const;
	void setDuration(int duration);
	void setinputOrder(std::vector<int> order);
	std::vector<int> getInputOrder() const;

private:
	int rate_;
	int duration_;
	std::vector<int> inputOrder_;
};
