//
// Created by manout on 17-11-4.
//

#ifndef CPP_VERSION_SCANNER_H
#define CPP_VERSION_SCANNER_H

#include <string>
#include <fstream>
#include <queue>

class Scanner
{
public:
	explicit Scanner (const std::string& path);
	virtual ~Scanner();
	char readChar();
	bool isEof();
	
private:
	void prepare();
	std::string buffer;
	std::string ::iterator it;
	std::fstream file;
};


#endif //CPP_VERSION_SCANNER_H
