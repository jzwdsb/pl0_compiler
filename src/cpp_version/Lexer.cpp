//
// Created by manout on 17-11-4.
//

#include "Lexer.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <regex>

#define  jump_blank()   \
			while (isblank(ch)) ch = scanner->readChar()

Lexer::Lexer(Scanner* scer):scanner(scer),lex_table()
{

}

Item Lexer::get_token()
{
	prepare();
	std::string curr_token;
	curr_token = lex_table.front();
	lex_table.pop_front();
	Item curr_item(curr_item);
	if (key_word_set.find(curr_token) not_eq key_word_set.end())
	{
		curr_item.type = type::keyword;
		return curr_item;
	}
	if (operator_set.find(curr_token) not_eq operator_set.end())
	{
		curr_item.type = type::_operator;
		return curr_item;
	}
	if (delimiter_set.find(curr_token) not_eq delimiter_set.end())
	{
		curr_item.type = type::delimiter;
		return curr_item;
	}
	if (std::regex_match(curr_token, std::regex(R"([0-9]+)")))
	{
		curr_item.type = type::constant;
		return curr_item;
	}
	if (std::regex_match(curr_token, std::regex(R"([A-Za-z_][A-Za-z0-9_]*)")))
	{
		curr_item.type = type::identifier;
		return curr_item;
	}
	
}

Item Lexer::next_token()
{
	
	return Item();
}

void Lexer::prepare()
{
	if (lex_table.empty() and not scanner->isEof())
	{
		char ch = 0;
		std::vector<std::string> words;
		std::string curr_word;
		ch = scanner->readChar();
		/*  一次处理一行
		    遍历一行主要寻找的模式有
		    标识符
		    字面量
		    运算符
		    分隔符
		*/
		while (ch not_eq '\n')
		{
			jump_blank();
			// 遇到以字母或下划线开头的单词
			if (isalpha(ch) or ch == '_')
			{
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (isalpha(ch) or isdigit(ch) or ch == '_');
				lex_table.push_back(std::move(curr_word));
				curr_word.clear();
			}
			
			
			// 匹配数字字面量
			jump_blank();
			if (isdigit(ch))
			{
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (isdigit(ch));
				lex_table.push_back(curr_word);
				curr_word.clear();
			}
			
			
			// 匹配运算符
			jump_blank();
			if (operator_string.find(ch) not_eq std::string::npos)
			{
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (operator_string.find(ch) not_eq std::string::npos);
				lex_table.push_back(curr_word);
				curr_word.clear();
			}
			
			//匹配分隔符
			jump_blank();
			if (delimiter_string.find(ch) not_eq std::string::npos)
			{
				do
				{
					curr_word.push_back(ch);
					ch = scanner->readChar();
				}while (delimiter_string.find(ch) not_eq std::string::npos);
				lex_table.push_back(curr_word);
				curr_word.clear();
			}
		}
		/*
		if (current_line == nullptr)
		{
			return;
		}
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
			lex_table.push(std::move(word));
		}
		delete current_line;
		*/
	}
}



