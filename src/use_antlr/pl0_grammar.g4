grammar pl0_grammar;


/*	program rule, rule should start with a lowwer case*/
program:	block '.';


/*	block rule*/
block:	(const_declaration)? (var_declaration)? (procedure_declaration)? (statement)?;


/*	const_declaration rule*/
const_declaration:	('const' Ident '=' Number (',' Ident '=' Number)*)?;


/*	var_declaration rule*/
var_declaration:	(('int'|'var') Ident (',' Ident)* ';')?;


/*	procedure_declaration rule*/
procedure_declaration:	('procedure' Ident ';' block ';')?;


/*	statement rule*/
statement:			( Ident ':=' expression
					| 'call' Ident
					| 'begin' statement (';' statement)* 'end'
					| 'if' condition 'then' statement
					| 'while' condition 'do' statement
					| 'read' Ident
					| 'write' expression
					| expression 
					)?;


/*	condition rule*/
condition:			'odd' expression
					| expression Rel_op expression;



/*	expression rule*/
expression:			('-')? term (('+'|'-') term)*;


/*	term rule*/ 
term:				factor(('*'|'/'|'%') factor)*;


/*	factor rule*/
factor:				Ident | Number | '(' expression ')';


/*	Rel_op rule*/
Rel_op:				'='|'<>'|'<'|'<='|'>'|'>=';


/*	Number rule*/
Number:				[0-9]+;


/*	Ident rule*/
Ident:				[a-zA-Z]([a-zA-Z]|[0-9])*;

/*  skip white space*/
WS:                 [ \r\t\n]->skip;
