//
// Created by manout on 17-11-4.
//

#ifndef CPP_VERSION_SCANNER_H
#define CPP_VERSION_SCANNER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

/** 底层部件，实现对文件的读*/

class Scanner : public QObject
{
    Q_OBJECT
public:
    explicit Scanner();
    explicit Scanner (const QString& filename);
    Scanner(const Scanner&) = delete;
	virtual ~Scanner();
    QChar readChar();
	bool isEof();
    void setFile(const QString& filename);
public slots:
    void close();

signals:
    void readError(QString filename);

private:
	void prepare();
    QString buffer;
    QString::iterator it;
    QFile *file;
    QTextStream *in;
};


#endif //CPP_VERSION_SCANNER_H
