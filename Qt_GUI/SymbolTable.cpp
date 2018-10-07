//
// Created by manout on 17-11-5.
//

#include "SymbolTable.h"
SymbolTable::SymbolTable(SymbolTable *prev):table(), prev(prev)
{
	if (prev not_eq nullptr)
	{
		level = prev->level + 1;
	}
	else
	{
		level = 0;
	}
}
bool SymbolTable::add(const Symbol & item)
{
    if (not this->table.contains(item.name))
	{
		this->table[item.name] = item;
	}
	else
	{
        return false;
	}
    return true;
}

bool SymbolTable::add(const Symbol* item_ptr)
{
    return this->add(*item_ptr);
}

Symbol *SymbolTable::get(const QString& id)
{
	for (SymbolTable* curr_table = this;
	        curr_table not_eq nullptr;
				curr_table = curr_table->prev)
	{
        if (curr_table->table.contains(id))
		{
            return &curr_table->table[id];
		}
	}
	return nullptr;
}

int SymbolTable::get_level()
{
	return this->level;
}


