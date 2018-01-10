//
// Created by manout on 17-11-5.
//

#include <memory>
#include <iostream>

#include "pl0.h"
#include "Scanner.h"
#include "Lexer.h"

std::unique_ptr<Scanner> scanner;
std::unique_ptr<Lexer>   lexer;

/**	主程序的入口*/
int main ()
{
	std::string filename;
	std::cout << "please input the pl0 source file, full path" << std::endl;
	std::cin >> filename;
	scanner.reset(new Scanner(filename));
	lexer.reset(new Lexer(scanner.get()));
	program();
	show_code();
	save_code();
	interpret();
}