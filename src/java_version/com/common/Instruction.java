package com.common;

public class Instruction
{
    private Fct OP;
    private int L;
    private int M;

    public Instruction()
    {
        this.OP = Fct.err;
        this.L = -1;
        this.M = -1;
    }

    public Instruction(Fct op, int l, int m)
    {
        this.OP = op;
        this.L = l;
        this.M = m;
    }

    public Fct getOP()
    {
        return OP;
    }

    public void setOP(Fct OP)
    {
        this.OP = OP;
    }
    public int getM()
    {
        return M;
    }

    public void setM(int m)
    {
        M = m;
    }
    public int getL()
    {
        return L;
    }

    public void setL(int l)
    {
        L = l;
    }
}
