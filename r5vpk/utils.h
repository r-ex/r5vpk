#pragma once
#include "pch.h"

namespace fs = std::filesystem;

namespace utils
{
	bool replace(std::string& str, const std::string& from, const std::string& to);

	std::string stripLocalization(std::string dirPath);

	bool endsWith(const std::string& mainStr, const std::string& toMatch);

	std::chrono::milliseconds GetTimeInMilliseconds();
};

