//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_SYMBOLTABLE_H
#define CPP_VERSION_SYMBOLTABLE_H
#include <unordered_map>
#include "pl0.h"


class SymbolTable
{
public:
	explicit SymbolTable(SymbolTable* prev = nullptr);
	SymbolTable& add(const Symbol&);
	SymbolTable& add(const Symbol*);
	Symbol* get(const std::string& id);
	int get_level();
private:
	std::unordered_map<std::string, Symbol> table;
	SymbolTable* prev;
	int level;
};


#endif //CPP_VERSION_SYMBOLTABLE_H
