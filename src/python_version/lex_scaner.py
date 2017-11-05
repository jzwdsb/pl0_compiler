#! /usr/bin/python3
# -*- coding: utf8 -*- 

"""
    python 版本的词法扫描器
    其实将问题抽象起来就是从一个字符串流中提取每个符号
    将其做上标记并输出的过程
    get_token() 方法应当返回从输入流中取出一个词素并返回
    next_token() 方法应当返回输入流中的下一个词素单数不返回
"""

import re

# 规则声明

keywords = r"const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int"

constant = r"[0-9]+"

identfier = r"[A-Za-z_][A-Za-z_0-9]*"

operator = r"\+|-|\*|\/|#|=|<|>|<>|:=|>=|<="

delimiter = r"[,.;():]"

whitespace = r"\s*"

separator = r"([+\-*/#,.;():\s]|<=|>=|:=|=|<|>)"



def scan_file(file_path: str):
    with open(file_path) as pl0_file:
        for line in pl0_file.readline():
            tokens = re.split(separator, line)





