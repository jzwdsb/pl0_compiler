#! /usr/bin/python3
# -*- coding: utf8 -*- 

"""
    python 版本的词法扫描器
    其实将问题抽象起来就是从一个字符串流中提取每个符号
    将其做上标记并输出的过程
"""

import sys
import re


# 规则声明

keywords     = r"const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int"

constant     = r"[0-9]+"

identfier     = r"[A-Za-z_][A-Za-z_0-9]*"

operator     = r"\+|-|\*|\/|#|=|<|>|<>|:=|>=|<="

delimiter    = r"[,.;():]"

whitespace   = r"\s*"

separator    = r"([+\-*/#,.;():\s]|<=|>=|:=|=|<|>)"

print ("""
1:      keywords
2:      operator
3:      delimiter
4:      constant
5:      identfier
        """)

with open(sys.argv[1], mode='r') as pl0_file:
# 每次的词法分析只处理一行
    content = []
    result = []
    word_error = []
    content = pl0_file.readlines()
    for line in content:
        line = re.split(separator, line)
        line = [c for c in line if re.match('\S', c)]
        for word in line:
            if (re.match(keywords, word)):
                result.append((word, 1))
                continue
            if (re.match(operator, word)):
                result.append((word, 2))
                continue
            if (re.match(delimiter, word)):
                result.append((word, 3))
                continue
            if (re.match(constant, word)):
                result.append((word, 4))
                continue
            if (re.match(identfier, word)):
                result.append((word, 5))
                continue
            word_error.append("invalid word: %s"%(word))
            
        print(line)
    print(result)
    if len(word_error) != 0:
        print(word_error)
