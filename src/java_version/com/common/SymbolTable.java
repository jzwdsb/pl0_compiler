package com.common;

import java.util.HashMap;

public class SymbolTable
{
    private SymbolTable  prev = null;
    private HashMap<String, Symbol> table;
    private int level = 0;

    public SymbolTable(SymbolTable prev)
    {
        this.prev = prev;
        if (this.prev != null)
        {
            this.level = prev.level + 1;
        }
        this.table = new HashMap<String, Symbol>();
    }

    public void add(Symbol symbol)
    {
        if (this.table.containsKey(symbol.name))
        {
            Parser.error(19);
        }else
        {
            this.table.put(symbol.name, symbol);
        }
    }

    public Symbol get(String name)
    {
        SymbolTable curr = this;
        while (curr != null)
        {
            if (curr.table.containsKey(name))
            {
                return curr.table.get(name);
            }
            curr = curr.prev;
        }
        return null;
    }

    public int getLevel()
    {
        return this.level;
    }
}
