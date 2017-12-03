//
// Created by manout on 17-11-4.
//

#include "Lexer.h"

/**
 * 为了方便而声明的操作符字符串，同时也是为了以后的词法分析方便扩展
 * 包含所有操作符所使用的字符
 */
const std::string operator_string("+-*/%:=<>()");


/** 为了方便而声明的分隔符字符串，包含所有分割符用到的字符*/
const std::string delimiter_string(",;.");

Lexer::Lexer(Scanner* scer):scanner(scer),token_table()
{
	prepare();
}

/** 该函数取出下一个词素并返回*/
std::string Lexer::get_token()
{
	prepare();
	std::string curr_token;
	if (not scanner->isEof())
	{
		curr_token = token_table.front();
		token_table.pop();
	}
	
	return curr_token;
}

/** 该函数返回下一个词素但是不取出 */
const std::string & Lexer::next_token()
{
	prepare();
	return token_table.front();
}


#define  jump_blank()   while (isblank(ch) and ch not_eq '\n') ch = scanner->readChar()

void Lexer::prepare()
{
	while (token_table.empty() and not scanner->isEof())
	{
		char ch = 0;
		std::string curr_word;
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
			jump_blank();
			/** 遇到以字母或下划线开头的单词*/
			if (isalpha(ch) or ch == '_')
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while ((isalpha(ch) or isdigit(ch) or ch == '_') and not scanner->isEof());
				token_table.push(std::move(curr_word));
			}
			
			
			
			jump_blank();
			/** 匹配数字字面量*/
			if (isdigit(ch))
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (isdigit(ch) and not scanner->isEof());
				token_table.push(std::move(curr_word));
				if(not isblank(ch) and delimiter_string.find(ch) == std::string::npos)
				{
					error(19);
				}
			}
			
			
			jump_blank();
			/** 匹配运算符*/
			if (operator_string.find(ch) not_eq std::string::npos)
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (operator_string.find(ch) not_eq std::string::npos and not scanner->isEof());
				token_table.push(std::move(curr_word));
			}
			
			jump_blank();
			/** 匹配分隔符*/
			if (delimiter_string.find(ch) not_eq std::string::npos)
			{
				curr_word.clear();
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (delimiter_string.find(ch) not_eq std::string::npos and not scanner->isEof());
				token_table.push(std::move(curr_word));
			}
		}
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
	return scanner->isEof();
}

#undef jump_blank
