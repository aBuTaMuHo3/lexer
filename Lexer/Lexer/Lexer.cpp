#include "stdafx.h"
#include "Lexer.h"
using namespace std;
Lexer::Lexer()
	:m_isBlockComment(false)
	, m_line(++m_line)
	, m_isLineComment(false)
{

}

vector<pair<string, string>> Lexer::CreateTokenList(istream &input)
{

	while (!input.eof())
	{
		bool isError = false;
		size_t it = 0;
		string line;
		getline(input,line);
		while (it < line.length())
		{
			string element = GetToken(it, line);
			if (!element.empty())
			{
				string type = GetTokenType(element);
				if (type == STRING_TOKEN || type == CHAR_TOKEN)
				{
					element.erase(0, 1);
					element.erase(element.length() - 1, 1);
				}
				if (type == LINE_COMMENT || m_isBlockComment)
				{
					break;
				}
				if (type == OPEN_BLOCK_COMMENT)
				{
					SkipComment(it, line);
					continue;
				}
				m_tokens.push_back(make_pair(type, element));

			}
		}
		
	}
	return m_tokens;

}


string Lexer::GetToken(size_t & it, string & line)
{
	string result = "";
	if (find(SEPARATOR_VALUES.begin(), SEPARATOR_VALUES.end(), line[it]) == SEPARATOR_VALUES.end()
		&& line[it] != DOT_VALUE)
	{
		if (line[it] != WHITESPACE)
		{
			result = isdigit(line[it]) ? GetNumber(line, it) : GetIdentificator(line, it);
		}
		else
		{
			++it;
		}
	}
	else
	{
		result = (line[it] == CHAR_QUOTE_VALUE || line[it] == STRING_QUOTE_VALUE) ? GetChars(line[it], line, it) : GetOperator(line, it);
	}
	return result;
}


string Lexer::GetChars(char quote, string & line, size_t &it)
{
	string value = "";
	value += quote;
	++it;
	while (it < line.length() && (/*(line[it - 1] && line[it] == quote) &&*/ line[it] != quote))
	{
		value += line[it];
		++it;
	}
	value += quote;
	++it;
	return value;
}

string Lexer::GetIdentificator(string & line, size_t & it)
{
	string value = "";
	while (it < line.length() && (find(SEPARATOR_VALUES.begin(), SEPARATOR_VALUES.end(),
		line[it]) == SEPARATOR_VALUES.end() && line[it] != DOT_VALUE) && line[it] != WHITESPACE)
	{
		value += line[it];
		++it;
	}
	return value;
}

string Lexer::GetNumber(string & line, size_t & it)
{
	string value = "";
	while (it < line.length() && find(SEPARATOR_VALUES.begin(), SEPARATOR_VALUES.end(),
		line[it]) == SEPARATOR_VALUES.end() && line[it] != WHITESPACE)
	{
		value += line[it];
		++it;
	}
	return value;
}

string Lexer::GetOperator(string & line, size_t & it)
{
	string value = "";
	while (it < line.length() && (find(SEPARATOR_VALUES.begin(), SEPARATOR_VALUES.end(),
		line[it]) != SEPARATOR_VALUES.end() || line[it] == DOT_VALUE) && TOKENS.find(value + line[it]) != TOKENS.end()
		&& line[it] != WHITESPACE)
	{
		value += line[it];
		++it;
	}
	return value;
}

string Lexer::GetTokenType(string & token)
{
	if (TOKENS.find(token) != TOKENS.end())
	{
		return TOKENS.find(token)->second;
	}
	if (IsString(token))
	{
		return STRING_TOKEN;
	}
	if (IsChar(token))
	{
		return CHAR_TOKEN;
	}
	if (IsIdentificator(token))
	{
		return ID_TOKEN;
	}
	if (IsNumber(token))
	{
		return GetNumberType(token);
	}
	return ERROR;
}

void Lexer::SkipComment(size_t & it, string & line)
{
	size_t nextPos = line.find(CLOSE_BLOCK_COMMENT_VALUE, it);
	m_isBlockComment = (nextPos == string::npos) ? true : false;
	it = nextPos + 2;
}

bool Lexer::IsString(string & token)
{
	return (*token.begin() == STRING_QUOTE_VALUE && *(token.rbegin()) == STRING_QUOTE_VALUE);
}

bool Lexer::IsChar(string & token)
{
	return (*token.begin() == CHAR_QUOTE_VALUE && *(token.rbegin()) == CHAR_QUOTE_VALUE);
}

bool Lexer::IsIdentificator(string & token)
{
	for (size_t it = 0; it < token.length(); ++it)
	{
		if ((it == 0 && !isalpha(token[it])) || (!isalpha(token[it]) && !isdigit(token[it])))
		{
			return false;
		}
	}
	return true;
}

bool Lexer::IsNumber(string & token)
{
	for (size_t it = 0; it < token.length(); ++it)
	{
		if (it == 0 && !IsStartsLikeNumber(it, token))
		{
			return false;
		}
		else if (!IsValidNumberSymbol(it, token) && !HaveOtherBase(it, token))
		{
			return false;
		}
	}
	return true;
}

bool Lexer::IsStartsLikeNumber(size_t it, string & token)
{
	return it == 0 && isdigit(token[it]);
}

bool Lexer::HaveOtherBase(size_t it, string & token)
{
	return it == 1 && (token.find(HEX_NUMBER_PREF) == 0 || token.find(BIN_NUMBER_PREF) == 0
		|| token.find(OCT_NUMBER_PREF) == 0) || find(AVALIABLE_FOR_HEX.begin(),
			AVALIABLE_FOR_HEX.end(), toupper(token[it])) != AVALIABLE_FOR_HEX.end() || toupper(token[it]) == EXP_INF;
}

bool Lexer::IsValidNumberSymbol(size_t it, string & token)
{
	return (it == token.length() - 1 && (token[it] == FLOAT_END))
		|| (token[it] == DOT_VALUE) || isdigit(token[it]);
}

string Lexer::GetNumberType(string & token)
{
	if (CheckBase(token, AVALIABLE_FOR_OCT, OCT_NUMBER_PREF))
	{
		return OCT_NUMBER_TOKEN;
	}

	if (CheckBase(token, AVALIABLE_FOR_BIN, BIN_NUMBER_PREF))
	{
		return BIN_NUMBER_TOKEN;
	}

	if (CheckBase(token, AVALIABLE_FOR_HEX, HEX_NUMBER_PREF))
	{
		return HEX_NUMBER_TOKEN;
	}

	return GetDecimalNumberType(token);
}

bool Lexer::CheckBase(string & token, const vector<char> &baseValues, const string &pref)
{
	int delimeterCounter = 0;
	if (token.find(pref) == string::npos)
	{
		return false;
	}
	/*else if (pref == OCT_NUMBER_PREF && token == "0")
	{
		return false;
	}*/
	for (size_t it = 2; it < token.length(); ++it)
	{
		if (token[it] == DOT_VALUE)
		{
			++delimeterCounter;
			continue;
		}
		if (find(baseValues.begin(), baseValues.end(), toupper(token[it])) == baseValues.end()
			|| delimeterCounter > 1 )
		{
			return false;
		}		
	}
	return true;
}

string Lexer::GetDecimalNumberType(string &token)
{
	int delimeterCounter = 0;
	if (token.find(EXP_INF) != string::npos || token.find(tolower(EXP_INF)) != string::npos)
		return GetExp(token);
	for (size_t it = 0; it < token.length(); ++it)
	{
		if (!isdigit(token[it]))
		{
			if (token[it] == DOT_VALUE)
			{
				++delimeterCounter;
			}
			else if (it == token.size() - 1 && token[it] == FLOAT_END)
			{
				return FLOAT_TOKEN;
			}
			else
			{
				return ERROR;
			}
			if (delimeterCounter > 1)
			{
				return ERROR;
			}
		}
	}
	return (delimeterCounter == 0) ? INT_TOKEN : FLOAT_TOKEN;
}

std::string Lexer::GetExp(string & token)
{
	size_t ePos = 0;
	ePos = token.find(EXP_INF);
	if (ePos == string::npos)
	{
		ePos = token.find(tolower(EXP_INF));
	}
	string mType = GetDecimalNumberType(token.substr(0, ePos - 1));
	string pType = "";
	if (token[ePos + 1] == '+' || token[ePos + 1] == '-')
	{
		ePos++;
	}
	pType = GetDecimalNumberType(token.substr(ePos+1));
	if (pType == ERROR || mType == ERROR || pType == FLOAT_TOKEN)
	{
		return ERROR;
	}
	return FLOAT_TOKEN;
}


