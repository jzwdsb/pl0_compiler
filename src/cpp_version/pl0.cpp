//
// Created by manout on 17-11-5.
//

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <set>
#include "pl0.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include "Lexer.h"


/**	定义全局词法分析器*/
extern std::unique_ptr<Scanner> scanner ;
extern std::unique_ptr<Lexer>   lexer   ;

const std::unordered_set<std::string> key_word_set
	{
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
	 };


const std::unordered_set<std::string> rel_op
	{
		"=","<>", "<", "<=", ">", ">="
	 };


const std::array<std::string, 9> OP_STR =
	{
		"lit",
		"opr",
		"lod",
		"sto",
		"cal",
		"inc",
		"jmp",
		"jpc",
		"sio"
	};

const std::array<std::string, 33> err_msg
	{
		/*  0*/      "",
		/*  1*/      "Found ':=' when expecting '='",
		/*  2*/      "There must be a number to follow",
		/*  3*/      "There must be an '=' or '[]' to follow the identifier",
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

/**	顶层命名空间, 好像没用到*/
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

__always_inline
bool isnum(const std::string& str)
{
	if (str.length() == 0)
	{
		return false;
	}
	for (auto ch : str)
	{
		if (not isdigit(ch))
		{
			return false;
		}
	}
	return true;
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
	array_declaration();
	auto pos = code.size();
	generate_code(fct::jmp, 0, 0);
	procedure_declaration();
	code[pos].M = static_cast<int>(code.size());
	statement();
	generate_code(fct::opr, 0, 0);
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
		if (key_word_set.count(curr_token) == 0)
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
					/**	可能会抛出 std::invalid_argument 异常*/
					try
					{
						curr_sym.value = std::stoi(curr_token);
					}
					catch (std::invalid_argument& e)
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
				if (key_word_set.count(curr_token) == 0)
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
							/** 这里可能亏抛出　std::invalid_argument 异常*/
							try
							{
								curr_sym.value = std::stoi(curr_token);
							}
							catch (std::invalid_argument& e)
							{
								std::cerr << e.what();
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
	int variable_count = 0;
	if (curr_token == "int" or curr_token == "var" )
	{
		lexer->get_token();
		curr_token = lexer->get_token();
		if (key_word_set.count(curr_token) == 0)
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
						if (isalpha(curr_token[0]) or curr_token[0] == '_')
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

void array_declaration()
{
	std::string curr_token;
	int base_address = 0;
	if (lexer->next_token() == "array")
	{
		lexer->get_token(); /** consume one token*/
		curr_token = lexer->get_token();
		if(key_word_set.count(curr_token) == 0)
		{
			if(isalpha(curr_token[0]) or curr_token[0] == '_')
			{
				
				Symbol curr_symbol(curr_token);
				curr_symbol.type = object::array;
				curr_symbol.level = local_space->get_level();
				curr_symbol.addr = base_address;
				if (lexer->next_token() not_eq "[")
				{
					try
					{
						curr_symbol.size =std::stoi(lexer->get_token());
						base_address += curr_symbol.size;
					}
					catch (std::invalid_argument& e)
					{
						std::cerr << e.what();
						error(2);
					}
					catch (...)
					{
						error(2);
					}
				}else
				{
					error(3);
				}
				
			}else
			{
				error(19);
			}
		}else
		{
			error(19);
		}
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
			if (key_word_set.count(curr_token) == 0)
			{
				if (isalpha(curr_token[0]) or curr_token[0] == '_')
				{
					Symbol curr_prod(curr_token);
					curr_prod.type = object::procedure;
					curr_prod.addr = static_cast<int>(code.size());
					curr_prod.level = local_space->get_level();
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
							/** 在　procedure 的最后加上过程返回语句*/
							generate_code(fct::opr, 0, 0);
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
		{
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
						          /** cost ;*/
						          lexer->get_token();
						          statement();
					          }
					          if (lexer->next_token() == "end")
					          {
						          /** cost end*/
						          lexer->get_token();
						          return ;
					          }
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
						          lexer->get_token();
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
		};
	
	if (oper_table.count(curr_token) not_eq 0 )
	{
		curr_token = lexer->get_token();
		oper_table[curr_token]();
		return ;
	}
 	if (local_space->get(lexer->next_token()) not_eq nullptr)
	{
		// 获取标志符
		curr_token = lexer->get_token();
		
		Symbol* ident = local_space->get(curr_token);
		
		if (lexer->next_token() == ":=")
		{
 			curr_token = lexer->get_token();
 			expression();
 			generate_code(fct::sto, local_space->get_level() - ident->level, ident->addr);
		}
		else
		{
			error(13);
		}
	}
	/** statement 可以直接推导为空串，　到这直接退出即可*/
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
		if (rel_op.count(lexer->next_token()) == 0)
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


void factor()
{
	std::string curr_token;
	Symbol* symbol = nullptr;
	
	curr_token = lexer->get_token();
	if (curr_token == "(")
	{
		curr_token = lexer->get_token();
		expression();
		curr_token = lexer->get_token();
		if (curr_token not_eq ")")
		{
			error(22);
		}
		return ;
	}
	if (isnum(curr_token))
	{
		int num = std::stoi(curr_token);
		generate_code(fct::lit, 0, num);
		return ;
	}
	if ((symbol = local_space->get(curr_token)) not_eq nullptr)
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

std::vector<std::string> code_to_str()
{
	std::vector<std::string> code_str;
	std::for_each(code.begin(), code.end(),
		          [&](instruction ir)
		          {code_str.emplace_back((boost::format("%1% %2% %3%") %OP_STR[ir.OP] %ir.L % ir.M).str());});
	return code_str;
}

void show_code()
{
	std::vector<std::string> code_str = code_to_str();
	int code_line = 0;
	std::for_each(code_str.begin(), code_str.end(),
	              [&](std::string p_code)
	              {
		              std::cout<<code_line << ':' <<p_code<<std::endl;
		              ++code_line;
	              });
}