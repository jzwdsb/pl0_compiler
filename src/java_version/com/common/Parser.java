package com.common;

import com.common.Lexer;
import com.sun.org.apache.xml.internal.serializer.ToTextSAXHandler;

import java.io.IOException;
import java.util.ArrayList;


public class Parser
{
    private Lexer lexer ;
    private ArrayList<Instruction> code = new ArrayList<>();
    private int baseAddress = 0;
    SymbolTable localSpace;
    public Parser (Lexer lexer)
    {
        this.lexer = lexer;
    }

    public void parseCode() throws IOException
    {
        program();
    }

    private void program() throws IOException
    {
        block();
        if (lexer.nextToken().value.equals("."))
        {
            lexer.getToken();
            return;
        }
        error(9);
    }

    private void block() throws IOException
    {
        SymbolTable prev = localSpace;
        localSpace = new SymbolTable(prev);
        baseAddress = 3;
        const_declaration();
        var_declaration();
        int pos = code.size();
        generateCode(Fct.jmp, 0, 0);
        procedure_declaration();
        code.get(pos).setM(code.size());
        statement();
        generateCode(Fct.opr, 0, 0);
        localSpace = prev;

    }

    private void const_declaration() throws IOException
    {
        Lexer.Token currToken;
        if (lexer.nextToken().equals("const"))
        {
            lexer.getToken();
            do
            {
                currToken = lexer.getToken();
                if (currToken.type == Lexer.TokenType.IDENTIFIER)
                {
                    Symbol currSym = new Symbol(currToken.value);
                    currSym.type = Object.constant;
                    currSym.level = localSpace.getLevel();
                    currToken = lexer.getToken();
                    if (currToken.type == Lexer.TokenType.OPERATOR && currToken.value.equals("="))
                    {
                        currToken = lexer.getToken();
                        try
                        {
                            currSym.value = Integer.valueOf(currToken.value);
                        }
                        catch(NumberFormatException e)
                        {
                            e.getStackTrace();
                            System.out.println(e.getMessage());
                            error(2);
                        }
                        localSpace.add(currSym);
                    }else
                    {
                        error(3);
                    }
                }else
                {
                    error(19);
                }
            }while (lexer.nextToken().equals(",") && lexer.getToken().equals(","));
            if (lexer.nextToken().type == Lexer.TokenType.DELIMITER && lexer.nextToken().value.equals(";"))
            {
                lexer.getToken();
            }else
            {
                error(5);
            }
        }
    }

    private void var_declaration() throws IOException
    {
        Lexer.Token currToken;
        int variableCount = 0;
        if (lexer.nextToken().value.equals("int") || lexer.nextToken().value.equals("var"))
        {
            lexer.getToken();
            do
            {
                currToken = lexer.getToken();
                if (currToken.type == Lexer.TokenType.IDENTIFIER)
                {
                    Symbol currVar = new Symbol(currToken.value);
                    currVar.type = Object.variable;
                    currVar.addr = baseAddress;
                    currVar.level = localSpace.getLevel();
                    localSpace.add(currVar);
                    ++baseAddress;
                    ++variableCount;
                }else
                {
                    error(19);
                }
            }while(lexer.nextToken().value.equals(";") && lexer.getToken().value.equals(";"));
        }
        if (lexer.nextToken().value.equals(";"))
        {
            lexer.getToken();
        }else
        {
            error(5);
        }
    }

    private void procedure_declaration() throws IOException
    {
        Lexer.Token currToken;
        while (lexer.nextToken().value.equals("procedure"))
        {
            lexer.getToken();
            currToken = lexer.getToken();
            if (currToken.type == Lexer.TokenType.KEYWORD)
            {
                Symbol currProcd = new Symbol(currToken.value);
                currProcd.type = Object.procedure;
                currProcd.addr = code.size();
                currProcd.level = localSpace.getLevel();
                if (lexer.nextToken().equals(";"))
                {
                    lexer.getToken();
                    block();
                    if (lexer.nextToken().value.equals(";"))
                    {
                        lexer.getToken();
                        currProcd.size = code.size() - currProcd.addr;
                        localSpace.add(currProcd);
                    }else
                    {
                        error(17);
                    }
                }else
                {
                    error(17);
                }
            }
        }
    }

    private void statement() throws IOException
    {
        Lexer.Token currToken;
        switch (lexer.nextToken().value)
        {
            case "begin":
                break;
            case "call":
                break;
            case "if":
                break;
            case "while":
                break;
            case "for":
                break;
            case "read":
                break;
            case "write":
                break;
            default:
                    break;
        }

    }

    private void expression()
    {

    }

    private void term()
    {

    }

    private void factor()
    {

    }

    static void error(int err_code)
    {

    }

    private void generateCode(Fct op, int l, int m)
    {
        this.code.add(new Instruction(op, l, m));
    }

}
