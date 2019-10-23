#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "Memory.h"

#include "UnitTests.h"
#include "Tests.h"

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

bool HasArg( const std::string& argument, int argc, const char** argv )
{
	// Iterate through all arguments -- the first one is the program path
	for ( int i = 1; i < argc; ++i )
	{
		if ( argument == argv[ i ] )
			return true;
	}

	return false;
}

int main( int argc, const char** argv )
{
#if defined(_WIN32) || defined(_WIN64)
	// Enable ANSI colors for windows console
	HANDLE hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD dwMode = 0;
	GetConsoleMode( hStdOut, &dwMode );
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode( hStdOut, dwMode );
#endif

	if ( HasArg( "-memtest", argc, argv ) )
	{
		std::cout << "Running memory tests.." << std::endl;
		size_t beginUsage = Memory::GetVirtualMemoryUsage();
		size_t finishUsage = 0;

		std::cout << "Current memory usage: " << Memory::FormatBytes( beginUsage ) << std::endl;

		for ( int i = 0; i < 10000 * 12; ++i )
		{
			auto lexed = Lexer::Parse( "{ a = 1 + 2 * 2 + -5; a = a || 8; } { a; b; a++ || b; }" );
			AST::FreeTree( Parser::Parse( lexed ) );
		}

		finishUsage = Memory::GetVirtualMemoryUsage();

		std::cout << "Memory usage when finished: " << Memory::FormatBytes( finishUsage ) << std::endl;
		std::cout << "Difference: " << Memory::FormatBytes( finishUsage - beginUsage ) << std::endl;
	}
	else
	{
		// Call tests
		RunLexerTests();
		RunValueTests();
		RunParserTests();
		RunRuntimeTests();
	}

#ifdef _WIN32
	std::getchar();
#endif
	return 0;
}
