//
// Created by manout on 17-11-5.
//

#include <memory>

#include "pl0.h"
#include "Scanner.h"
#include "Lexer.h"

std::unique_ptr<Scanner> scanner( new  Scanner("/home/manout/pl0_compiler/demo.pl0"));
std::unique_ptr<Lexer>   lexer ( new Lexer(scanner.get()));

/**	主程序的入口*/
int main ()
{
	var_declaration();
}