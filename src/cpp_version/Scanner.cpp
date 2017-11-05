//
// Created by manout on 17-11-4.
//

#include "Scanner.h"

Scanner::Scanner(const std::string &path):ch(0), file(path)
{

}

Scanner::~Scanner()
{
	file.close();
}

char Scanner::readChar()
{
	if (not file.eof())
	{
		do
		{
			ch = static_cast<char>(file.get());
		}while (not file.eof());
	}
	else
	{
		ch = -1;
	}
	return ch;
}

char Scanner::nextChar()
{
	auto next = static_cast<char>(file.get());
	file.unget();
	return next;
}

bool Scanner::isEof()
{
	return file.eof();
}

std::string* Scanner::readLine()
{
	std::string* line = new std::string();
	if ( not file.eof())
	{
		getline(file, *line, '\n');
		return line;
	}
	else
	{
		delete line;
		return nullptr;
	}
}



