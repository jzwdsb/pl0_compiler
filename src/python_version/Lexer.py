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
import collections

Token = collections.namedtuple('Token', ['type', 'value'])


def tokenize(code: str) -> Token:
    token_specification = [
        ("KEYWORDS", r"const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int"),
        ('OP', r'\+|-|\*|/|\+\+|--|<>|:=|=|>=|<=|<|>|%'),
        ('DELIMITER', r'[,;.]'),
        ('NUM', r'\d+'),
        ('IDENTIFIER', r'[A-Za-z][A-Za-z_0-9]*'),
        ('SKIP', r'\s+'),
        ("MISMATCH", r'.')
    ]
    tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_specification)
    print(tok_regex)
    for mo in re.finditer(tok_regex, code):
        kind = mo.lastgroup
        value = mo.group(kind)
        if kind == 'SKIP':
            continue
        elif kind == 'MISMATCH':
            raise RuntimeError('%s unexpected on line' % value)
        else:
            yield Token(kind, value)


if __name__ == '__main__':
    with open("/home/manout/pl0_compiler/demo.pl0", 'r') as file:
        code = file.read()
        for token in tokenize(code):
            print(token)
