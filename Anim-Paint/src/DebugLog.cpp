#include "DebugLog.hpp"

void DebugLog(std::wstring message) {
	std::wcout << L"[Log] ";
	std::wcout << message << L"\n";
}

void DebugError(std::wstring  message) {
	std::wcout << L"[Error] ";
	std::wcout << message << L"\n";
}