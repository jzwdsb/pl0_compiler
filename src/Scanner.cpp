//
// Created by manout on 17-11-4.
//

#include "Scanner.h"

Scanner::Scanner(const std::string &path):buffer(),it(buffer.end()), file(path, std::fstream::in)
{
	prepare();
}

Scanner::~Scanner()
{
	file.close();
}

char Scanner::readChar()
{
	prepare();
	char ch = *it;
	++it;
	return ch;
}

bool Scanner::isEof()
{
	return file.eof();
}

void Scanner::prepare()
{
	if (it == buffer.end() and not file.eof())
	{
		getline(file, buffer);
		buffer.push_back('\n');
		it = buffer.begin();
	}
}
