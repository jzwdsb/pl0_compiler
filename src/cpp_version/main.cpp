//
// Created by manout on 17-11-5.
//

#include <memory>
#include <iostream>

#include "pl0.h"
#include "Scanner.h"
#include "Lexer.h"

Scanner* scanner;
Lexer*   lexer;

/**	主程序的入口*/
int main (int argc, char* argv[])
{
	if (argc <  2)
	{
		std::cout << "usage: pl0 <source file>";
		return 0;
	}

	std::string filepath(argv[1]);
	scanner = new Scanner(filepath);
	lexer = new Lexer(scanner);
	program();
	show_code();
	save_code();
	interpret();
}
