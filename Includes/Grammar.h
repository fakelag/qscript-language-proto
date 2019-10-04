#pragma once

#include <string>
#include <map>

namespace Grammar
{
	enum LeftBindingPower
	{
		LBP_NAME			= 0,
		LBP_FEATURE			= 0, // TODO: Split this into multiple (e.g LBP_FORLOOP)
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
		S_COLON,
		S_IF,
		S_WHILE,
		S_VAR,
		S_FUNC,
		S_RETURN,
		S_TRUE,
		S_FALSE,
		S_SCOPE,
		S_BREAK,
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
		{ S_BRACKET_OPEN,	{ "{",			0,					} },
		{ S_BRACKET_CLOSE,	{ "}",			0,					} },
		{ S_SBRACKET_OPEN,	{ "[",			LBP_OPENBRACKET,	} },
		{ S_SBRACKET_CLOSE,	{ "]",			0,					} },
		{ S_PARENT_OPEN,	{ "(",			LBP_OPENBRACKET,	} },
		{ S_PARENT_CLOSE,	{ ")",			0,					} },

		// Arithmetics
		{ S_ADD,			{ "+",			LBP_ARITHMETIC_1,	} },
		{ S_SUB,			{ "-",			LBP_ARITHMETIC_1,	} },
		{ S_MUL,			{ "*",			LBP_ARITHMETIC_2,	} },
		{ S_DIV,			{ "/",			LBP_ARITHMETIC_2,	} },
		{ S_MOD,			{ "%",			LBP_ARITHMETIC_2,	} },
		{ S_POW,			{ "**",			LBP_ARITHMETIC_3,	} },

		// Separators
		{ S_SEMICOLON,		{ ";",			LBP_SEMICOLON,		} },
		{ S_COLON,			{ ",",			LBP_COLON,			} },

		// Language Features
		{ S_IF,				{ "if",			LBP_FEATURE,		} },
		{ S_WHILE,			{ "while",		LBP_FEATURE,		} },
		{ S_VAR,			{ "var",		-1,					} },
		{ S_FUNC,			{ "function",	LBP_FEATURE,		} },
		{ S_RETURN,			{ "return",		LBP_FEATURE,		} },
		{ S_BREAK,			{ "break",		LBP_FEATURE,		} },
		{ S_TRUE,			{ "true",		LBP_FEATURE,		} },
		{ S_FALSE,			{ "false",		LBP_FEATURE,		} },
	};
}
