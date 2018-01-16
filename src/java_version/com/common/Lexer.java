package com.common;


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

        IDENTIFIER("[A-Za-z_][\\w_]*");
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


    private File file = null;
    private Queue<Token> buffer = null;
    private BufferedReader reader = null;
    static private Pattern tokenPattern;
    static private String tokenPatternString;
    static
    {
        StringBuffer tokenPatternBuffer = new StringBuffer();
        for (TokenType tokenType : TokenType.values())
        {
            tokenPatternBuffer.append(String.format("|(?<%s>%s)", tokenType.name(), tokenType.pattern));
        }
        /** 去掉开头多余的 | */
        tokenPattern = Pattern.compile(tokenPatternBuffer.substring(1));
    }
    public Lexer(String filename) throws FileNotFoundException
    {
        setSourceFile(filename);
    }

    public void setSourceFile(String filepath) throws FileNotFoundException
    {
        file = new File(filepath);
        reader = new BufferedReader(new FileReader(file));
        this.buffer = new ArrayDeque<Token>();
    }

    public void close() throws IOException
    {
        this.reader.close();
    }
    private void prepare() throws IOException
    {
        while(buffer.isEmpty() && reader.ready())
        {
            String line = reader.readLine();
            Matcher matcher = tokenPattern.matcher(line);
            while (matcher.find())
            {
                if (matcher.group(TokenType.KEYWORD.name()) != null)
                {
                    buffer.add(new Token(TokenType.KEYWORD, matcher.group(TokenType.KEYWORD.name())));
                }else if (matcher.group(TokenType.OPERATOR.name()) != null)
                {
                    buffer.add(new Token(TokenType.OPERATOR, matcher.group(TokenType.OPERATOR.name())));
                }else if (matcher.group(TokenType.DELIMITER.name()) != null)
                {
                    buffer.add(new Token(TokenType.DELIMITER, matcher.group(TokenType.DELIMITER.name())));
                }else if (matcher.group(TokenType.NUMBER.name()) != null)
                {
                    buffer.add(new Token(TokenType.NUMBER, matcher.group(TokenType.NUMBER.name())));
                }else if (matcher.group(TokenType.IDENTIFIER.name()) != null)
                {
                    buffer.add(new Token(TokenType.IDENTIFIER, matcher.group(TokenType.IDENTIFIER.name())));
                }
            }
        }

    }

    public Token getToken() throws IOException
    {
        prepare();
        return buffer.poll();
    }

    public Token nextToken() throws IOException
    {
        prepare();
        return buffer.peek();
    }
}
