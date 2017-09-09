#include <iostream>
#include <algorithm>
#include <regex>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;

string digit        = R"([0-9])"

string keywords     = R"(const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int)"

string constant     = R"([0-9]+)"

string identfier    = R"([A-Za-z_][A-Za-Z_0-9]*)"

string operator_tag = R"(\+|-|\*|\/|#|=|<|>|<>|:=|>=|<=)"

string delimiter    = R"([,.;():])"

string delim		= R"([ \n\t])"

string whitespace	= R"([ \n\t]+)"


