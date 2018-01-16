import com.common.Lexer;

import java.io.*;

public class Main {
    public static void main(String[] args) throws IOException {
        /**
	    String filename = args[0];
        File file = new File(filename);
        BufferedReader fileReader = new BufferedReader(new FileReader(file));
        String line;
        List<Pair<String, Integer>> result = new LinkedList<>();
        while ((line = fileReader.readLine()) != null)
        {
            Stream.of(line.split("((?<=\\W)+|(?=\\W)+)"))
                    .collect(Collectors.toList())
                    .forEach(value -> result.add(matchType(value)));
        }
        result.forEach(value -> { if (value != null) System.out.println(value.getKey() +
                                        "\t : \t" + value.getValue().toString());});
		fileReader.close();
    }
    private static Pattern keywords_pattern   = Pattern.compile(keywords);
    private static Pattern constant_pattern   = Pattern.compile(constant);
    private static Pattern operator_pattern   = Pattern.compile(operator);
    private static Pattern delimiter_pattern  = Pattern.compile(delimiter);
    private static Pattern identifier_pattern = Pattern.compile(identifier);

    private static Pair<String, Integer> matchType(String word) {
        if (keywords_pattern.matcher(word).matches()) { return new Pair<>(word, 1); }
        if (operator_pattern.matcher(word).matches()) { return new Pair<>(word, 2); }
        if (delimiter_pattern.matcher(word).matches()){ return new Pair<>(word, 3); }
        if (constant_pattern.matcher(word).matches()) { return new Pair<>(word, 4); }
        if (identifier_pattern.matcher(word).matches()){ return new Pair<>(word, 5);}
        return null;
         */

        Lexer lexer = new Lexer("/home/manout/pl0_compiler/demo.pl0");
        while (lexer.nextToken() != null)
        {
            lexer.getToken();
        }
    }
}
