#pragma once

#include <vector>
#include <functional>
#include <algorithm>

template<typename T>
struct Delegate
{
	std::vector<T*> mFuncs;

	template<class T> void operator+=(T* ptr)
	{
		mFuncs.push_back(ptr);
	}

	template<class T> void operator-=(T* ptr)
	{
		std::remove(mFuncs.begin(), mFuncs.end(), ptr);
	}

	void invoke(const std::function<void(T*)>& func)
	{
		for (auto& f : mFuncs) 
			func(f);
	}
};
