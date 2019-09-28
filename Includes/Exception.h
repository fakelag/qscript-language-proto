#include <exception>
#include <string>
#include <vector>
#include "Grammar.h"

#ifndef _NOEXCEPT
#define _NOEXCEPT noexcept
#endif

class Exception : public std::exception
{
public:
	Exception( const std::string& what )
	{
		m_What = what;
	}

	const char* what() const _NOEXCEPT { return m_What.c_str(); }
private:
	std::string m_What;
};

class ParseException : public Exception
{
public:
	ParseException() : Exception( "Parsing exception" )
	{
	}

	ParseException( const std::vector<Grammar::SymbolLoc_t> locations, const std::vector<std::string> errors )
		: Exception( "Parsing exception" )
	{
		m_Locations = locations;
		m_Errors = errors;
	}

	ParseException( const Grammar::SymbolLoc_t& location, const std::string& error )
		: Exception( "Parsing exception" )
	{
		AddError( location, error );
	}

	void AddError( const Grammar::SymbolLoc_t& location, const std::string& error )
	{
		m_Locations.push_back( location );
		m_Errors.push_back( error );
	}

	const std::vector< Grammar::SymbolLoc_t >& locations() const { return m_Locations; }
	const std::vector< std::string >& errors() const { return m_Errors; }
private:
	std::vector< Grammar::SymbolLoc_t >		m_Locations;
	std::vector< std::string >				m_Errors;
};
