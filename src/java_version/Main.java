import com.common.Lexer;

import java.io.*;

public class Main {
    public static void main(String[] args) throws IOException {
        Lexer lexer = new Lexer("/home/manout/pl0_compiler/demo.pl0");
        while (lexer.nextToken() != null)
        {
            lexer.getToken();
        }
    }
}
