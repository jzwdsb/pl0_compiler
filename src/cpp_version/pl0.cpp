//
// Created by manout on 17-11-5.
//

#include <iostream>
#include <boost/lexical_cast.hpp>
#include "pl0.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include "Lexer.h"


/**	定义全局词法分析器*/
extern std::unique_ptr<Scanner> scanner ;
extern std::unique_ptr<Lexer>   lexer   ;

const std::unordered_set<std::string> key_word_set
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
	    "procedure",
	    "until"
	 });


const std::unordered_set<std::string> rel_op
	({
		"=","<>", "<", "<=", ">", ">="
	 });
	


const std::array<std::string, 33> err_msg
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


__always_inline
void generate_code(fct OP, int L, int M)
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
	auto pos = code.size();
	generate_code(fct::jmp, 0, 0);
	const_declaration();
	var_declaration();
	procedure_declaration();
	code[pos].M = static_cast<int>(code.size());
	statement();
	delete local_space;
	local_space = prev;
}

void const_declaration()
{
	std::string curr_token ;
	if (lexer->next_token() == "const")
	{
		lexer->get_token();
		curr_token = lexer->get_token();
		if (key_word_set.find(curr_token) == key_word_set.end())
		{
			if (isalpha(curr_token[0]) or curr_token[0] == '_')
			{
				Symbol curr_sym(curr_token);
				curr_sym.type = object::constant;
				curr_sym.level = local_space->get_level();
				curr_token = lexer->get_token();
				if (curr_token == "=")
				{
					curr_token = lexer->get_token();
					/**	可能会抛出 boost::bad_lexical_cast 异常*/
					try
					{
						curr_sym.value = boost::lexical_cast<int>(curr_token);
					}
					catch (boost::bad_lexical_cast& e)
					{
						std::cerr<<e.what()<<std::endl;
						error(2);
					}
					catch (...)
					{
						error(2);
					}
					local_space->add(curr_sym);
					
				} else
				{
					error(3);
				}
			} else
			{
				error(19);
			}
			while (lexer->next_token() == ",")
			{
				curr_token = lexer->get_token();
				curr_token = lexer->get_token();
				if (key_word_set.find(curr_token) == key_word_set.end())
				{
					if (isalpha(curr_token[0]) or curr_token[0] == '_')
					{
						Symbol curr_sym(curr_token);
						curr_sym.type = object::constant;
						curr_sym.level = local_space->get_level();
						curr_token = lexer->get_token();
						if (curr_token == "=")
						{
							curr_token = lexer->get_token();
							curr_sym.value = boost::lexical_cast<int>(curr_token);
							local_space->add(curr_sym);
							
						} else
						{
							error(3);
						}
					} else
					{
						error(19);
					}
				}
				if (lexer->next_token() == ";")
				{
					curr_token = lexer->get_token();
				} else
				{
					error(17);
				}
			}
		}else
		{
			error(19);
		}
	}
}

void var_declaration()
{
	std::string curr_token = lexer->next_token();
	int variable_count = 3;
	if (curr_token == "int" or curr_token == "var" )
	{
		lexer->get_token();
		curr_token = lexer->get_token();
		if (key_word_set.find(curr_token) == key_word_set.end())
		{
			if (isalpha(curr_token[0]) or curr_token[0] == '_')
			{
				Symbol curr_sym(curr_token);
				curr_sym.level = local_space->get_level();
				curr_sym.type = object::variable;
				curr_sym.addr = variable_count;
				local_space->add(curr_sym);
				++variable_count;
				if (lexer->next_token() == ",")
				{
					do
					{
						lexer->get_token();
						curr_token = lexer->get_token();
						if (isalpha(curr_token[0] or curr_token[0] == '_'))
						{
							curr_sym.name = curr_token;
							curr_sym.level = local_space->get_level();
							curr_sym.type = object::variable;
							curr_sym.addr = variable_count;
							local_space->add(curr_sym);
							++variable_count;
						}else
						{
							error(19);
						}
					} while (lexer->next_token() == ",");
					if (lexer->next_token() == ";")
					{
						lexer->get_token();
					} else
					{
						error(17);
					}
				}
			}else
			{
				error(19);
			}
		}else
		{
			error(19);
		}
		generate_code(fct::inc, 0, variable_count);
	}

}

void procedure_declaration()
{
	std::string curr_token;
	if (lexer->next_token() == "procedure")
	{
		
		do
		{
			lexer->get_token();
			curr_token =  lexer->get_token();
			if (key_word_set.find(curr_token) == key_word_set.end())
			{
				if (isalpha(curr_token[0]) or curr_token[0] == '_')
				{
					Symbol curr_prod(curr_token);
					curr_prod.type = object::procedure;
					curr_prod.addr = static_cast<int>(code.size());
					if (lexer->next_token() == ";")
					{
						curr_token = lexer->get_token();
						block();
						if (lexer->next_token() == ";")
						{
							curr_token = lexer->get_token();
							/** 一个过程的在正文区的长度等于 结束位置 - 开始位置*/
							curr_prod.size = static_cast<int>(code.size() - curr_prod.addr);
							local_space->add(curr_prod);
						} else
						{
							error(17);
						}
					} else
					{
						error(17);
					}
				}
			}else
			{
				error(19);
			}
		}while(lexer->next_token() == "procedure");
	}
}

void statement()
{
	std::string curr_token;
	/**	为了避免在if else 选择分支的时间开销，选择使用 hash 散列法将移进符号映射到操作*/
	static std::unordered_map<std::string, std::function<void()>> oper_table
		({
			 {
				"call", [&]
				        {
					        Symbol *procedure = local_space->get(lexer->get_token());
					        if (procedure not_eq nullptr)
					        {
						        generate_code(fct::cal, procedure->level, procedure->addr);
					        } else
					        {
						        error(6);
					        }
				        }
			 },
			 {
				 "begin", [&]
				          {
					          statement();
					          while (lexer->next_token() == ";")
					          {
						          lexer->get_token();
						          statement();
					          }
					          if (lexer->next_token() == "end")
					          {}
					          else error(17);
				          }
			 },
			 {
				 "if",[&]
				      {
					      condition();
					      int cond_false_jmp = 0;
					      int if_finish_jmp = 0;
					      if (lexer->next_token() == "then")
					      {
						      generate_code(fct::jpc, 0, 0);
						      cond_false_jmp = static_cast<int>(code.size() - 1);
						      lexer->get_token();
						      statement();
						      generate_code(fct::jmp, 0, 0);
						      if_finish_jmp = static_cast<int>(code.size() - 1);
					      } else
					      {
						      error(16);
					      }
					      if (lexer->next_token() == "else")
					      {
						      lexer->get_token();
						      code[cond_false_jmp].M = static_cast<int>(code.size() - 1);
						      statement();
					      }
					      code[if_finish_jmp].M = static_cast<int>(code.size());
				      }
			 },
			 {
				 "while", [&]
				          {
					          condition();
					          generate_code(fct::jpc, 0, 0);
					          int first_jmp = static_cast<int>(code.size() - 1);
					          if (lexer->next_token() == "do")
					          {
						          statement();
					          } else
					          {
						          error(18);
					          }
					          generate_code(fct::jmp, 0, first_jmp);
					          code[first_jmp].M = static_cast<int>(code.size());
				          }
				          
			 },
			 {
				 "read", [&]
				         {
					         generate_code(fct::sio, 0, 2);
					         Symbol* variable = local_space->get(lexer->get_token());
					         if (variable not_eq nullptr)
					         {
						         generate_code(fct::sto, variable->level, variable->addr);
					         }else
					         {
						         error(11);
					         }
				         }
			 },
			 {
				 "write", [&]
				          {
					          expression();
					          generate_code(fct::sio, 0, 1);
				          }
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
			generate_code(fct::sto, ident->level, ident->addr);
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
				 "=", [&]{generate_code(fct::opr, 0, 8);}
			 },
			 {
				"<>", [&]{generate_code(fct::opr, 0, 9);}
			 },
			 {
				">", [&]{generate_code(fct::opr, 0, 10);}
			 },
			 {
				">=", [&]{generate_code(fct::opr, 0, 11);}
			 },
			 {
				"<", [&]{generate_code(fct::opr, 0, 12);}
			 },
			 {
				"<=", [&]{generate_code(fct::opr, 0, 13);}
			 }
			 
		 });
	if (lexer->next_token() == "odd")
	{
		curr_token = lexer->get_token();
		expression();
		generate_code(fct::opr, 0, 6);
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
			generate_code(fct::opr, 0, 1);
		}
		while (lexer->next_token() == "+" or lexer->next_token() == "-")
		{
			curr_token = lexer->get_token();
			term();
			switch (curr_token[0])
			{
				case '+':
					generate_code(fct::opr, 0, 2);
					break;
				case '-':
					generate_code(fct::opr, 0, 3);
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
					generate_code(fct::opr, 0, 2);
					break;
				case '-':
					generate_code(fct::opr, 0, 3);
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
				generate_code(fct::opr, 0, 4);
				break;
			case '/':
				generate_code(fct::opr, 0, 5);
				break;
			case '%':
				generate_code(fct::opr, 0, 7);
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
		generate_code(fct::lit, 0, num);
	}
	else if ((symbol = local_space->get(curr_token)) not_eq nullptr)
	{
		
		switch (symbol->type)
		{
			case object::constant :
				generate_code(fct::lit, 0, symbol->value);
				break;
			case object::variable :
				generate_code(fct::lod, symbol->level, symbol->addr);
				break;
			case object::array :
				// TODO here is going to implement
				break;
			case object::procedure :
				error(21);
				break;
			default:
				break;
		}
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

