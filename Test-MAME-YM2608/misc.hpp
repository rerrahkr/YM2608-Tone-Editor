#pragma once

#include <cmath>

constexpr float F_PI = 3.14159265f;
constexpr size_t SMPL_BUFSIZE = 0x10000;


inline float sinc(float x)
{
	return ((!x) ? 1.0f : (std::sin(x) / x));
}