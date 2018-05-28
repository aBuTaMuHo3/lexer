#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "LexerConst.h"
class Lexer
{
public:
	Lexer();
	~Lexer() = default;

	std::vector<std::pair<std::string, std::string>> CreateTokenList(std::istream &input);

private:

	std::string GetToken(size_t & it, std::string & line);
	std::string GetChars(char quote, std::string & line, size_t & it);
	std::string GetIdentificator(std::string & line, size_t & it);
	std::string GetNumber(std::string & line, size_t & it);
	std::string GetOperator(std::string & line, size_t & it);
	std::string GetTokenType(std::string & token);
	void SkipComment(size_t & it, std::string & line);
	bool IsString(std::string & token);
	bool IsChar(std::string & token);
	bool IsIdentificator(std::string & token);
	bool IsNumber(std::string & token);
	bool IsStartsLikeNumber(size_t it, std::string & token);
	bool HaveOtherBase(size_t it, std::string & token);
	bool IsValidNumberSymbol(size_t it, std::string & token);
	bool CheckBase(std::string & token, const std::vector<char> & baseValues, const std::string & pref);
	std::string GetNumberType(std::string & token);
	std::string GetDecimalNumberType(std::string & token);
	std::string GetExp(std::string & token);


	unsigned int m_line;
	bool m_isBlockComment;
	bool m_isLineComment;
	std::vector<std::pair<std::string,std::string>> m_tokens;
};



