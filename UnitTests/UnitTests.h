#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Exception.h"

struct TestResult_t
{
	std::string					m_TestDescription;
	bool						m_Passed;
	std::vector< std::string >	m_FailedConditions;
};

#define UTEST_S(x) #x
#define UTEST_S2(x) UTEST_S(x)
#define S__LINE__ UTEST_S2(__LINE__)

#define UTEST_BEGIN( name ) \
std::string testName = name; \
std::vector< TestResult_t > testResults; \
std::vector< std::string > exceptionLines; \
try {

#define UTEST_END() \
} catch ( const ParseException& parseException ) { \
	exceptionLines.push_back( "\t\033[31m[Parser Exception]\033[39m " + testResults[ testResults.size() - 1 ].m_TestDescription + ": " ); \
	auto errors = parseException.errors(); \
	auto locations = parseException.locations(); \
	for ( size_t i = 0; i < errors.size(); ++i ) \
	{ \
		exceptionLines.push_back( "\t\t " + errors[ i ] + " at line " + std::to_string( locations[ i ].m_LineNr ) \
			+ " col " + std::to_string( locations[ i ].m_ColNr ) ); \
	} \
} \
catch ( const std::exception& exception ) { \
	exceptionLines.push_back( "\t\033[31m[Exception]\033[39m " + std::string( exception.what() ) ); \
} \
catch ( const std::string& exception ) { \
	exceptionLines.push_back( "\t\033[31m[Exception]\033[39m " + exception ); \
} \
{ \
std::cout << "[Unit Tests] " << testName << std::endl; \
for ( TestResult_t result : testResults ) { \
	if ( result.m_Passed ) { \
		std::cout << "\t\033[32m[Passed]\033[39m " << result.m_TestDescription << std::endl; \
	} else { \
		std::cout << "\t\033[31m[Failed]\033[39m " << result.m_TestDescription << std::endl; \
		for ( std::string failedCondition : result.m_FailedConditions) \
			std::cout << "\t\t " << failedCondition << std::endl; \
	} \
} \
if ( exceptionLines.size() > 0 ) \
{ \
	std::cout << "\n\t\033[31mUnhandled exceptions occurred\033[39m" << std::endl; \
	for ( auto line : exceptionLines ) std::cout << line << std::endl; \
} \
}

#define UTEST_CASE( description ) \
testResults.push_back( { description, false } ); \
[&testResults]() -> void

#define UTEST_CASE_CLOSED() \
testResults[ testResults.size() - 1 ].m_Passed = true;

#define UTEST_ASSERT( condition ) \
if ( !(condition) ) { \
	testResults[ testResults.size() - 1 ].m_Passed = false; \
	testResults[ testResults.size() - 1 ].m_FailedConditions.push_back( "Assert: \033[33m" #condition "\033[39m at line " S__LINE__ ); \
	return; \
}

#define UTEST_THROW( condition ) \
{\
	bool passed = false; \
	try { \
		condition; \
	} catch ( ... ) { \
		passed = true; \
	} \
	if (!passed) { \
		testResults[ testResults.size() - 1 ].m_Passed = false; \
		testResults[ testResults.size() - 1 ].m_FailedConditions.push_back( "Code: \033[33m" #condition "\033[39m at line " S__LINE__ ); \
		return; \
	} \
}

#define UTEST_THROW_EXCEPTION( condition, exceptionType, exceptionTest ) \
{\
	bool passed = false; \
	try { \
		condition; \
	} catch ( exceptionType ) { \
		if (exceptionTest) {\
			passed = true; \
		} \
	} \
	if (!passed) { \
		testResults[ testResults.size() - 1 ].m_Passed = false; \
		testResults[ testResults.size() - 1 ].m_FailedConditions.push_back( "Exception condition: \033[33m" #exceptionTest "\033[39m at line " S__LINE__ ); \
		return; \
	} \
}

#define UTEST_NOT_THROW( condition ) \
{\
	try { \
		condition; \
	} catch ( ... ) { \
		testResults[ testResults.size() - 1 ].m_Passed = false; \
		testResults[ testResults.size() - 1 ].m_FailedConditions.push_back( "Code: \033[33m" #condition "\033[39m at line " S__LINE__ ); \
		return; \
	} \
}
