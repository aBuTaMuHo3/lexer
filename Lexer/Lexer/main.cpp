#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Lexer.h"
using namespace std;

void Print(const pair<string,string> &tokens)
{
		cout << "[" <<  tokens.first<< "]" << endl;
		cout <<tokens.second << endl<<endl;
		
}

int main(int argc, char * argv[])
{
	int a = 0654;
	cout << a << endl;
	setlocale(LC_ALL, "Russian");
	try
	{
		/*if (argc < 2)
		{
			throw logic_error("Invalid arguments count");
		}
		ifstream input(argv[1]);*/
		ifstream input("1.txt");
		string str;
		Lexer lexer;
		vector<pair<string, string>> tokenList;
		tokenList = lexer.CreateTokenList(input);
		for each (pair<string,string> token in tokenList)
		{
			Print(token);
		}
		cout << "[" << EOF_TOKEN << "]" << endl;
	}
	catch (exception & err)
	{
		cout << err.what() << endl;
	}

	return 0;
}