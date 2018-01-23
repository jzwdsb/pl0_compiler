package com.common;


import org.omg.Messaging.SYNC_WITH_TRANSPORT;

public class Symbol
{
    public String name;
    public Object type = Object.abnormal;
    public int value = -1;
    public int level = -1;
    public int addr = -1;
    public int size = -1;
    Symbol(CharSequence name)
    {
        this.name =  name.toString();
    }
}
