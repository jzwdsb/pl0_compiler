//
// Created by manout on 17-11-4.
//

#ifndef CPP_VERSION_SCANNER_H
#define CPP_VERSION_SCANNER_H

#include <string>
#include <fstream>

class Scanner
{
public:
	explicit Scanner (const std::string& path);
	virtual ~Scanner();
	std::string* readLine();
	char readChar();
	char nextChar();
	bool isEof();
	
private:
	char ch;
	std::string curr_line;
	std::fstream file;
};


#endif //CPP_VERSION_SCANNER_H
