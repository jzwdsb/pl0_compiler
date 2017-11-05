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
	SymbolTable& add(const Item&);
	SymbolTable& add(const Item*);
	Item* get(const std::string& id);
private:
	std::unordered_map<std::string, Item> table;
	SymbolTable* prev;
};


#endif //CPP_VERSION_SYMBOLTABLE_H
