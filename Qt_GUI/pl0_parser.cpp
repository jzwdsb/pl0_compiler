#include "pl0_parser.h"

#include <functional>

const QSet<QString> key_word_set
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

const QSet<QString> rel_op
    {
        "=","<>", "<", "<=", ">", ">="
     };

const QString OP_STR [] =
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



#define generate_code(OP, L, M)   code.push_back(instruction((OP), (L), (M)))

__always_inline
bool isnum(const QString& str)
{
    bool ok = false;
    str.toInt(&ok, 10);
    return ok;
}

pl0_parser::pl0_parser():QObject(),local_space(nullptr)
{
    this->lexer = new Lexer();
    connect(this->lexer, SIGNAL(lexError(int ,int)), this, SIGNAL(error(int, int)));
}

pl0_parser::pl0_parser(QString &filename):QObject(), local_space(nullptr)
{
    this->lexer = new Lexer();
    this->setCodeSource(filename);
}

pl0_parser::~pl0_parser()
{
    this->clear();
}

void pl0_parser::program()
{
    QString curr_token;
    block();
    if (lexer->next_token() == tr("."))
    {
        curr_token = lexer->get_token();
        return ;
    }
    else
    {
        emit error(lexer->rows(),9);
    }
}

void pl0_parser::block()
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

void pl0_parser::const_declaration()
{
    QString curr_token ;
    if (lexer->next_token() == tr("const"))
    {
        lexer->get_token();
        do
        {
            curr_token = lexer->get_token();
            if (not key_word_set.contains(curr_token))
            {
                if (curr_token[0].isLetter() or curr_token[0] == '_')
                {
                    Symbol curr_sym(curr_token);
                    curr_sym.type = object::constant;
                    curr_sym.level = local_space->get_level();
                    curr_token = lexer->get_token();
                    if (curr_token == tr("="))
                    {
                        curr_token = lexer->get_token();
                        bool ok = false;
                        curr_sym.value = curr_token.toInt(&ok, 10);
                        if (not ok)
                        {
                            emit error(lexer->rows(), 2);
                            return;
                        }
                        local_space->add(curr_sym);

                    } else
                    {
                        emit error(lexer->rows(), 3);
                    }
                } else
                {
                    emit error(lexer->rows(), 19);
                }
            }else
            {
                emit error(lexer->rows(), 19);
            }
        }while (lexer->next_token() == "," and lexer->get_token() == ",");
        if (lexer->next_token() == ";")
        {
            lexer->get_token();
        }
        else
        {
            emit error(lexer->rows(), 5);
        }
    }
}

void pl0_parser::var_declaration()
{
    QString curr_token = lexer->next_token();
    int variable_count = 0;
    base_address = 3;
    if (curr_token == tr("int") or curr_token == tr("var"))
    {
        lexer->get_token();
        do
        {
            curr_token = lexer->get_token();
            if (not key_word_set.contains(curr_token))
            {
                if (curr_token[0].isLetter() or curr_token[0] == '_')
                {
                    Symbol curr_sym(curr_token);
                    curr_sym.level = local_space->get_level();
                    curr_sym.type = object::variable;
                    curr_sym.addr = base_address;
                    local_space->add(curr_sym);
                    ++variable_count;
                    ++base_address;
                }else
                {
                    emit error(lexer->rows(), 19);
                }
            }else
            {
                emit error(lexer->rows(), 19);
            }
        }while (lexer->next_token() == "," and lexer->get_token() == ",");
        if (lexer->next_token() == ";")
        {
            lexer->get_token();
        }else
        {
            emit error (lexer->rows(), 5);
        }
        generate_code(fct::inc, 0, variable_count);
    }
}

void pl0_parser::array_declaration()
{
    QString curr_token;
    int size_count = 0;
    if (lexer->next_token() == "array")
    {
        lexer->get_token(); /** consume one token*/
        do
        {
            curr_token = lexer->get_token();
            if ( not key_word_set.contains(curr_token))
            {
                if (curr_token[0].isLetter() or curr_token[0] == '_')
                {
                    Symbol curr_symbol(curr_token);
                    curr_symbol.type = object::array;
                    curr_symbol.level = local_space->get_level();
                    curr_symbol.addr = base_address;
                    if (lexer->next_token() == "[")
                    {
                        bool ok = false;
                        lexer->get_token();
                        curr_token = lexer->get_token();
                        curr_symbol.size = curr_token.toInt(&ok, 10);
                        if (not ok)
                        {
                            emit error(lexer->rows(), 2);
                        }
                        local_space->add(curr_symbol);
                        base_address += curr_symbol.size;
                        size_count += curr_symbol.size;
                        if (lexer->next_token() == "]")
                        {
                            lexer->get_token();
                        } else
                        {
                            emit error(lexer->rows(), 26);
                        }
                    } else
                    {
                        emit error(lexer->rows(), 3);
                    }
                } else
                {
                    emit error(lexer->rows(), 19);
                }
            } else
            {
                emit error(lexer->rows(), 19);
            }
        } while (lexer->next_token() == "," and lexer->get_token() == ",");
        if (lexer->next_token() == ";")
        {
            lexer->get_token();
        } else
        {
            emit error(lexer->rows(), 5);
        }
        generate_code(fct::inc, 0, size_count);
    }
}

void pl0_parser::procedure_declaration()
{
    QString curr_token;
    while (lexer->next_token() == tr("procedure"))
    {
        lexer->get_token();
        curr_token =  lexer->get_token();
        if (key_word_set.find(curr_token) == key_word_set.end())
        {
            if (curr_token[0].isLetter() or curr_token[0] == '_')
            {
                Symbol curr_prod(curr_token);
                curr_prod.type = object::procedure;
                curr_prod.addr = static_cast<int>(code.size());
                curr_prod.level = local_space->get_level();
                if (lexer->next_token() == ";")
                {
                    curr_token = lexer->get_token();
                    block();
                    if (lexer->next_token() == tr(";"))
                    {
                        curr_token = lexer->get_token();
                        /** 一个过程的在正文区的长度等于 结束位置 - 开始位置*/
                        curr_prod.size = static_cast<int>(code.size() - curr_prod.addr);
                        local_space->add(curr_prod);
                    }
                    }else
                    {
                       emit error(lexer->rows(), 17);
                    }
                } else
                {
                    emit error(lexer->rows(), 17);
                }
        }else
        {
            emit error(lexer->rows(), 19);
        }
    }
}

void pl0_parser::statement()
{
    QString curr_token;
    /**	为了避免在if else 选择分支的时间开销，选择使用 hash 散列法将移进符号映射到操作*/
    static std::unordered_map<std::string, std::function<void()>> oper_table
	    {
		    {
			    "call",  [&]
			             {
				             Symbol *procedure = local_space->get(lexer->get_token());
                             if (procedure not_eq nullptr and procedure->type == object::procedure)
				             {
                                 generate_code(fct::cal, local_space->get_level() -  procedure->level, procedure->addr);
				             } else
				             {
                                 emit error(lexer->rows(),  6);
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
					             return;
                             } else
                             {
                                 emit error(lexer->rows(), 17);
                             }
			             }
		    },
		    {
			    "if",    [&]
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
                                 emit error(lexer->rows(), 16);
				             }
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
                                 emit error(lexer->rows(), 18);
				             }
                             generate_code(fct::jmp, 0, jmp_back_pos);
				             code[first_jmp].M = static_cast<int>(code.size());
			             }
			
		    },
		    {
			    "read",  [&]
			             {
				             generate_code(fct::sio, 0, 2);
				             Symbol *variable = local_space->get(lexer->get_token());
                             if (variable not_eq nullptr and variable->type == object::variable)
				             {
                                 generate_code(fct::sto, local_space->get_level() - variable->level, variable->addr);
				             } else
                             {
                                 emit error(lexer->rows(), 11);
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
            {
                "for",  [&]
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
                                                emit error(lexer->rows(), 17);
                                            }
                                        } else
                                        {
                                            emit error(lexer->rows(), 17);
                                        }
                                    }else
                                    {
                                        emit error(lexer->rows(), 22);
                                    }
                                }
                        }
            };

    if (oper_table.find(lexer->next_token().toStdString()) not_eq oper_table.end())
    {
        curr_token = lexer->get_token();
        oper_table[curr_token.toStdString()]();
        return ;
    }
    if (local_space->get(lexer->next_token()) not_eq nullptr)
    {
        // 获取标志符
        curr_token = lexer->get_token();

        Symbol* ident = local_space->get(curr_token);
        static QHash<QString, int> assign_op
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
                    return;
                }
                if (assign_op.contains(lexer->next_token()))
                {
                    curr_token = lexer->get_token();
                    generate_code(fct::lod, local_space->get_level() - ident->level, ident->addr);
                    expression();
                    generate_code(fct::opr, 0, assign_op[curr_token]);
                    generate_code(fct::sto, local_space->get_level() - ident->level, ident->addr);
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
                            return;
                        }
                        if (assign_op.contains(lexer->next_token()))
                        {
                            curr_token = lexer->next_token();
                            generate_code(fct::lda, local_space->get_level() - ident->level, ident->addr);
                            expression();
                            generate_code(fct::opr, 0, assign_op[curr_token]);
                            generate_code(fct::sta, local_space->get_level() - ident->level, ident->level);
                            return;
                        }
                    }else
                    {
                        emit error(lexer->rows(), 26);
                    }
                }else
                {
                    emit error(lexer->rows(), 3);
                }
                break;
            default:
                emit error(lexer->rows(), 8);
        }
    }
    /** statement 可以直接推导为空串，　到这直接退出即可*/
}

void pl0_parser::condition()
{
    QString curr_token;
    static std::unordered_map<std::string, int> relation_OP (
	    {
            { "=", 8}, {"<>", 9}, {">", 10}, {">=", 11}, {"<", 12}, {"<=", 13}
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
        if (rel_op.contains(lexer->next_token()))
        {
            curr_token = lexer->get_token();
            expression();
            generate_code(fct::opr, 0, relation_OP[curr_token.toStdString()]);
        }
        else
        {
            emit error(lexer->rows(), 20);
        }
    }
}

void pl0_parser::expression()
{
    QString curr_token;
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
            switch (curr_token[0].toLatin1())
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

        }while (lexer->next_token() == "+" or lexer->next_token() == "-");
    }
}

void pl0_parser::term()
{
    QString curr_token;
    factor();
    while (lexer->next_token() == "*" or lexer->next_token() == "/" or lexer->next_token() == "%")
    {
        curr_token = lexer->get_token();
        factor();
        switch (curr_token[0].toLatin1())
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


void pl0_parser::factor()
{
    QString curr_token;
    Symbol* symbol = nullptr;

    curr_token = lexer->get_token();
    if (curr_token == "(")
    {
        curr_token = lexer->get_token();
        expression();
        curr_token = lexer->get_token();
        if (curr_token not_eq ")")
        {
            emit error(lexer->rows(), 22);
        }
        return ;
    }
    else if (isnum(curr_token))
    {
        generate_code(fct::lit, 0, curr_token.toInt());
        return ;
    }
    else if ((symbol = local_space->get(curr_token)) not_eq nullptr)
    {

        switch (symbol->type)
        {
            case object::constant :
                generate_code(fct::lit, 0, symbol->value);
                break;
            case object::variable :
                generate_code(fct::lod, local_space->get_level() - symbol->level, symbol->addr);
                break;
            case object::array :
                if (lexer->next_token() == "[")
                {
                    lexer->get_token();
                    expression();
                    if (lexer->next_token() == "]")
                    {
                        lexer->get_token();
                        generate_code(fct::lda, local_space->get_level() - symbol->level, symbol->addr);
                    }else
                    {
                        emit error(lexer->rows(), 26);
                    }
                }else
                {
                    emit error(lexer->rows(), 3);
                }
                break;
            case object::procedure :
                emit error(lexer->rows(), 21);
                break;
            default:
                break;
        }
    }
    else
    {
        emit error(lexer->rows(), 11);
    }
}

void pl0_parser::close()
{
    if (this->lexer not_eq nullptr)
    {
        this->lexer->close();
    }
    this->code.clear();
}

void pl0_parser::clear()
{
    this->lexer->close();
    delete this->lexer;
    this->lexer = nullptr;
    delete this->local_space;
    this->local_space = nullptr;
}

const QList<QString>* pl0_parser::codeToStr()
{
    for (instruction ins : code)
    {
        codeStr.append(tr("%1 %2 %3\n").arg(OP_STR[ins.OP]).arg(ins.L).arg(ins.M));
    }

    return &this->codeStr;
}

const QList<instruction> *pl0_parser::getCode()
{
    return &this->code;
}

void pl0_parser::setCodeSource(const QString &filename)
{
    this->close();
    this->lexer->setFile(filename);
}

void pl0_parser::parserCode()
{
    this->program();
}
