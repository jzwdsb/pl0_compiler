//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_PL0_H
#define CPP_VERSION_PL0_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#define NOT_INITIALIZE  0xffffffff

/** 标识符类型*/
enum object
{
	constant,
	variable,
	array,
	procedure,
	abnormal = 0xff     /** a error code to indicate this field need to be initialized*/
};

/** 类　P code 指令集*/
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
	sio,
	err = 0xff
};

/** 类P Code 指令类型， 包含三个字段，指令OP, 层差L, 和另一个操作数M*/
struct instruction
{
	fct OP = fct::err;
	int L = NOT_INITIALIZE;
	int M = NOT_INITIALIZE;
	instruction() = default;
	instruction(const instruction&) = default;
	instruction(fct op, int l, int m):OP(op), L(l), M(m){}
};


/** pl0　的符号定义*/
struct Symbol
{
	/** here assign other member to a abnormal value such as 0xffffffff,
	 *  so it will not hides the logic error in parsing process
	 * */
public:
	std::string name;
	object type = object::abnormal;
	int value = NOT_INITIALIZE;          /** only used by constant */
	int level = NOT_INITIALIZE;          /** level, used by variable, array, procedure */
	int addr = NOT_INITIALIZE;           /** address, used by procedure */
	int size = NOT_INITIALIZE;           /** size, used by procedure */
	Symbol() = default;
	Symbol(const Symbol&) = default;
	explicit Symbol(std::string name):
		name(std::move(name)){};
};

extern void generate_code(fct, int, int);
/** 以下函数若匹配成功则生成相应的 类P code代码，
 * 通过向前查看一个移进符号选择移进规则
 * 错误处理通过 error 输出错误信息后 exit(-1) 直接退出*/

extern void program();
extern void block();
extern void const_declaration();
extern void var_declaration();
extern void array_declaration();
extern void procedure_declaration();
extern void statement();
extern void condition();
extern void expression();
extern void term();
extern void factor();
extern void error(int err_code);
extern std::vector<std::string> code_to_str();
extern void show_code();
extern void save_code();
/**	解释器*/
void interpret();
#endif //CPP_VERSION_PL0_H
