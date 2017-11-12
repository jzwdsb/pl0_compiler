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


enum object
{
	constant,
	variable,
	array,
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

/** 类P Code 指令类型， 包含三个字段，指令OP, 层差L, 和另一个操作数M*/
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
	Symbol(const std::string& name):
		name(name),type(0),value(0), level(0),addr(0), size(0){};
};



/**
 * 为了方便而声明的操作符字符串，同时也是为了以后的词法分析方便扩展
 * 包含所有操作符所使用的字符 */
extern std::string operator_string;
extern std::unordered_set<std::string> rel_op;


/** 为了方便而声明的分隔符字符串，包含所有分割符用到的字符 */
extern std::string delimiter_string;

extern std::array<std::string, 32> err_msg;



void generate_code(int, int, int);
/** 以下函数若匹配成功则生成相应的 类P code代码，
 * 通过向前查看一个移进符号选择移进规则
 * 错误处理通过 error 输出错误信息后 exit(-1) 直接退出*/

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

/**	解释器*/
void interpret();
#endif //CPP_VERSION_PL0_H
