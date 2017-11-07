//
// Created by manout on 17-11-5.
//

#include "SymbolTable.h"
SymbolTable::SymbolTable(SymbolTable *prev = nullptr):table(), prev(prev)
{
	if (prev not_eq nullptr)
	{
		level = prev->level;
	}
	else
	{
		level = 0;
	}
}
SymbolTable &SymbolTable::add(const Symbol & item)
{
	if (this->table.find(item.name) not_eq this->table.end())
	{
		this->table[item.name] = item;
	}
	else
	{
		error(11);
	}
	return *this;
}

SymbolTable &SymbolTable::add(const Symbol* item_ptr)
{
	this->add(*item_ptr);
	return *this;
}
Symbol *SymbolTable::get(const std::string& id)
{
	for (SymbolTable* curr_table = this;
	        curr_table not_eq nullptr;
				curr_table = curr_table->prev)
	{
		auto query_ret = curr_table->table.find(id);
		if (query_ret not_eq curr_table->table.end())
		{
			return &query_ret->second;
		}
	}
	return nullptr;
}


