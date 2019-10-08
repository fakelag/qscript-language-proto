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
		LBP_LOGIC_NOT		= 20,
		LBP_LOGIC			= 30,
		LBP_ASSIGN			= 30,
		LBP_EQUALITY		= 40,
		LBP_ARITHMETIC_1	= 60,
		LBP_ARITHMETIC_2	= 70,
		LBP_ARITHMETIC_3	= 80,
		LBP_INCDEC			= 90,
		LBP_OPENBRACKET		= 100,
		LBP_DOT				= 110,
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
		S_DOT,
		S_IF,
		S_WHILE,
		S_FOR,
		S_VAR,
		S_FUNC,
		S_RETURN,
		S_TRUE,
		S_FALSE,
		S_BREAK,

		// Pseudo-symbols
		S_SCOPE,
		S_CALL,
		S_FUNCDEF,
		S_FUNCBODY,

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
		{ S_ASSIGN,			{ "=",			LBP_ASSIGN,			} },
		{ S_ASSIGN_ADD,		{ "+=",			LBP_ASSIGN,			} },
		{ S_ASSIGN_SUB,		{ "-=",			LBP_ASSIGN,			} },
		{ S_ASSIGN_MUL,		{ "*=",			LBP_ASSIGN,			} },
		{ S_ASSIGN_DIV,		{ "/=",			LBP_ASSIGN,			} },
		{ S_ASSIGN_MOD,		{ "%=",			LBP_ASSIGN,			} },

		// Logical operators
		{ S_LOGIC_AND,		{ "&&",			LBP_LOGIC,			} },
		{ S_LOGIC_OR,		{ "||",			LBP_LOGIC,			} },
		{ S_LOGIC_NOT,		{ "!",			LBP_LOGIC_NOT,		} },

		// Equality
		{ S_EQUALS,			{ "==",			LBP_EQUALITY,		} },
		{ S_EQUALS_NOT,		{ "!=",			LBP_EQUALITY,		} },
		{ S_MORE_OR_EQUALS,	{ ">=",			LBP_EQUALITY,		} },
		{ S_LESS_OR_EQUALS,	{ "<=",			LBP_EQUALITY,		} },
		{ S_MORETHAN,		{ ">",			LBP_EQUALITY,		} },
		{ S_LESSTHAN,		{ "<",			LBP_EQUALITY,		} },

		// Increment/Decrement
		{ S_INCREMENT,		{ "++",			LBP_INCDEC,			} },
		{ S_DECREMENT,		{ "--",			LBP_INCDEC,			} },

		// Brackets
		{ S_BRACKET_OPEN,	{ "{",			LBP_NONE,			} },
		{ S_BRACKET_CLOSE,	{ "}",			LBP_NONE,			} },
		{ S_SBRACKET_OPEN,	{ "[",			LBP_OPENBRACKET,	} },
		{ S_SBRACKET_CLOSE,	{ "]",			LBP_NONE,			} },
		{ S_PARENT_OPEN,	{ "(",			LBP_OPENBRACKET,	} },
		{ S_PARENT_CLOSE,	{ ")",			LBP_NONE,			} },

		// Arithmetics
		{ S_ADD,			{ "+",			LBP_ARITHMETIC_1,	} },
		{ S_SUB,			{ "-",			LBP_ARITHMETIC_1,	} },
		{ S_MUL,			{ "*",			LBP_ARITHMETIC_2,	} },
		{ S_DIV,			{ "/",			LBP_ARITHMETIC_2,	} },
		{ S_MOD,			{ "%",			LBP_ARITHMETIC_2,	} },
		{ S_POW,			{ "**",			LBP_ARITHMETIC_3,	} },

		// Separators
		{ S_SEMICOLON,		{ ";",			LBP_NONE,			} },
		{ S_COMMA,			{ ",",			LBP_COMMA,			} },
		{ S_DOT,			{ ".",			LBP_DOT,			} },

		// Language Features
		{ S_IF,				{ "if",			LBP_NONE,			} },
		{ S_WHILE,			{ "while",		LBP_NONE,			} },
		{ S_FOR, 			{ "for", 		LBP_NONE, 			} },
		{ S_VAR,			{ "var",		LBP_IMMEDIATE,		} },
		{ S_FUNC,			{ "function",	LBP_NONE,			} },
		{ S_RETURN,			{ "return",		LBP_NONE,			} },
		{ S_BREAK,			{ "break",		LBP_NONE,			} },
		{ S_TRUE,			{ "true",		LBP_NONE,			} },
		{ S_FALSE,			{ "false",		LBP_NONE,			} },
	};
}
