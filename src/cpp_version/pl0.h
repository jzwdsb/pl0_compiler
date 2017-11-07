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
	inc,
	jmp,
	jpc,
	sio
};

/* 类P Code 指令类型， 包含三个字段，指令f, 层差l, 和另一个操作数a*/
struct instruction
{
	enum fct OP;
	int L;
	int M;
	instruction():OP(0),L(0),M(0){}
};

struct Symbol
{
	
	std::string name;
	enum object type;
	int value;          // only used by constant
	int level;          // layer, used by variable, array, procedure
	int addr;           // address, used by procedure
	int size;           // size, used by procedure
	Symbol():name(), type(0), value(0), level(0), addr(0),size(0){};
	Symbol(const Symbol&) = default;
	Symbol(const std::string& name):name(name),type(0),value(0), level(0),addr(0), size(0){};
};


extern std::unordered_set<std::string> key_word_set;

// 为了方便而声明的操作符字符串，同时也是为了以后的词法分析方便扩展
// 包含所有操作符所使用的字符
extern std::string operator_string;
extern std::unordered_set<std::string> operator_set;

extern std::unordered_set<std::string> rel_op;


// 为了方便而声明的分隔符字符串，包含所有分割符用到的字符
extern std::string delimiter_string;
extern std::unordered_set<std::string> delimiter_set;

extern std::string err_msg[];




// 以下函数若匹配成功则生成相应的目标代码，若失败则将词素推回以进行下一步分析

void generate_code();

void program();
void block();
void const_declaration();
void var_declaration();
void procedure_declaration();
void statement();
void condition();
void expression();
void term();
void factor();
void error(int err_code);
void interpret();
#endif //CPP_VERSION_PL0_H
