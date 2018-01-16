package com.common;

import jdk.nashorn.internal.parser.Token;

import java.io.*;
import java.util.ArrayDeque;
import java.util.Queue;
import java.util.regex.*;
public class Lexer
{

    public static enum TokenType
    {
        KEYWORD("begin|end|if|then|else|const|procedure|var|int|do|while|call|read|write|repeat|odd|until|sqrt|array"),

        OPERATOR("[*/+\\-]"),

        DELIMITER("[\\.;,]"),

        NUMBER("-?\\d+"),

        IDENTIFIER("[A-Za-z_][\\w_]+");
        public final String pattern;

        TokenType(String pattern)
        {
            this.pattern = pattern;
        }
    }

    public static class Token
    {
        public TokenType type;
        public String value;

        public Token(TokenType type, String value)
        {
            this.type = type;
            this.value = value;
        }

        @Override
        public String toString()
        {
            return String.format("(%s %s)", type.name(), value);
        }
    }


    private Queue<Token> buffer = null;

    public Lexer()
    {

    }

    public Lexer(String filename) throws FileNotFoundException
    {
        setSourceFile(filename);
    }

    public void setSourceFile(String filepath) throws FileNotFoundException
    {
    }

    private void prepare() throws IOException
    {

    }

    public String getToken() throws IOException
    {
    }

    public String nextToken() throws IOException
    {
    }
}
