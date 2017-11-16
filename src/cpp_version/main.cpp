//
// Created by manout on 17-11-5.
//


#include <FlexLexer.h>
#include "pl0.h"
#include "Scanner.h"
#include "Lexer.h"

Scanner* scanner = new Scanner("/home/manout/pl0_compiler/demo.pl0");
Lexer*   lexer   = new Lexer(scanner);

/**	主程序的入口*/
int main ()
{
	while (not lexer->isEof())
	{
		std::cout << lexer->get_token() <<std::endl;
	}
	
}