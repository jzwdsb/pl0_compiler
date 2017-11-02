#include <iostream>
#include <algorithm>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>


using std::string;
using std::stringstream;
using std::regex;
using std::regex_match;
using std::vector;
using std::for_each;
using std::cin;
using std::cout;
/*
 * c++ 版本的词法分析器
 */

const string digit        = R"([0-9])";

const string keywords     = R"(const|var|procedure|begin|end|if|then"
								"|while|do|read|call|write|writeln|module|int)";

const string constant     = R"([0-9]+)";

const string identfier    = R"([A-Za-z_][A-Za-Z_0-9]*)";

const string operator_tag = R"(\+|-|\*|\/|#|=|<|>|<>|:=|>=|<=)";

const string delimiter    = R"([,.;():])";

const string delim		= R"([ \n\t])";

const string whitespace	= R"([ \n\t]+)";

enum word_type
{
	keywords_,
	operator_tag_,
	delimiter_,
	constant_,
	identfier_,
	error
};

regex digit_regex(digit);
regex keywords_regex(keywords);
regex constant_regex(constant);
regex identfier_regex(identfier);
regex operator_tag_regex(operator_tag);
regex delimiter_regex(delimiter);
regex whitespace_regex(whitespace);



word_type get_word_type(const string& word)
{
	if (regex_match(word, keywords_regex))
		return word_type::keywords_;
	if(regex_match(word, operator_tag_regex))
		return word_type::operator_tag_;
	if(regex_match(word, delimiter_regex))
		return word_type::delimiter_;
	if(regex_match(word, constant_regex))
		return word_type::constant_;
	if(regex_match(word, identfier_regex))
		return word_type::identfier_;
	
	return word_type::error;
}
int main(int argc, char* argv[], char* env[])
{
	std::fstream filestream(argv[1], std::ios_base::in);
	const string separator = "+-*/,.;:()<>:=";
	string line;
	vector<string> words;
	vector<std::pair<string, word_type>> result;
	while (std::getline(filestream, line))
	{
		boost::split(words, line, boost::is_any_of(separator));
		std::for_each(words.begin(), words.end(), [&](const string& word){
					result.emplace_back(word, get_word_type(word));
				});
	}
	for_each(result.begin(), result.end(), [](const std::pair<string, word_type> word )
										{ printf("%s\t:\t%d", word.first.c_str(), word.second); });
	return 0;
}

