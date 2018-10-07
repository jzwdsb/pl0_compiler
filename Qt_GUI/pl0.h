//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_PL0_H
#define CPP_VERSION_PL0_H
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <QObject>
#include <QList>

#define NOT_INITIALIZE  0xffffffff

/** 标识符类型*/
enum object
{
	constant,
	variable,
	array,
    procedure,
    abnormal = 0xff
};

/** 类　P code 指令集*/
enum fct
{
	lit,
	opr,
	lod,
    lda,
    sto,
    sta,
	cal,
	inc,
	jmp,
	jpc,
    sio,
    err = 0xff
};

/** 类P Code 指令类型， 包含三个字段，指令OP, 层差L, 和另一个操作数M*/
struct instruction
{
	fct OP;
	int L;
	int M;
	instruction() = default;
	instruction(const instruction&) = default;
	instruction(fct op, int l, int m):OP(op), L(l), M(m){}
    instruction& operator =(const instruction&) = default;
};


/** pl0　的符号定义*/
struct Symbol
{
public:
    QString name;
    object type = object::abnormal;
    int value = NOT_INITIALIZE;          // only used by constant
    int level = NOT_INITIALIZE;          // layer, used by variable, array, procedure
    int addr = NOT_INITIALIZE;           // address, used by procedure
    int size = NOT_INITIALIZE;           // size, used by procedure
	Symbol() = default;
	Symbol(const Symbol&) = default;
    Symbol(const QString& name):name(name){}
};


#endif //CPP_VERSION_PL0_H
