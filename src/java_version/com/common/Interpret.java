package com.common;

import java.util.ArrayList;

public class Interpret
{
    private ArrayList<Instruction> code;
    private Instruction IR;
    private int PC;
    private int ESP;
    private int EBP;

    public void runOneStep()
    {
        decode();
    }

    public void run()
    {
        do
        {
            decode();
        }while (this.PC != 0);
    }

    public void reset()
    {
        this.PC = 0;
        this.ESP = 0;
        this.EBP = 0;
        this.IR = null;
    }

    private void decode()
    {

    }

}
