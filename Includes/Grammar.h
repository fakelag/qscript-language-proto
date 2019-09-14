#pragma once
#include <string>
#include <map>

namespace Grammar
{
	enum LeftBindingPower
	{
		LBP_NAME			= 0,
		LBP_SCOLON			= 0,
		LBP_ASSIGN			= 10,
		LBP_ARITHMETIC		= 40,
	};

	enum SymbolGroup
	{
		SG_ARITHMETIC		= 0,
		SG_ASSIGNMENT,
		SG_SCOLON,
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
		S_ADD,
		S_SUB,
		S_MUL,
		S_DIV,
		S_SCOLON,
	};

	static const std::map<Symbol, SymbolInfo_t> LanguageSymbols = {
		{ S_ADD,		{ "+", LBP_ARITHMETIC, SG_ARITHMETIC } },
		{ S_SUB,		{ "-", LBP_ARITHMETIC, SG_ARITHMETIC } },
		{ S_MUL,		{ "*", LBP_ARITHMETIC, SG_ARITHMETIC } },
		{ S_DIV,		{ "/", LBP_ARITHMETIC, SG_ARITHMETIC } },
		{ S_SCOLON,		{ ";", LBP_SCOLON, SG_SCOLON } },
	};
}
