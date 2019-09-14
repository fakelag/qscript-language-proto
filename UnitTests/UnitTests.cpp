#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "UnitTests.h"
#include "Tests.h"

int main()
{
#if defined(_WIN32) || defined(_WIN64)
	// Enable ANSI colors for windows console
	HANDLE hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD dwMode = 0;
	GetConsoleMode( hStdOut, &dwMode );
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode( hStdOut, dwMode );
#endif

	// Call tests
	RunLexerTests();
	return 0;
}
