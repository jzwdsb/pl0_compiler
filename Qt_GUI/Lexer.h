//
// Created by manout on 17-11-4.
//

#ifndef CPP_VERSION_LEXER_H
#define CPP_VERSION_LEXER_H

#include <QQueue>

#include <QString>

#include "pl0.h"
#include "Scanner.h"

/** 建立在 Scanner 上，实现为字符流的分词*/
class Lexer : public QObject
{
    Q_OBJECT
public:
    explicit Lexer();
    Lexer(const Lexer&) = delete;
    virtual ~Lexer();
    void setFile(const QString& filename);
    void close();

    void clear();

    QString get_token();
    const QString & next_token();
	bool isEof();
    int rows();
signals:
    void lexError(int row, int err_code);
private:
    void prepare();
    /** lexer 掌握scanner的所有权*/
	Scanner* scanner;
    QQueue<QString> token_table;

    int row;
};


#endif //CPP_VERSION_LEXER_H
