#pragma once

#include <memory>

template <class T>
class Singleton final
{
public:
	static T& GetInstance()
	{
		static std::unique_ptr<T> instance = std::make_unique<T>();
		return *instance;
	}

protected:
	Singleton() {};

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator= (const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator= (Singleton&&) noexcept = delete;
};