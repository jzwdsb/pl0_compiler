%{/* plo 语言词法分析程序 lex 描述文档*/

int count = 0;
%}
%option yylineno case-insensitive

keyword			const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int

constant		[+-]?[0-9]+((\.[0-9]+)|([Ee][+-]?[0-9]))?

identfier		[A-Za-z_][A-Za-z_0-9]*

operator		\+|-|\*|\/|:=|>=|<=|#|=|>|<|<>

delimiter		[,.;():]

delim			[ \n\t]

whitespace		{delim}+

%%

{keyword}		{	printf("< %s, 1 >\t",  yytext);}

{operator}		{	printf("< %s, 2 >\t",  yytext);}

{delimiter}		{	printf("< %s, 3 >\t",  yytext);}

{constant}		{	printf("< %s, 4 >\t",  yytext);}

{identfier}		{	printf("< %s, 5 >\t",  yytext);}

\t+				{				 }
\n+				{	printf("\n");}
%%

int  main()
{
	printf("1: keyword\n");
	printf("2: operator\n");
	printf("3: delimiter\n");
	printf("4: constant\n");
	printf("5: identfier\n");
	yylex();
	return 0;
}

int yywrap()
{
	return 1;
}
