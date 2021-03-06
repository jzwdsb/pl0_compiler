//
// Created by manout on 17-11-5.
//

#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include <set>
#include "pl0.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include "Lexer.h"

/**	引用全局词法分析器*/
extern Scanner* scanner ;
extern Lexer*   lexer   ;

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
	    "until",
	    "sqrt",
	    "array"
	 };


const std::unordered_set<std::string> rel_op
	{
		"=","<>", "<", "<=", ">", ">="
	 };


const std::array<std::string, 11> OP_STR =
	{
		"lit",
		"opr",
		"lod",
		"lda",
		"sto",
		"sta",
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
		/* 22*/      "Missing ')'. '('",
		/* 23*/      "The symbol can not be followed by an factor",
		/* 24*/      "The symbol can not be as the beginning of an expression",
		/* 25*/      "Invalid Instruction",
		/* 26*/      "Missing ']' after the array declaration",
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

/** 变量的基地址，在生成符号表里使用，　var-declaration 和　array-declaration 使用同一段栈空间所以需要全局变量通信*/
int base_address = 3;

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
	block();
	if (lexer->next_token() == ".")
	{
		lexer->get_token();
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
	/** reset base address*/
    base_address = 3;
	char_declaration();
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

void char_declaration()
{
	std::string curr_token;
	int char_count = 0;
	if (lexer->next_token() == "char")
	{
		lexer->get_token();
		do
		{
			curr_token = lexer->get_token();
			if (key_word_set.count(curr_token) == 0)
			{
				if (isalpha(curr_token[0]) or curr_token[0] == '_')
				{
					Symbol curr_symbol(curr_token);
					curr_symbol.level = local_space->get_level();
					curr_symbol.type = object::inchar;
					curr_symbol.addr = base_address;
					++base_address;
					++char_count;
					local_space->add(curr_symbol);
				}else
				{
					error(19);
				}
			}else
			{
				error(19);
			}
		}while (lexer->next_token() == "," and lexer->get_token() == ",");
		if (lexer->next_token() == ";")
		{
			lexer->get_token();
		}else
		{
			error(5);
		}
	}
}

void const_declaration()
{
	std::string curr_token ;
	if (lexer->next_token() == "const")
	{
		lexer->get_token();
		do
		{
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
						catch (std::invalid_argument &e)
						{
							std::cerr << e.what() << std::endl;
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
			}else
			{
				error(19);
			}
		}while (lexer->next_token() == "," and lexer->get_token() == ",");
		if (lexer->next_token() == ";")
		{
			lexer->get_token();
		}else
		{
			error(5);
		}
	}
}

void var_declaration()
{
	std::string curr_token ;
	int variable_count = 0;
	if (lexer->next_token() == "int" or lexer->next_token() == "var")
	{
		lexer->get_token();
		do
		{
			curr_token = lexer->get_token();
			if (key_word_set.count(curr_token) == 0)
			{
				if (isalpha(curr_token[0]) or curr_token[0] == '_')
				{
					Symbol curr_sym(curr_token);
					curr_sym.level = local_space->get_level();
					curr_sym.type = object::variable;
					curr_sym.addr = base_address;
					local_space->add(curr_sym);
					++variable_count;
					++base_address;
				} else
				{
					error(19);
				}
			} else
			{
				error(19);
			}
		} while (lexer->next_token() == "," and lexer->get_token() == ",");
		if (lexer->next_token() == ";")
		{
			lexer->get_token(); /** consume ';' token*/
		} else
		{
			error(5);
		}
		generate_code(fct::inc, 0, variable_count);
	}
}

void array_declaration()
{
	std::string curr_token;
	int size_count = 0;
	if (lexer->next_token() == "array")
	{
		/** consume one token*/
		lexer->get_token();
		do
		{
			curr_token = lexer->get_token();
			if (key_word_set.count(curr_token) == 0)
			{
				if (isalpha(curr_token[0]) or curr_token[0] == '_')
				{
					
					Symbol curr_symbol(curr_token);
					curr_symbol.type = object::array;
					curr_symbol.level = local_space->get_level();
					curr_symbol.addr = base_address;
					if (lexer->next_token() == "[")
					{
						lexer->get_token();
						try
						{
							curr_symbol.size = std::stoi(lexer->get_token());
						}
						catch (std::invalid_argument &e)
						{
							std::cerr << e.what();
							error(2);
						}
						catch (...)
						{
							error(2);
						}
						local_space->add(curr_symbol);
						base_address += curr_symbol.size;
						size_count += curr_symbol.size;
						if (lexer->next_token() == "]")
						{
							lexer->get_token();
						} else
						{
							error(26);
						}
					} else
					{
						error(3);
					}
					
				} else
				{
					error(19);
				}
			} else
			{
				error(19);
			}
		} while (lexer->next_token() == "," and lexer->get_token() == ",");
		if (lexer->next_token() == ";")
		{
			lexer->get_token();
		} else
		{
			error(5);
		}
		generate_code(fct::inc, 0, size_count);
	}
}

void procedure_declaration()
{
	std::string curr_token;
	while  (lexer->next_token() == "procedure")
	{
		lexer->get_token();
		curr_token = lexer->get_token();
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
					} else
					{
						error(17);
					}
				} else
				{
					error(17);
				}
			}
		} else
		{
			error(19);
		}
		
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
					        if (procedure not_eq nullptr and procedure->type == object::procedure)
					        {
						        generate_code(fct::cal,local_space->get_level() -  procedure->level, procedure->addr);
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
						      cond_false_jmp = static_cast<int>(code.size());
						      generate_code(fct::jpc, 0, 0);
						      lexer->get_token();
						      statement();
						      if_finish_jmp = static_cast<int>(code.size());
						      generate_code(fct::jmp, 0, 0);
					      } else
					      {
						      error(16);
					      }
					      /** here is one of the expand part, 'else' branch*/
					      if (lexer->next_token() == "else")
					      {
						      lexer->get_token();
						      code[cond_false_jmp].M = static_cast<int>(code.size());
						      statement();
					      }
					      else
					      {
						      code[cond_false_jmp].M = static_cast<int>(code.size());
					      }
					      code[if_finish_jmp].M = static_cast<int>(code.size());
				      }
			 },
			 {
				 "while", [&]
				          {
					          int jmp_back_pos = static_cast<int>(code.size());
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
					          generate_code(fct::jmp, 0, jmp_back_pos);
					          code[first_jmp].M = static_cast<int>(code.size());
				          }
				          
			 },
			 {
				 "read", [&]
				         {
					         Symbol* variable = local_space->get(lexer->get_token());
					         if (variable not_eq nullptr)
					         {
						         switch (variable->type)
						         {
							         case object::variable:
								         generate_code(fct::sio, 0, 2);
								         generate_code(fct::sto, local_space->get_level() - variable->level, variable->addr);
								         break;
							         case object::inchar:
								         generate_code(fct::sio, 1, 2);
								         generate_code(fct::sto, local_space->get_level() - variable->level, variable->addr);
								         break;
							         default:
								         error(11);
								         break;
						         }
					         }
				         }
			 },
			 {
				 "write", [&]
				          {
					          
					          if (local_space->get(lexer->next_token())->type == object::inchar)
					          {
						          Symbol* ident = local_space->get(lexer->get_token());
						          generate_code(fct::lod, local_space->get_level() - ident->level, ident->addr);
						          generate_code(fct::sio, 1, 1);
					          }
					          expression();
					          generate_code(fct::sio, 0, 1);
				          }
			 },
			 {
				 "for", [&]
				        {
					        if (lexer->next_token() == "(")
					        {
						
						
						        statement();
						        int cond_false_jmp = 0;
						        int continue_jmp = 0;
						        int state_finish_jmp = 0;
						        int update_jmp = 0;
						        if (lexer->next_token() == ";")
						        {
							        lexer->get_token();
							        update_jmp = static_cast<int>(code.size());
							        condition();
							        cond_false_jmp = static_cast<int>(code.size());
							        generate_code(fct::jpc, 0, 0);
							        continue_jmp = static_cast<int>(code.size());
							        generate_code(fct::jmp, 0, 0);
							        if (lexer->next_token() == ";")
							        {
								        lexer->get_token();
								        state_finish_jmp = static_cast<int>(code.size());
								        statement();
								        generate_code(fct::jmp, 0, update_jmp);
								        if (lexer->next_token() == ")")
								        {
									        lexer->get_token();
									        code[continue_jmp].M = static_cast<int>(code.size());
									        statement();
									        generate_code(fct::jmp, 0, state_finish_jmp);
									        code[cond_false_jmp].M = static_cast<int>(code.size());
								        }
							        } else
							        {
								        error(17);
							        }
						        } else
						        {
							        error(17);
						        }
					        }else
					        {
						        error(22);
					        }
				        }
			 }
		};
	
	
	/** if next token in the first set of statement, so get into corresponding branch to do some work*/
	if (oper_table.count(lexer->next_token()) not_eq 0 )
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
		static std::unordered_map<std::string, int> assign_op
			{
				{"+=", 2}, {"-=", 3}, {"*=", 4}, {"/=", 5}, {"%=", 7}
			};
		
		switch (ident->type)
		{
			case object::variable:
				if (lexer->next_token() == ":=")
				{
					lexer->get_token();
					expression();
					generate_code(fct::sto, local_space->get_level() - ident->level, ident->addr);
					return ;
				}
				if (assign_op.count(lexer->next_token()) == 1)
				{
					curr_token = lexer->get_token();
					generate_code(fct::lod, local_space->get_level() - ident->level, ident->addr);
					expression();
					generate_code(fct::opr, 0, assign_op[curr_token]);
					generate_code(fct::sto, local_space->get_level() - ident->level, ident->addr);
					return ;
				}else
				{
					error(13);
				}
				break;
			case object::array:
				if (lexer->next_token() == "[")
				{
					lexer->get_token();
					expression();
					if (lexer->next_token() == "]")
					{
						lexer->get_token();
						if (lexer->next_token() == ":=")
						{
							lexer->get_token();
							expression();
							generate_code(fct::sta, local_space->get_level() - ident->level, ident->addr);
							return ;
						}
						if (assign_op.count(lexer->next_token()) == 1)
						{
                            /**  In the runtime, here will run into a unexpected action.
                             * the offset is already consumed, so the store operation can't find
                             * the right address of the array element
                             * */
							curr_token = lexer->get_token();
							generate_code(fct::lda, local_space->get_level() - ident->level, ident->addr);
							expression();
							generate_code(fct::opr, 0, assign_op[curr_token]);
							generate_code(fct::sta, local_space->get_level() - ident->level, ident->addr);
							return ;
						}else
						{
							error(13);
						}
					
					}else
					{
						error(26);
					}
				} else
				{
					error(3);
				}
				break;
			default:
				error(8);
		}
	}
	/** statement can be derived to an empty string, just return when reach here*/
}

void condition()
{
	std::string curr_token;
	static std::unordered_map<std::string, int> relation_OP
		({ { "=", 8 }, {"<>", 9}, {">", 10}, {">=", 11}, {"<", 12},{"<=", 13}});
	if (lexer->next_token() == "odd")
	{
		curr_token = lexer->get_token();
		expression();
		generate_code(fct::opr, 0, 6);
	}
	else
	{
		expression();
		if (rel_op.count(lexer->next_token()) == 1)
		{
			curr_token = lexer->get_token();
			expression();
			generate_code(fct::opr, 0, relation_OP[curr_token]);
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
		
	}
	else
	{
		term();
	}
	if (lexer->next_token() == "+" or lexer->next_token() == "-")
	{
		do
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
		} while (lexer->next_token() == "+" or lexer->next_token() == "-");
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
	Symbol* ident = nullptr;
	std::unordered_map<std::string, int> self_op_code
		{
			{"++", 2}, {"--", 3}
		};
	if (lexer->next_token() == "++" or lexer->next_token() == "--")
	{
		std::string self_op = lexer->get_token();
		curr_token = lexer->get_token();
		if ((ident = local_space->get(curr_token)) not_eq nullptr )
		{
			switch (ident->type)
			{
				case object::variable:
					generate_code(fct::lod, local_space->get_level() - ident->level, ident->addr);
					generate_code(fct::lit, 0, 1);
					generate_code(fct::opr, 0, self_op_code[self_op]);
					generate_code(fct::sto, local_space->get_level() - ident->level, ident->addr);
					break;
					
				/** TODO, the pl0 stack model can't implement the self operate on array element*/
				case object::array:
					/*
					if (lexer->next_token() == "[")
					{
						lexer->get_token();
						expression();
						if (lexer->next_token() == "]")
						{
							lexer->get_token();
							generate_code(fct::lda, local_space->get_level() - ident->level, ident->addr);
							generate_code(fct::lit, 0, 1);
							generate_code(fct::opr, 0, self_op_code[self_op]);
							generate_code(fct::sta, local_space->get_level() - ident->level, ident->addr);
						}else
						{
							error(26);
						}
					}else
					{
						error(3);
					}
					 */
					break;
				default:
					error(8);
			}
			
		}else
		{
			error(11);
		}
		return ;
	}
	
	curr_token = lexer->get_token();
	if (curr_token == "(")
	{
		lexer->get_token();
		expression();
		if (lexer->next_token() not_eq ")")
		{
			error(22);
		}
		return ;
	}
	if (isnum(curr_token))
	{
		/** 	isnum condition judge ensure that
		 *  stoi below wont't throw a exception*/
		generate_code(fct::lit, 0, std::stoi(curr_token));
		return ;
	}
	if ((ident = local_space->get(curr_token)) not_eq nullptr)
	{
		switch (ident->type)
		{
			case object::constant :
				generate_code(fct::lit, 0, ident->value);
				break;
			case object::variable :
				generate_code(fct::lod, local_space->get_level() - ident->level, ident->addr);
				break;
			case object::array :
				if (lexer->next_token() == "[")
				{
					lexer->get_token();
					expression();
					if (lexer->next_token() == "]")
					{
						lexer->get_token();
						generate_code(fct::lda, local_space->get_level() - ident->level, ident->addr);
					}else
					{
						error(26);
					}
				}else
				{
					error(3);
				}
				break;
			case object::procedure :
				error(21);
				break;
			default:
				error(8);
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

void save_code()
{
	std::ofstream of("/home/manout/pl0_compiler/asm_result", std::fstream::out);
	std::string content;
	std::vector<std::string> codeStr = code_to_str();
	for (std::string& line : codeStr)
	{
		content.append(line);
		content.push_back('\n');
	}
	of << content;
	of.close();
}