#include <iostream>
#include <algorithm>
#include <regex>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::regex;
using std::regex_match;

using std::cin;
using std::cout;

/*
 * c++ 版本的词法分析器
 */

const string digit        = R"([0-9])"

const string keywords     = R"(const|var|procedure|begin|end|if|then"
								"|while|do|read|call|write|writeln|module|int)"

const string constant     = R"([0-9]+)"

const string identfier    = R"([A-Za-z_][A-Za-Z_0-9]*)"

const string operator_tag = R"(\+|-|\*|\/|#|=|<|>|<>|:=|>=|<=)"

const string delimiter    = R"([,.;():])"

const string delim		= R"([ \n\t])"

const string whitespace	= R"([ \n\t]+)"

enum word_type
{
	digit_,
	keywords_,
	constant_,
	identfier_,
	operator_tag_,
	delimiter_,
	delim_,
	whitespace_
};

regex digit_regex(digit);
regex keywords_regex(keywords);
regex constant_regex(constant);
regex identfier_regex(identfier);
regex operator_tag_regex(operator_tag);
regex delimiter_regex(delimiter);
regex whitespace_regex(whitespace);


bool check_is_separator(char);

bool get_word(stringstream& ss, string& str)
{
	string tmp;
	const static string separator = operator_tag + delimiter;
	if(not  ss << tmp)
	{
		return false;
	}
	if (regex_search(identfier, )  tmp.find_first_of(separator) not_eq string::npos)
	{
		return false;
	}
	
}

int main(int argc, char* argv[], char* env[])
{
	stringstream ss(cin);
	string current_word;
	while ()
	{
		if ()
	}

}

