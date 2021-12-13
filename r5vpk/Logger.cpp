#include "pch.h"

Logger g_Logger;

// this is totally unnecessary. oh well
void Logger::DisplayStartupMessage()
{
	Info(" _____  _______      _______  _  __\n");
	Info("|  __ \\| ____\\ \\    / /  __ \\| |/ /\n");
	Info("| |__) | |__  \\ \\  / /| |__) | ' / \n");
	Info("|  _  /|___ \\  \\ \\/ / |  ___/|  <  \n");
	Info("| | \\ \\ ___) |  \\  /  | |    | . \\ \n");
	Info("|_|  \\_\\____/    \\/   |_|    |_|\\_\\\n");
	Info("\n");
}

void Logger::Info(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::string sFmt(fmt);

	sFmt = "[r5vpk]:[I] " + sFmt;

	vprintf(sFmt.c_str(), args);

	va_end(args);
}

void Logger::Info(std::string msg)
{
	msg = "[r5vpk]:[I] " + msg + "\n";
	printf(msg.c_str());
}

void Logger::Error(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::string sFmt(fmt);

	sFmt = "[r5vpk]:[E] " + sFmt;

	vprintf(sFmt.c_str(), args);

	va_end(args);
}

void Logger::Error(std::string msg)
{
	msg = "[r5vpk]:[E] " + msg + "\n";
	printf(msg.c_str());
}

void Logger::Warning(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	std::string sFmt(fmt);

	sFmt = "[r5vpk]:[W] " + sFmt;

	vprintf(sFmt.c_str(), args);

	va_end(args);
}

void Logger::Debug(const char* fmt, ...)
{
#if _DEBUG
	va_list args;
	va_start(args, fmt);

	std::string sFmt(fmt);

	sFmt = "[r5vpk]:[D] " + sFmt;

	vprintf(sFmt.c_str(), args);

	va_end(args);
#endif
}

void Logger::Debug(std::string msg)
{
#if _DEBUG
	msg = "[r5vpk]:[D] " + msg + "\n";
	printf(msg.c_str());
#endif
}