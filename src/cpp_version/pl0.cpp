//
// Created by manout on 17-11-5.
//

#include <regex>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "pl0.h"
#include "Scanner.h"
#include "Lexer.h"


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

/**运行时栈，用来存储过程调用返回地址*/
std::vector<int> runtime_stack;

/**	定义全局词法分析器*/
Scanner *scanner = new Scanner("../../demo.pl0");
Lexer   *lexer   = new Lexer(scanner);

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

std::unordered_set<std::string> rel_op
	({
		"=","<>", "<", "<=", ">", ">="
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
		/* 25*/      "Invalid Instruction",
		/* 26*/      "",
		/* 27*/      "",
		/* 28*/      "",
		/* 29*/      "",
		/* 30*/      "",
		/* 31*/      "The number is too great",
		/* 32*/      "There are too many levels",
	};

/**	顶层命名空间*/
SymbolTable top;

/**	当前命名空间*/
SymbolTable  *local_space = &top;

/** 生成的类P Code 代码表, 对应于程序的text(正文段)*/
std::vector<instruction> code;


/** 当前层级*/
int level = 0;

/** 当前 procedure 长度*/
int prod_size = 0;

enum type get_type(const std::string& token)
{
	if (key_word_set.find(token) not_eq key_word_set.end())
	{
		return  type::keyword;
	}
	if (operator_set.find(token) not_eq operator_set.end())
	{
		return type::_operator;
	}
	if (delimiter_set.find(token) not_eq delimiter_set.end())
	{
		return type::delimiter;
	}
	if (std::regex_match(token, std::regex(R"([0-9]+)")))
	{
		return type::constant;
	}
	if (std::regex_match(token, std::regex(R"([A-Za-z_][A-Za-z0-9_]*)")))
	{
		return type::identifier;
	}
	// 控制流到此出说明出现没有任何规则规则可以匹配的 token
	// 应当进行错误处理
	error(19);
}


__always_inline
void generate_code(int OP, int L, int M)
{
	code.emplace_back(OP, L, M);
}


void program()
{
	std::string curr_token;
	block();
	if (lexer->next_token() == ".")
	{
		curr_token = lexer->get_token();
		return ;
	}
	else
	{
		error(9);
	}
}

void block()
{
	SymbolTable* prev = local_space;
	local_space = new SymbolTable(prev);
	const_declaration();
	var_declaration();
	procedure_declaration();
	statement();
	delete local_space;
	local_space = prev;
}

void const_declaration()
{
	std::string curr_token = lexer->next_token();
	if (curr_token == "const")
	{
	
	}
}

void var_declaration()
{
	std::string curr_token = lexer->next_token();
	if (curr_token == "int" or curr_token == "var" )
	{
	
	}

}

void procedure_declaration()
{
	std::string curr_token = lexer->next_token();
	if (curr_token == "procedure")
	{
		while (curr_token == "procedure")
		{
			curr_token = lexer->get_token();
			Symbol curr_prod(curr_token);
			curr_prod.type = object::procedure;
			curr_prod.addr = static_cast<int>(code.size());
			
		}
	}
}

void statement()
{
	std::string curr_token;
	static std::unordered_map<std::string, std::function<void()>> oper_table
		({
			 {
				"call", [&](){}
			 },
			 {
				 "begin", [&](){statement();
				                while(lexer->next_token() == ";")
				                {;statement();}
			                    if (lexer->next_token() == "end")
			                    {;}
			                    else error(17);}
			 },
			 {
				 "if",[&](){condition();
				            if (lexer->next_token() == "then")
				            {lexer->get_token();statement();}
			                if (lexer->next_token() == "else")
			                {statement();}}
			 },
			 {
				 "while", [&](){condition();
				                if (lexer->next_token() == "do")
				                {statement();}
			                    else error(18);}
			 },
			 {
				 "read", [&](){}
			 },
			 {
				 "write", [&](){ expression();}
			 },
		 });
	
	if (oper_table.find(lexer->next_token()) not_eq oper_table.end())
	{
		curr_token = lexer->get_token();
		oper_table[curr_token]();
	}
	else if (local_space->get(lexer->next_token()) not_eq nullptr)
	{
		// 获取标志符
		curr_token = lexer->get_token();
		
		Symbol* ident = local_space->get(curr_token);
		
		if (lexer->next_token() == ":=")
		{
			curr_token = lexer->get_token();
			expression();
			ident->value = runtime_stack.back();
			
		}
		else
		{
			error(13);
		}
	}
	else
	{
		expression();
	}
}

void condition()
{
	std::string curr_token;
	static std::unordered_map<std::string, std::function<void()>> relation_OP
		({
			 {
				 "=", [&]{code.emplace_back(fct::opr, 0, 8);}
			 },
			 {
				"<>", [&]{code.emplace_back(fct::opr, 0, 9);}
			 },
			 {
				">", [&]{code.emplace_back(fct::opr, 0, 10);}
			 },
			 {
				">=", [&]{code.emplace_back(fct::opr, 0, 11);}
			 },
			 {
				"<", [&]{code.emplace_back(fct::opr, 0, 12);}
			 },
			 {
				"<=", [&]{code.emplace_back(fct::opr, 0, 13);}
			 }
			 
		 });
	if (lexer->next_token() == "odd")
	{
		curr_token = lexer->get_token();
		expression();
		code.emplace_back(fct::opr, 0, 6);
	}
	else
	{
		expression();
		if (rel_op.find(lexer->next_token()) not_eq rel_op.end())
		{
			curr_token = lexer->get_token();
			expression();
			relation_OP[curr_token]();
		}
		else
		{
			error(20);
		}
	}
	
	
}

void expression()
{
	std::string curr_token;
	if (lexer->next_token() == "+" or lexer->next_token() == "-")
	{
		curr_token = lexer->get_token();
		term();
		if (curr_token == "-")
		{
			code.emplace_back(fct::opr, 0, 1);
		}
		while (lexer->next_token() == "+" or lexer->next_token() == "-")
		{
			curr_token = lexer->get_token();
			term();
			switch (curr_token[0])
			{
				case '+':
					code.emplace_back(fct::opr, 0, 2);
					break;
				case '-':
					code.emplace_back(fct::opr, 0, 3);
					break;
				default:
					break;
			}
			
		}
	}
	else
	{
		term();
		curr_token = lexer->next_token();
		while (curr_token == "+" or curr_token == "-")
		{
			curr_token = lexer->get_token();
			term();
			switch (curr_token[0])
			{
				case '+':
					code.emplace_back(fct::opr, 0, 2);
					break;
				case '-':
					code.emplace_back(fct::opr, 0, 3);
					break;
				default:
					break;
			}
		}
	}
}

void term()
{
	std::string curr_token;
	factor();
	while (lexer->next_token() == "*" or lexer->next_token() == "/" or lexer->next_token() == "%")
	{
		curr_token = lexer->get_token();
		factor();
		switch (curr_token[0])
		{
			case '*':
				code.emplace_back(fct::opr, 0, 4);
				break;
			case '/':
				code.emplace_back(fct::opr, 0, 5);
				break;
			case '%':
				code.emplace_back(fct::opr, 0, 7);
			default:
				break;
		}
	}
}

__always_inline
bool isnum(const std::string& str)
{
	for (auto ch : str)
	{
		if (not isdigit(ch))
		{
			return false;
		}
	}
	return true;
}

void factor()
{
	std::string curr_token;
	Symbol* symbol = nullptr;
	if (curr_token == "(")
	{
		curr_token = lexer->get_token();
		expression();
		curr_token = lexer->get_token();
		if (curr_token not_eq ")")
		{
			error(22);
		}
	}
	else if (isnum(curr_token))
	{
		int num = boost::lexical_cast<int>(curr_token);
		runtime_stack.push_back(num);
	}
	else if ((symbol = local_space->get(curr_token)) not_eq nullptr)
	{
		runtime_stack.push_back(symbol->value);
	}
	else
	{
		error(11);
	}
	
	
}

void error(int index)
{
	std::cerr<< err_msg[index]<<std::endl;
	exit(-1);
}

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
		switch(IR.OP)
		{
			case fct::lit :
				/** lit 0 M
				 * 		push the constant value M onto the stack*/
				runtime_stack.push_back(IR.M);
				++ESP;
				break;
			case fct::opr :
				switch (IR.M)
				{
					case 0:
						/**	opr 0 0
						 * 		return to the caller from a procedure*/
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
				break;
			case fct::sto :
				break;
			case fct::cal :
				break;
			case fct::inc :
				break;
			case fct::jmp :
				break;
			case fct::jpc :
				break;
			case fct::sio :
				break;
			default:
				error(25);
		}
		
	}while (ESP not_eq 0);
	
	
}