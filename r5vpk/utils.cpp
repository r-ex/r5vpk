#include "pch.h"

using namespace std::chrono;


namespace utils
{
	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	std::string stripLocalization(std::string dirPath) {
		fs::path p(dirPath);
		std::string fileName = p.filename().u8string();
		replace(fileName, "english", "");
		return fileName;
	}

	bool endsWith(const std::string& mainStr, const std::string& toMatch)
	{
		if (mainStr.size() >= toMatch.size() &&
			mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
			return true;
		else
			return false;
	}

	std::chrono::milliseconds GetTimeInMilliseconds()
	{
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}
}
