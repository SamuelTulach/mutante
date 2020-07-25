#pragma once

namespace Utils
{
	PVOID GetModuleBase(const char* moduleName);
	bool CheckMask(const char* base, const char* pattern, const char* mask);
	PVOID FindPattern(PVOID base, int length, const char* pattern, const char* mask);
	PVOID FindPatternImage(PVOID base, const char* pattern, const char* mask);
	void RandomText(char* text, const int length);
}
