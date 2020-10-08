#include "Log.h"

#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <io.h>

#include <Windows.h>

#include "./Application/Application.h"
#include "./Engine/Settings.h"

namespace Log
{
	using namespace std;
	static const WORD MAX_CONSOLE_LINES = 500;

	void InitFile()
	{

	}

	void InitConsole()
	{
		// src: https://stackoverflow.com/a/46050762/2034041
		//void RedirectIOToConsole() 
		{
			//Create a console for this application
			AllocConsole();

			// Get STDOUT handle
			HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
			std::FILE* COutputHandle = _fdopen(SystemOutput, "w");

			// Get STDERR handle
			HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
			int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
			std::FILE* CErrorHandle = _fdopen(SystemError, "w");

			// Get STDIN handle
			HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
			int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
			std::FILE* CInputHandle = _fdopen(SystemInput, "r");

			//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
			std::ios::sync_with_stdio(true);

			// Redirect the CRT standard input, output, and error handles to the console
			freopen_s(&CInputHandle, "CONIN$", "r", stdin);
			freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
			freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

			//Clear the error state for each of the C++ standard stream objects. We need to do this, as
			//attempts to access the standard streams before they refer to a valid target will cause the
			//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
			//to always occur during startup regardless of whether anything has been read from or written to
			//the console or not.
			std::wcout.clear();
			std::cout.clear();
			std::wcerr.clear();
			std::cerr.clear();
			std::wcin.clear();
			std::cin.clear();
		}
	}



	void Initialize(const Settings::Logger& settings)
	{
		if (settings.bConsole) InitConsole();
		if (settings.bFile)    InitFile();
	}



	void Exit()
	{
		std::string msg = "[LOG]: Exit...\n";

		std::cout << msg;
		OutputDebugStringA(msg.c_str());

	}

	void Info(const std::string& s)
	{
		std::string info = "[INFO]: " + s + "\n";
		OutputDebugStringA(info.c_str());              // VS output

		std::cout << info;                            // Console Output
	}

	void Warning(const std::string& s)
	{
		std::string warn = "[WARNING]: " + s + "\n";
		OutputDebugStringA(warn.c_str());              // VS output

		std::cout << warn;				              // Console Output
	}

	void Error(const std::string& s)
	{
		std::string error = "[ERROR]: " + s + "\n";
		OutputDebugStringA(error.c_str());             // VS output

		std::cout << error;				              // Console Output
	}

	void InfoW(const std::wstring& s)
	{
		std::wstring info = L"[INFO]: " + s + L"\n";
		OutputDebugString(info.c_str());

		std::wcout << info;
	}

	void WarningW(const std::wstring& s)
	{
		std::wstring warn = L"[WARNING]: " + s + L"\n";
		OutputDebugString(warn.c_str());

		std::wcout << warn;
	}

	void ErrorW(const std::wstring& s)
	{
		std::wstring error = L"[ERROR]: " + s + L"\n";
		OutputDebugString(error.c_str());

		std::wcout << error;
	}

} // namespace Log
