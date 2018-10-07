#ifndef PRASER_H
#define PRASER_H

#include <QObject>
#include <QList>


#include "Scanner.h"
#include "Lexer.h"
#include "SymbolTable.h"
#include "pl0.h"
class pl0_parser : public QObject
{
    Q_OBJECT
public:
    friend class interpret;
    pl0_parser();
    pl0_parser(const pl0_parser&) = delete;
    pl0_parser(QString& filename);
    ~pl0_parser();
    /** 以下函数若匹配成功则生成相应的 类P code代码，
     * 通过向前查看一个移进符号选择移进规则
     * 错误处理通过 error 输出错误信息后 exit(-1) 直接退出*/

    void program();
    void block();
    void const_declaration();
    void var_declaration();
    void array_declaration();
    void procedure_declaration();
    void statement();
    void condition();
    void expression();
    void term();
    void factor();

    void close();
    void clear();

    void setCodeSource(const QString& filename);
    void setLexer(Lexer* l);

    // Parser interface
    void parserCode();
    const QList<QString>* codeToStr();
    const QList<instruction>* getCode();

signals:
    /** pass error send by below to the mainWindow*/
    void error(int row,int err_code);

private :

    int base_address;
    /**	词法分析器*/
    Lexer*  lexer;
    /**	当前命名空间*/
    SymbolTable  *local_space;
    /** 生成的类P Code 代码表, 对应于程序的text(正文段)*/
    QList<instruction> code;

    QList<QString> codeStr;
};

#endif // PRASER_H
