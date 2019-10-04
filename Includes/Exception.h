#pragma once
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
protected:
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

	ParseException operator=( const ParseException& other )
	{
		m_Errors.clear();
		m_Locations.clear();

		m_What = other.m_What;

		std::copy( other.m_Errors.begin(), other.m_Errors.end(), std::back_inserter( m_Errors ) );
		std::copy( other.m_Locations.begin(), other.m_Locations.end(), std::back_inserter( m_Locations ) );

		return *this;
	}
private:
	std::vector< Grammar::SymbolLoc_t >		m_Locations;
	std::vector< std::string >				m_Errors;
};
