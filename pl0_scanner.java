package com.manout;

import javafx.util.Pair;
import org.jetbrains.annotations.NotNull;

import java.io.*;
import java.util.LinkedList;
import java.util.List;
import java.util.regex.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toList;

public class Main {

    private static String keywords     = "\\s*(const|var|procedure|begin|end|if|then|while|do|read|call|write|writeln|module|int)\\s*";

    private static String constant     = "[0-9]+";

    private static String identifier    = "[A-Za-z_][A-Za-z_0-9]*";

    private static String operator     = "\\+|-|\\*|/|#|=|<|>|<>|:=|>=|<=";

    private static String delimiter    = "[,.;():]";

    private static String separator = "((?<=:=)|(?=:=))|((?<=[+\\-*/#=<>,.;():])|(?=[+\\-*/#=<>./;():]))|\\s+";
    public static void main(String[] args) throws IOException {
	    String filename = args[0];
        File file = new File(filename);
        BufferedReader fileReader = new BufferedReader(new FileReader(file));
        String line;
        List<Pair<String, Integer>> result = new LinkedList<>();

        while ((line = fileReader.readLine()) != null)
        {

            Stream.of(line.split(separator)).filter(value -> !value.equals(""))
                    .collect(Collectors.toList()).forEach(value -> result.add(matchType(value)));
        }
        result.forEach(value -> { if (value.getValue() != 0) System.out.println(value.getKey() + " : " + value.getValue().toString());});

    }

    private static Pattern keywords_pattern   = Pattern.compile(keywords);
    private static Pattern constant_pattern   = Pattern.compile(constant);
    private static Pattern operator_pattern   = Pattern.compile(operator);
    private static Pattern delimiter_pattern  = Pattern.compile(delimiter);
    private static Pattern identifier_pattern = Pattern.compile(identifier);

    @NotNull
    private static Pair<String, Integer> matchType(String word) {
        if (keywords_pattern.matcher(word).matches()) {
            return new Pair<>(word, 1);
        }
        if (operator_pattern.matcher(word).matches())
        {
            return new Pair<>(word, 2);
        }
        if (delimiter_pattern.matcher(word).matches())
        {
            return new Pair<>(word, 3);
        }
        if (constant_pattern.matcher(word).matches())
        {
            return new Pair<>(word, 4);
        }
        if (identifier_pattern.matcher(word).matches())
        {
            return new Pair<>(word, 5);
        }
        return new Pair<>("",0);
    }
}
