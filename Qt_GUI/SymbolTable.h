//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_SYMBOLTABLE_H
#define CPP_VERSION_SYMBOLTABLE_H

#include <QHash>

#include "pl0.h"


class SymbolTable
{
public:
	explicit SymbolTable(SymbolTable* prev = nullptr);
    bool add(const Symbol&);
    bool add(const Symbol*);
    Symbol* get(const QString& id);
	int get_level();
private:
    QHash<QString, Symbol> table;
	SymbolTable* prev;
	int level;
};


#endif //CPP_VERSION_SYMBOLTABLE_H
