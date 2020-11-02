#pragma once

#include <string>
#include <QString>

inline QString utf8ToQString(const std::string& str)
{
	return QString::fromUtf8(str.c_str(), static_cast<int>(str.length()));
}
