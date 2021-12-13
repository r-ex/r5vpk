#pragma once
#include "pch.h"

class Logger
{
public:
	void DisplayStartupMessage();

	void Info(const char* fmt, ...);
	void Info(std::string msg);

	void Error(const char* fmt, ...);
	void Error(std::string msg);

	void Warning(const char* fmt, ...);

	// debug logs are compiled out on release build because they either cause low performance
	// or serve no purpose for the end-user (i.e. references to known issues in code)
	void Debug(const char* fmt, ...);
	void Debug(std::string msg);
};

extern Logger g_Logger;
