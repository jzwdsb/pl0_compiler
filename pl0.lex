%{/* plo 语言词法分析程序 lex 描述文档*/%}

%{
/*
	pl0 的词法描述，用正则表达式书写
*/

digit		[0-9]		/* 数字 0 - 9 */

//letter		[a-zA-Z]	/* 字母接受 a-z 的小写，同时本程序定义 PL/0 语言对大小写敏感*/

//number		{digit}+	/* 无符号整数*/

//identifier	{letter}( {letter} | {digit} )*	/* 标识符 */

//newline		[\n]		/* 新行*/

//whitespace	[\t]+		/* 制表符 */

// pl0 的保留字枚举，我实在懒得写大小写不区分的情况
keyword		[const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln] 

// 常数
constant	([0-9])+

// 标识符
identfier	[A-Za-z]([[A-Za-z][0-9][_]])+

// 运算符
operator	\+|-|\*|\/|:=|>=|<=|#|=

// 分界符
delimiter	[,.;()]

// pl0 的词法分析器要求跳过分隔符(空格，回车，制表符)
delim		[ \n\t]
whitespace	{delim}+

%}

// 以下是 lex 的规则声明部分
%%
