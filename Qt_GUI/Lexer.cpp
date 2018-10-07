//
// Created by manout on 17-11-4.
//

#include "Lexer.h"

/**
 * 为了方便而声明的操作符字符串，同时也是为了以后的词法分析方便扩展
 * 包含所有操作符所使用的字符
 */
const QString operator_string("+-*/%:=<>()[]");


/** 为了方便而声明的分隔符字符串，包含所有分隔符用到的字符*/
const QString delimiter_string(",;.");

Lexer::Lexer()
{
    this->scanner = new Scanner();
    this->row = 1;
}

Lexer::~Lexer()
{
    this->close();
    delete this->scanner;
}

void Lexer::close()
{
    if(this->scanner not_eq nullptr)
    {
        this->scanner->close();
    }
    this->token_table.clear();
}

void Lexer::clear()
{
    if (this->scanner not_eq nullptr)
    {
        this->scanner->close();
        delete scanner;
    }
}

void Lexer::setFile(const QString &filename)
{
    this->row = 1;
    this->close();
    this->scanner->setFile(filename);
    token_table.clear();
    this->prepare();
}

/** 该函数取出下一个词素并返回*/
QString Lexer::get_token()
{
	prepare();
    QString curr_token;
    if (not token_table.isEmpty())
	{
        curr_token = token_table.head();
        token_table.dequeue();
	}
	return curr_token;
}

/** 该函数返回下一个词素但是不取出 */
const QString& Lexer::next_token()
{
	prepare();
    return token_table.head();
}

#define  jump_blank  while (ch.isSpace() and ch not_eq '\n'){ ch = scanner->readChar();}

void Lexer::prepare()
{
	while (token_table.empty() and not scanner->isEof())
	{
        QChar ch = 0;
        QString curr_word;
        ch = scanner->readChar();
		/**  一次处理一行
		    遍历一行主要寻找的模式有
		    标识符
		    字面量
		    运算符
		    分隔符
		*/
		while (ch not_eq '\n')
		{
            jump_blank
			/** 遇到以字母或下划线开头的单词*/
            if (ch.isLetter() or ch == '_')
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
                    ch = scanner->readChar();
                }while ((ch.isLetter() or ch.isDigit() or ch == '_') and not  this->scanner->isEof());
                token_table.enqueue(curr_word);
			}
			
            jump_blank
			/** 匹配数字字面量*/
            if (ch.isDigit())
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
                    ch = scanner->readChar();
                }while (ch.isDigit() and not scanner->isEof());
                token_table.enqueue(curr_word);
			}
			
            jump_blank
			/** 匹配运算符*/
            if (operator_string.contains(ch) )
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
                    ch = scanner->readChar();
                }while (operator_string.contains(ch) and not scanner->isEof());
                token_table.enqueue(curr_word);
			}
			
            jump_blank
			/** 匹配分隔符*/
            if (delimiter_string.contains(ch))
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
                    ch = scanner->readChar();
                }while (delimiter_string.contains(ch) and not scanner->isEof());
                token_table.enqueue(curr_word);
			}
		}
        ++this->row;
		/*
		//先按空白字符划分，之后再按分隔符划分
		boost::split(words, *current_line, boost::is_any_of(R"( \n\t,)"));
		/*boost::split(words, *current_line,
			std::bind((bool (const std::string&,const std::regex&,std::regex_constants::match_flag_type))
				          std::regex_match,std::regex(R"(\s,)"),std::placeholders::_2));
		for (const std::string word:words)
		{
			auto find_seq = std::find_if(word.begin(), word.end(), boost::is_any_of(",.;():"));
			if (find_seq not_eq word.end())
			{
			
			}
		}
		for ( std::string& word : words)
		{
			token_table.push(std::move(word));
		}
		delete current_line;
		*/
	}
}

bool Lexer::isEof()
{
    return scanner->isEof() and token_table.isEmpty();
}

int Lexer::rows()
{
    return this->row;
}


#undef jump_blank
