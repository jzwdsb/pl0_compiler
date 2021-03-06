//
// Created by manout on 17-11-4.
//

#ifndef CPP_VERSION_LEXER_H
#define CPP_VERSION_LEXER_H

#include <queue>
#include "pl0.h"
#include "Scanner.h"
class Lexer
{
public:
	explicit Lexer(Scanner* scer);
	std::string get_token();
	const std::string & next_token();
	bool isEof();
	virtual ~Lexer() = default;
private:
	void prepare();
	Scanner* scanner;
	std::queue<std::string> token_table;
};


#endif //CPP_VERSION_LEXER_H
