#pragma once

#include <string>
#include <map>

namespace Grammar
{
	enum LeftBindingPower
	{
		LBP_IMMEDIATE		= -1,
		LBP_NONE			= 0,
		LBP_COMMA			= 10,
		LBP_ASSIGN			= 20,
		LBP_LOGIC_NOT		= 30,
		LBP_LOGIC			= 40,
		LBP_EQUALITY		= 50,
		LBP_ARITHMETIC_1	= 60,
		LBP_ARITHMETIC_2	= 70,
		LBP_ARITHMETIC_3	= 80,
		LBP_INCDEC			= 90,
		LBP_OPENBRACKET		= 100,
		LBP_DOT				= 110,
		LBP_TYPE			= 120,
	};

	struct SymbolLoc_t
	{
		int					m_LineNr;
		int					m_ColNr;
		std::string 		m_SrcToken;
	};

	struct SymbolInfo_t
	{
		std::string			m_Token;
		int					m_LBP;
		bool				m_IsWord;
	};

	enum Symbol
	{
		// Physical symbols
		S_NAME,
		S_STRCNST,
		S_INTCNST,
		S_DBLCNST,
		S_ASSIGN,
		S_ASSIGN_ADD,
		S_ASSIGN_SUB,
		S_ASSIGN_MUL,
		S_ASSIGN_DIV,
		S_ASSIGN_MOD,
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
		S_MOD,
		S_SEMICOLON,
		S_COMMA,
		S_COLON,
		S_DOT,
		S_IF,
		S_WHILE,
		S_FOR,
		S_VAR,
		S_FUNC,
		S_RETURN,
		S_TRUE,
		S_FALSE,
		S_NULL,
		S_BREAK,
		S_DEFER,

		// Types
		S_TYPE,
		S_TYPETEXT,
		S_TYPESTRING,
		S_TYPEINTEGER,
		S_TYPEDECIMAL,
		S_TYPEBOOLEAN,
		S_TYPEARRAY,
		S_TYPEOBJECT,
		S_TYPENONE,

		// Pseudo-symbols
		S_SCOPE,
		S_CALL,
		S_FUNCDEF,
		S_FUNCBODY,

		S_CLASS,
		S_CLASSBODY,

		// List symbols are used for any kind of comma separated sequences of expressions.
		// e.g Function calls, print( "foo: ", foo )
		S_LIST,

		// Array symbols are used strictly for initializing new arrays
		// e.g [1, 2, 3]
		// Although the array literal could contain a list, it is removed
		// by the parser, and the body of the list is appended directly to
		// the array expression
		S_ARRAY,
		S_ACCESS,
	};

	static const std::map<Symbol, SymbolInfo_t> LanguageSymbols = {
		// Assignments
		{ S_ASSIGN,			{ "=",			LBP_ASSIGN,			false } },
		{ S_ASSIGN_ADD,		{ "+=",			LBP_ASSIGN,			false } },
		{ S_ASSIGN_SUB,		{ "-=",			LBP_ASSIGN,			false } },
		{ S_ASSIGN_MUL,		{ "*=",			LBP_ASSIGN,			false } },
		{ S_ASSIGN_DIV,		{ "/=",			LBP_ASSIGN,			false } },
		{ S_ASSIGN_MOD,		{ "%=",			LBP_ASSIGN,			false } },

		// Logical operators
		{ S_LOGIC_AND,		{ "&&",			LBP_LOGIC,			false } },
		{ S_LOGIC_OR,		{ "||",			LBP_LOGIC,			false } },
		{ S_LOGIC_NOT,		{ "!",			LBP_LOGIC_NOT,		false } },

		// Equality
		{ S_EQUALS,			{ "==",			LBP_EQUALITY,		false } },
		{ S_EQUALS_NOT,		{ "!=",			LBP_EQUALITY,		false } },
		{ S_MORE_OR_EQUALS,	{ ">=",			LBP_EQUALITY,		false } },
		{ S_LESS_OR_EQUALS,	{ "<=",			LBP_EQUALITY,		false } },
		{ S_MORETHAN,		{ ">",			LBP_EQUALITY,		false } },
		{ S_LESSTHAN,		{ "<",			LBP_EQUALITY,		false } },

		// Increment/Decrement
		{ S_INCREMENT,		{ "++",			LBP_INCDEC,			false } },
		{ S_DECREMENT,		{ "--",			LBP_INCDEC,			false } },

		// Brackets
		{ S_BRACKET_OPEN,	{ "{",			LBP_NONE,			false } },
		{ S_BRACKET_CLOSE,	{ "}",			LBP_NONE,			false } },
		{ S_SBRACKET_OPEN,	{ "[",			LBP_OPENBRACKET,	false } },
		{ S_SBRACKET_CLOSE,	{ "]",			LBP_NONE,			false } },
		{ S_PARENT_OPEN,	{ "(",			LBP_OPENBRACKET,	false } },
		{ S_PARENT_CLOSE,	{ ")",			LBP_NONE,			false } },

		// Arithmetics
		{ S_ADD,			{ "+",			LBP_ARITHMETIC_1,	false } },
		{ S_SUB,			{ "-",			LBP_ARITHMETIC_1,	false } },
		{ S_MUL,			{ "*",			LBP_ARITHMETIC_2,	false } },
		{ S_DIV,			{ "/",			LBP_ARITHMETIC_2,	false } },
		{ S_MOD,			{ "%",			LBP_ARITHMETIC_2,	false } },
		{ S_POW,			{ "**",			LBP_ARITHMETIC_3,	false } },

		// Separators
		{ S_SEMICOLON,		{ ";",			LBP_NONE,			false } },
		{ S_COMMA,			{ ",",			LBP_COMMA,			false } },
		{ S_DOT,			{ ".",			LBP_DOT,			false } },
		{ S_COLON, 			{ ":", 			LBP_NONE, 			false } },

		// Language Features
		{ S_IF,				{ "if",			LBP_NONE,			true } },
		{ S_WHILE,			{ "while",		LBP_NONE,			true } },
		{ S_FOR, 			{ "for", 		LBP_NONE, 			true } },
		{ S_VAR,			{ "var",		LBP_IMMEDIATE,		true } },
		{ S_FUNC,			{ "function",	LBP_NONE,			true } },
		{ S_RETURN,			{ "return",		LBP_NONE,			true } },
		{ S_BREAK,			{ "break",		LBP_NONE,			true } },
		{ S_TRUE,			{ "true",		LBP_NONE,			true } },
		{ S_FALSE,			{ "false",		LBP_NONE,			true } },
		{ S_CLASS, 			{ "class", 		LBP_NONE, 			true } },
		{ S_DEFER, 			{ "defer", 		LBP_NONE, 			true } },
		{ S_NULL, 			{ "null", 		LBP_NONE, 			true } },
		{ S_TYPE, 			{ "type", 		LBP_TYPE, 			true } },
		{ S_TYPETEXT, 		{ "typestr",	LBP_TYPE, 			true } },

		// Types
		{ S_TYPESTRING, 	{ "string", 	LBP_NONE, 			true } },
		{ S_TYPEINTEGER, 	{ "int", 		LBP_NONE, 			true } },
		{ S_TYPEDECIMAL, 	{ "decimal", 	LBP_NONE, 			true } },
		{ S_TYPEBOOLEAN, 	{ "bool", 		LBP_NONE, 			true } },
		{ S_TYPEARRAY, 		{ "array", 		LBP_NONE, 			true } },
		{ S_TYPEOBJECT, 	{ "object", 	LBP_NONE, 			true } },
		{ S_TYPENONE, 		{ "none", 		LBP_NONE, 			true } },
	};
}
