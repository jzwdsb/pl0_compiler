//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_PL0_H
#define CPP_VERSION_PL0_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "SymbolTable.h"

enum type
{
	keyword,
	_operator,
	delimiter,
	constant,
	identifier
};

enum object
{
	constant,
	variable,
	arrray,
	procedure
};

enum fct
{
	lit,
	opr,
	lod,
	sto,
	cal,
	Int,
	jmp,
	jpc
};

struct instruction
{
	enum fct f;
	long l;
	long a;
};

struct Item
{
	
	std::string name;
	int type;
	int value;      // only used by constant
	int level;      // layer, used by variable, array, procedure
	int addr;       // address, used by procedure
	int size;       // address, used by procedure
	Item():name(), type(0), value(0), level(0), addr(0),size(0){};
	Item(const Item&) = default;
	Item(const std::string& name):name(name),type(0),value(0), level(0),addr(0), size(0){};
};

std::unordered_set<std::string> key_word_set
	({
	    "begin",
	    "end",
	    "if",
	    "then",
	    "else",
	    "const",
	    "procedure",
	    "var",
	    "int",
	    "do",
	    "while",
	    "call",
	    "read",
	    "write",
	    "repeat",
	    "odd",
	    "procedure"
	 });

// 为了方便而声明的操作符字符串，同时也是为了以后的词法分析方便扩展
// 包含所有操作符所使用的字符
std::string operator_string("+-*/%:=<>()");
std::unordered_set<std::string> operator_set
	({
		"+","-","*","/","=","<>", "<", "<=", ">", ">=", "(", ")",":="
	 });

// 为了方便而声明的分隔符字符串，包含所有分割符用到的字符
std::string delimiter_string(",;.");
std::unordered_set<std::string> delimiter_set
	({
		",", ";","."
	 });

std::string err_msg[] =
	{
		/*  0*/      "",
		/*  1*/      "Found ':=' when expecting '='",
		/*  2*/      "There must be a number to follow",
		/*  3*/      "There must be an '=' to follow the identifier",
		/*  4*/      "There must be an identifier to follow 'const', 'var' or 'procedure'",
		/*  5*/      "Missing ',' or ';'",
		/*  6*/      "Incorrect procedure name",
		/*  7*/      "Statement expected",
		/*  8*/      "Follow the statement is an incorrect symbol",
		/*  9*/      "'.' expected",
		/* 10*/      "',' expected",
		/* 11*/      "Undeclared identifier",
		/* 12*/      "Illegal assignment",
		/* 13*/      "':=' expected",
		/* 14*/      "There must be an identifier to follow the call",
		/* 15*/      "A constant or variable can not be called",
		/* 16*/      "'then' expected.",
		/* 17*/      "';' or 'end' expected",
		/* 18*/      "'do' expected.",
		/* 19*/      "Incorrect symbol.",
		/* 20*/      "Relative operators expected",
		/* 21*/      "Procedure identifier can not be in an expression",
		/* 22*/      "Missing ')'.",
		/* 23*/      "The symbol can not be followed by an factor",
		/* 24*/      "The symbol can not be as the beginning of an expression",
		/* 25*/      "",
		/* 26*/      "",
		/* 27*/      "",
		/* 28*/      "",
		/* 29*/      "",
		/* 30*/      "",
		/* 31*/      "The number is too great",
		/* 32*/      "There are too many levels",
	};

std::vector<instruction> code;      // 程序的执行栈
SymbolTable top;                    //

// 以下函数若匹配成功则生成相应的目标代码，若失败则将词素推回以进行下一步分析

void program();
void block();
void const_declaration();
void var_declaration();
void procedure_declaration();
void statement();
void condition();
void expression();
void term();

#endif //CPP_VERSION_PL0_H
