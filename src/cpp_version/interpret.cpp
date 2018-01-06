//
// Created by manout on 17-11-17.
//

#include <iostream>
#include "pl0.h"



/**
 * 八个通用寄存器，用来传递和暂存数据
 * EBX, EBP 为基址寄存器
 * ESP 为栈顶指针
 * */
int EAX, EBX, ECX, EDX, EBP, ESP, ESI, EDI;

/**	全局程序计数器*/
int PC;

/**	指令寄存器*/
instruction IR;

/** 运行时栈，用来存储过程所需局部变量*/
std::array<int, 1024> runtime_stack;

/** 生成的类P Code 代码表, 对应于程序的text(正文段)*/
extern std::vector<instruction> code;

/** 根据层差向前查询声明变量过程的基址*/
int base(int l)
{
	int b = EBP;
	while (l--)
	{
		b = runtime_stack[b];
	}
	return b;
}

#define stack_operate(OP)   runtime_stack[ESP - 2] = runtime_stack[ESP - 2] OP runtime_stack[ESP - 1]
/** 对　PC　更新操作作如下约定
 * 		１　顺序执行， PC每取一条指令自动加１，自增过程发生在取指令操作之后
 * 		２　跳转操作， 当发生跳转指令时，约定指令所指示的位置即为新的PC值(此点由编译器保证),
 * 		   跳转之后直接取指令然后更新PC
 * */
void interpret()
{
	/**	初始化程序计数器，从翻译的 code 段起始位置计数*/
	PC = 0;
	
	/** 初始化栈基寄存器和栈顶寄存器*/
	ESP = 0;
	EBP = 0;
	
	/** 初始化顶层静态链，动态链, 返回地址RA
	 *  取到顶层静态链时即程序结束*/
	runtime_stack[0] = 0;
	runtime_stack[1] = 0;
	runtime_stack[2] = 0;
	std::cout << "=========start interpret PL/0 program==========" << std::endl;
	do
	{
		IR = code[PC];
		++PC;
		switch (IR.OP)
		{
			case fct::lit :
				/** lit 0 M
				 * 		push the constant value M onto the stack*/
				runtime_stack[ESP] = IR.M;
				++ESP;
				break;
			case fct::opr :
				switch (IR.M)
				{
					case 0:
						/**	opr 0 0
						 * 		return to the caller from a procedure*/
						ESP = EBP;
						PC = runtime_stack[ESP + 2];
						EBP = runtime_stack[ESP + 1];
						break;
					case 1:
						/** opr 0 1
						 * 		Negation, pop the stack and return the negative of the value
						 * */
						runtime_stack[ESP - 1] = -runtime_stack[ESP - 1];
						break;
					case 2:
						/**	opr 0 2
						 * 		add
						 * */
						stack_operate(+);
						--ESP;
						break;
					case 3:
						/**	opr 0 3
						 * 		sub
						 * */
						stack_operate(-);
						--ESP;
						break;
					case 4:
						/** opr 0 4
						 * 		multiplication
						 * */
						stack_operate(*);
						--ESP;
						break;
					case 5:
						/**	opr 0 5
						 * 		divide
						 * */
						stack_operate(/);
						--ESP;
						break;
					case 6:
						/**	opr 0 6
						 * 		is it odd? pop the stack and push 1 if odd, 0 if even*/
						runtime_stack[ESP] = runtime_stack[ESP] % 2;
						break;
					case 7:
						/**	opr 0 7
						 * 		modulus ;pop two values from the stack, divide second
						 *   and push the reminder
						 * */
						stack_operate(%);
						--ESP;
						break;
					case 8:
						/** opr 0 9
						 * 		equality; pop two values from the stack
						 * 	 and push 1 if equal, 0 if not*/
						stack_operate(==);
						--ESP;
						break;
					case 9:
						/**	opr 0 9
						 * 		inequality
						 * 	 pop two values, push 0 if is equal, 1 if not*/
						stack_operate(!=);
						--ESP;
						break;
					case 10:
						/**	opr 0 10
						 * 		less then
						 * 	 pop two values, push 1 if first is less then second, 0 if not*/
						stack_operate(>);
						--ESP;
						break;
					case 11:
						/**	opr 0 11
						 * 		less then or equal then
						 * 	 pop two values, push 1 if first is less then  or equal second, 0 if not*/
						stack_operate(>=);
						--ESP;
						break;
					case 12:
						/** opr 0 12
						 * 		greater then
						 * */
						stack_operate(<);
						--ESP;
						break;
					case 13:
						/**	opr 0 13
						 * 		greater or equal to
						 * */
						stack_operate(<=);
						--ESP;
						break;
					default:
						error(25);
				}
				break;
			case fct::lod :
				/** to load a variable, we need to find base address of the procedure which declare it
				 *  then load it by the offset indicate by the field M*/
				runtime_stack[ESP] = runtime_stack[base(IR.L) + IR.M];
				++ESP;
				break;
			case fct::sto :
				/** basically like the load operation*/
				runtime_stack[base(IR.L) + IR.M] = runtime_stack[ESP - 1];
				break;
			case fct::cal :
				
				
				::PC = IR.M;
				break;
			case fct::inc :
				ESP += IR.M;
				break;
			case fct::jmp :
				PC = IR.M;
				break;
			case fct::jpc :
				if (runtime_stack[ESP] == 0)
				{
					PC = IR.M;
				}
				break;
			case fct::sio :
				switch (IR.M)
				{
					case 1:
						std::cout << runtime_stack[ESP];
						--ESP;
						break;
					case 2:
						int num;
						std::cin >> num;
						runtime_stack[++ESP] = num;
						break;
					default:
						error(25);
				}
				break;
			default:
				error(25);
		}
		
	} while (PC not_eq 0);
}

#undef stack_operate