//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_PL0_H
#define CPP_VERSION_PL0_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


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
	fct OP;
	int L;
	int M;
	instruction() = default;
	instruction(fct op, int l, int m):OP(op), L(l), M(m){}
};

struct Symbol
{
public:
	std::string name;
	object type;
	int value;          // only used by constant
	int level;          // layer, used by variable, array, procedure
	int addr;           // address, used by procedure
	int size;           // size, used by procedure
	Symbol() = default;
	Symbol(const Symbol&) = default;
	Symbol(const std::string& name):
		name(name),type(object::array),value(0), level(0),addr(0), size(0){};
};


extern void generate_code(fct, int, int);
/** 以下函数若匹配成功则生成相应的 类P code代码，
 * 通过向前查看一个移进符号选择移进规则
 * 错误处理通过 error 输出错误信息后 exit(-1) 直接退出*/

extern void program();
extern void block();
extern void const_declaration();
extern void var_declaration();
extern void procedure_declaration();
extern void statement();
extern void condition();
extern void expression();
extern void term();
extern void factor();
extern void error(int err_code);

/**	解释器*/
void interpret();
#endif //CPP_VERSION_PL0_H
