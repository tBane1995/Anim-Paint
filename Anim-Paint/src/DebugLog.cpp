#include "DebugLog.hpp"

void DebugLog(std::wstring message) {
	std::wcout << L"[Log] " << message << L"\n";
}

void DebugError(std::wstring  message) {
	std::wcout << L"[Error] " << message << L"\n";
}