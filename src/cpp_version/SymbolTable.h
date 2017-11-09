//
// Created by manout on 17-11-5.
//

#ifndef CPP_VERSION_SYMBOLTABLE_H
#define CPP_VERSION_SYMBOLTABLE_H
#include <unordered_set>
#include <bits/unordered_map.h>
#include "pl0.h"

class SymbolTable
{
public:
	explicit SymbolTable(SymbolTable* prev = nullptr);
	SymbolTable& add(const Symbol&);
	SymbolTable& add(const Symbol*);
	int get_level();
	Symbol* get(const std::string& id);
private:
	std::unordered_map<std::string, Symbol> table;
	SymbolTable* prev;
	int level;
};


#endif //CPP_VERSION_SYMBOLTABLE_H
