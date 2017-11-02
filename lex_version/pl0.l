%{/* plo 语言词法分析程序 lex 描述文档*/
#include <string>
#include <unordered_map>
//#define ECHO	{printf("can't recognise word %s\n", yytext);}
using std::string;
using std::unordered_map;
int count = 0;
enum  object 
{
	constant,
	variable,
	procedure
};

struct symbol
{
	string name ;
	enum object id_type;
	int level;
	int address;
	int size;
	int lines; 
};
unordered_map<string, int> name_map;
struct symbol symbol_table[100];

int top = 0;
int push_symbol()
{
	int curr_top = top;
	string name = yytext;
	if(name_map.find(name) == name_map.end())
	{
		name_map[name] = curr_top;
		symbol_table[curr_top].name = name;
		symbol_table[curr_top].lines = yylineno;
		++top;
		return curr_top;
	}
	return name_map[name];
}
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

{keyword}		{	printf("< 1, %s >  ",  yytext);}
{operator}		{	printf("< 2, %s >  ",  yytext);}
{delimiter}		{	printf("< 3, %s >  ",  yytext);}
{constant}		{	printf("< 4, %s >  ",  yytext);}
{identfier}		{	printf("< 5, offset of symbol_table: %d >  ",  yytext, push_symbol());}
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
