%{/* plo 语言词法分析程序 lex 描述文档*/

int count = 0;
%}

keyword			const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int

constant		([0-9])+

identfier		[A-Za-z]+

operator		\+|-|\*|\/|:=|>=|<=|#|=|>|<|<>

delimiter		[,.;():]

delim			[ \n\t]

whitespace		{delim}+

%%

{keyword}		{
					++count;printf("%d\t< %s, 1 >\n", count, yytext);
				}

{operator}		{
					++count;printf("%d\t< %s, 2 >\n", count, yytext);
				}

{delimiter}		{
					++count;printf("%d\t< %s, 3 >\n", count, yytext);
				}

{constant}		{
					++count;printf("%d\t< %s, 4 >\n", count, yytext);
				}

{identfier}		{
					++count;printf("%d\t< %s, 5 >\n", count, yytext);
				}

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
