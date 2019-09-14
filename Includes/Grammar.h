#pragma once
#include <string>
#include <map>

namespace Grammar
{
	enum LeftBindingPower
	{
		LBP_NAME			= 0,
		LBP_FEATURE			= 0,
		LBP_SEMICOLON		= 0,
		LBP_LOGIC_NOT		= 0,
		LBP_LOGIC			= 10,
		LBP_ASSIGN			= 10,
		LBP_EQUALITY		= 20,
		LBP_COLON			= 30,
		LBP_ARITHMETIC_1	= 40,
		LBP_ARITHMETIC_2	= 50,
		LBP_ARITHMETIC_3	= 60,
		LBP_INCDEC			= 70,
		LBP_OPENBRACKET		= 80,
	};

	enum SymbolGroup
	{
		SG_ARITHMETIC		= 0,
		SG_ASSIGNMENT,
		SG_LOGICAL,
		SG_SEPARATOR,
		SG_EQUALITY,
		SG_INCDEC,
		SG_BRACKET,
		SG_FEATURE,
		SG_NAME,
		SG_OTHER,
	};

	struct SymbolInfo_t
	{
		std::string			m_Token;
		int					m_LBP;
		SymbolGroup			m_Group;
	};

	enum Symbol
	{
		S_ASSIGN = 0,
		S_ASSIGN_ADD,
		S_ASSIGN_SUB,
		S_ASSIGN_MUL,
		S_ASSIGN_DIV,
		S_LOGIC_AND,
		S_LOGIC_OR,
		S_LOGIC_NOT,
		S_EQUALS,
		S_EQUALS_NOT,
		S_MORE_OR_EQUALS,
		S_LESS_OR_EQUALS,
		S_MORETHAN,
		S_LESSTHAN,
		S_INCREMENT,
		S_DECREMENT,
		S_BRACKET_OPEN,
		S_BRACKET_CLOSE,
		S_SBRACKET_OPEN,
		S_SBRACKET_CLOSE,
		S_PARENT_OPEN,
		S_PARENT_CLOSE,
		S_ADD,
		S_SUB,
		S_MUL,
		S_DIV,
		S_POW,
		S_SEMICOLON,
		S_COLON,
		S_IF,
		S_WHILE,
		S_VAR,
		S_FUNC,
		S_RETURN,
		S_BREAK,
		S_TRUE,
		S_FALSE,
	};

	static const std::map<Symbol, SymbolInfo_t> LanguageSymbols = {
		// Assignments
		{ S_ASSIGN,			{ "=",			LBP_ASSIGN,			SG_ASSIGNMENT } },
		{ S_ASSIGN_ADD,		{ "+=",			LBP_ASSIGN,			SG_ASSIGNMENT } },
		{ S_ASSIGN_SUB,		{ "-=",			LBP_ASSIGN,			SG_ASSIGNMENT } },
		{ S_ASSIGN_MUL,		{ "*=",			LBP_ASSIGN,			SG_ASSIGNMENT } },
		{ S_ASSIGN_DIV,		{ "/=",			LBP_ASSIGN,			SG_ASSIGNMENT } },

		// Logical operators
		{ S_LOGIC_AND,		{ "&&",			LBP_LOGIC,			SG_LOGICAL } },
		{ S_LOGIC_OR,		{ "||",			LBP_LOGIC,			SG_LOGICAL } },
		{ S_LOGIC_NOT,		{ "!",			LBP_LOGIC_NOT,		SG_LOGICAL } },

		// Equality
		{ S_EQUALS,			{ "==",			LBP_EQUALITY,		SG_EQUALITY } },
		{ S_EQUALS_NOT,		{ "!=",			LBP_EQUALITY,		SG_EQUALITY } },
		{ S_MORE_OR_EQUALS,	{ ">=",			LBP_EQUALITY,		SG_EQUALITY } },
		{ S_LESS_OR_EQUALS,	{ "<=",			LBP_EQUALITY,		SG_EQUALITY } },
		{ S_MORETHAN,		{ ">",			LBP_EQUALITY,		SG_EQUALITY } },
		{ S_LESSTHAN,		{ "<",			LBP_EQUALITY,		SG_EQUALITY } },

		// Increment/Decrement
		{ S_INCREMENT,		{ "++",			LBP_INCDEC,			SG_INCDEC } },
		{ S_LESSTHAN,		{ "--",			LBP_INCDEC,			SG_INCDEC } },

		// Brackets
		{ S_BRACKET_OPEN,	{ "{",			0,					SG_BRACKET } },
		{ S_BRACKET_CLOSE,	{ "}",			0,					SG_BRACKET } },
		{ S_SBRACKET_OPEN,	{ "[",			LBP_OPENBRACKET,	SG_BRACKET } },
		{ S_SBRACKET_CLOSE,	{ "]",			0,					SG_BRACKET } },
		{ S_PARENT_OPEN,	{ "(",			LBP_OPENBRACKET,	SG_BRACKET } },
		{ S_PARENT_CLOSE,	{ ")",			0,					SG_BRACKET } },

		// Arithmetics
		{ S_ADD,			{ "+",			LBP_ARITHMETIC_1,	SG_ARITHMETIC } },
		{ S_SUB,			{ "-",			LBP_ARITHMETIC_1,	SG_ARITHMETIC } },
		{ S_MUL,			{ "*",			LBP_ARITHMETIC_2,	SG_ARITHMETIC } },
		{ S_DIV,			{ "/",			LBP_ARITHMETIC_2,	SG_ARITHMETIC } },
		{ S_POW,			{ "**",			LBP_ARITHMETIC_3,	SG_ARITHMETIC } },

		// Separators
		{ S_SEMICOLON,		{ ";",			LBP_SEMICOLON,		SG_SEPARATOR } },
		{ S_COLON,			{ ",",			LBP_COLON,			SG_SEPARATOR } },

		// Language Features
		{ S_IF,				{ "if",			LBP_FEATURE,		SG_FEATURE } },
		{ S_WHILE,			{ "while",		LBP_FEATURE,		SG_FEATURE } },
		{ S_VAR,			{ "var",		LBP_FEATURE,		SG_FEATURE } },
		{ S_FUNC,			{ "function",	LBP_FEATURE,		SG_FEATURE } },
		{ S_RETURN,			{ "return",		LBP_FEATURE,		SG_FEATURE } },
		{ S_BREAK,			{ "break",		LBP_FEATURE,		SG_FEATURE } },
		{ S_TRUE,			{ "true",		LBP_FEATURE,		SG_FEATURE } },
		{ S_FALSE,			{ "false",		LBP_FEATURE,		SG_FEATURE } },
	};
}
