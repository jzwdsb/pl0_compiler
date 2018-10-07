//
// Created by manout on 17-11-4.
//

#include "Scanner.h"

Scanner::Scanner():file(nullptr), in(nullptr)
{
}

Scanner::Scanner(const QString &filename):file(nullptr),in(nullptr)
{
    this->setFile(filename);
	prepare();
}

Scanner::~Scanner()
{
}

QChar Scanner::readChar()
{
	prepare();
    QChar ch = *it;
	++it;
	return ch;
}

bool Scanner::isEof()
{
    return this->in->atEnd() and this->it == this->buffer.end();
}

void Scanner::close()
{
    if (this->file not_eq nullptr)
    {
        this->file->close();
        delete this->file;
        this->file = nullptr;
    }
    delete this->in;
    this->in = nullptr;
    buffer.clear();
    it = buffer.begin();
}

void Scanner::setFile(const QString &filename)
{
    this->close();
    this->file = new QFile(filename);

    if (not this->file->open(QFile::ReadOnly | QFile::Text))
    {
        emit this->readError(filename);
        return ;
    }

    this->in = new QTextStream(file);
}

void Scanner::prepare()
{
    if (it == buffer.end() and not in->atEnd())
	{
        this->buffer = this->in->readLine();
		buffer.push_back('\n');
		it = buffer.begin();
	}
}
