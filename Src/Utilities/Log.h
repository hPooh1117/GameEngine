#pragma once
#include <string>

namespace Settings { struct Logger; }

#define VARIADIC_LOG_FN(FN_NAME)\
template<class... Args>\
void FN_NAME(const char* format, Args&&... args)\
{\
	char msg[LEN_MSG_BUFFER];\
	sprintf_s(msg, format, args...);\
	##FN_NAME(std::string(msg));\
}

#define VARIADIC_WLOG_FN(FN_WNAME)\
template<class... Args>\
void FN_WNAME(const wchar_t* format, Args&&... args)\
{\
    wchar_t msg[LEN_MSG_BUFFER];\
    swprintf_s(msg, format, args...);\
    ##FN_WNAME(std::wstring(msg));\
}

namespace Log
{
	constexpr size_t LEN_MSG_BUFFER = 1024;

	void Initialize(const Settings::Logger& settings);
	void Exit();

	void Info(const std::string& s);
	void Warning(const std::string& s);
	void Error(const std::string& s);

	void InfoW(const std::wstring& s);
	void WarningW(const std::wstring& s);
	void ErrorW(const std::wstring& s);

	VARIADIC_LOG_FN(Info);
	VARIADIC_LOG_FN(Warning);
	VARIADIC_LOG_FN(Error);

	VARIADIC_WLOG_FN(InfoW);
	VARIADIC_WLOG_FN(WarningW);
	VARIADIC_WLOG_FN(ErrorW);

}