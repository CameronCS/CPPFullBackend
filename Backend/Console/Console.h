#pragma once

#if WIN32

#include <Windows.h>

void InitWinConsole() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

#endif


void InitConsole() {
#if WIN32
	InitWinConsole();
#endif
}