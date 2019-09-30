#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "Memory.h"
#include "UnitTests.h"
#include "Tests.h"

#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

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
		size_t beginUsage = Memory::GetVirtualMemoryUsage();
		size_t finishUsage = 0;

		std::cout << "Running memory tests.." << std::endl;
		std::cout << "Current memory usage: " << Memory::FormatBytes( beginUsage ) << std::endl;

		std::vector< AST::IExpression* > allExpressions;

		for ( int i = 0; i < 10000 * 8; ++i )
		{
			auto lexed = Lexer::Parse( "{ a = 1 + 2 * 2 + -5; a = a || 8; } { a; b; a++ || b; }" );
			// AST::FreeTree( Parser::Parse( lexed ) );

			auto expressions = Parser::Parse( lexed );
			allExpressions.insert( allExpressions.end(), expressions.begin(), expressions.end() );
		}

		AST::FreeTree( allExpressions );
		allExpressions.clear();

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
	}

	std::getchar();
	return 0;
}
