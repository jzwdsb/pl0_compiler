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

/** 运行时程序调用栈，用来存储过程跳转时的上下文*/
std::vector<int> call_stack;

/** 生成的类P Code 代码表, 对应于程序的text(正文段)*/
extern std::vector<instruction> code;

/** 对应于每个 procedure 的局部变量*/
extern std::unordered_map<int, std::unordered_map<int, int>> local_variable;

#define stack_operate(OP)   runtime_stack[ESP - 1] = runtime_stack[ESP - 1] OP runtime_stack[ESP]

void interpret()
{
	
	
	/**	初始化程序计数器，从翻译的 code 段起始位置计数*/
	PC = 0;
	
	/** 初始化栈基寄存器和栈顶寄存器*/
	ESP = static_cast<int>(runtime_stack.size() - 1);
	EBP = 0;
	
	std::cout << "=========start interpret PL/0 program==========" << std::endl;
	do
	{
		IR = code[PC];
		switch (IR.OP)
		{
			case fct::lit :
				/** lit 0 M
				 * 		push the constant value M onto the stack*/
				++ESP;
				runtime_stack[ESP] = IR.M;
				break;
			case fct::opr :
				switch (IR.M)
				{
					case 0:
						/**	opr 0 0
						 * 		return to the caller from a procedure*/
						PC = call_stack.back();
						call_stack.pop_back();
						++PC;
						break;
					case 1:
						/** opr 0 1
						 * 		Negation, pop the stack and return the negative of the value
						 * */
						runtime_stack[ESP] = -runtime_stack[ESP];
						++PC;
						break;
					case 2:
						/**	opr 0 2
						 * 		add
						 * */
						stack_operate(+);
						--ESP;
						++PC;
						break;
					case 3:
						/**	opr 0 3
						 * 		sub
						 * */
						stack_operate(-);
						--ESP;
						++PC;
						break;
					case 4:
						/** opr 0 4
						 * 		multiplication
						 * */
						stack_operate(*);
						--ESP;
						++PC;
						break;
					case 5:
						/**	opr 0 5
						 * 		divide
						 * */
						stack_operate(/);
						--ESP;
						++PC;
						break;
					case 6:
						/**	opr 0 6
						 * 		is it odd? pop the stack and push 1 if odd, 0 if even*/
						runtime_stack[ESP] = runtime_stack[ESP] % 2;
						++PC;
						break;
					case 7:
						/**	opr 0 7
						 * 		modulus ;pop two values from the stack, divide second
						 *   and push the reminder
						 * */
						stack_operate(%);
						--ESP;
						++PC;
						break;
					case 8:
						/** opr 0 9
						 * 		equality; pop two values from the stack
						 * 	 and push 1 if equal, 0 if not*/
						stack_operate(==);
						--ESP;
						++PC;
						break;
					case 9:
						/**	opr 0 9
						 * 		inequality
						 * 	 pop two values, push 0 if is equal, 1 if not*/
						stack_operate(!=);
						--ESP;
						++PC;
						break;
					case 10:
						/**	opr 0 10
						 * 		less then
						 * 	 pop two values, push 1 if first is less then second, 0 if not*/
						stack_operate(>);
						--ESP;
						++PC;
						break;
					case 11:
						/**	opr 0 11
						 * 		less then or equal then
						 * 	 pop two values, push 1 if first is less then  or equal second, 0 if not*/
						stack_operate(>=);
						--ESP;
						++PC;
						break;
					case 12:
						/** opr 0 12
						 * 		greater then
						 * */
						stack_operate(<);
						--ESP;
						++PC;
						break;
					case 13:
						/**	opr 0 13
						 * 		greater or equal to
						 * */
						stack_operate(<=);
						--ESP;
						++PC;
						break;
					default:
						error(25);
				}
				break;
			case fct::lod :
				 // TODO here is going to implement load variable
				++ESP;
				++PC;
				break;
			case fct::sto :
				// TODO here is going to implement store variable
				++PC;
				break;
			case fct::cal :
				/**	将当前 PC 入栈保存调用链*/
				call_stack.push_back(::PC);
				::PC = IR.M;
				break;
			case fct::inc :
				ESP += IR.M;
				++PC;
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
						++PC;
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
		
	} while (ESP not_eq 0);
}

#undef stack_operate