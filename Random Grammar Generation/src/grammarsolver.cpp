/*
 * File: grammarsolver.cpp
 * -----------------------
 * Assignment 3 Part C Coding Grammar Solver
 * This program mainly writes down grammarGenerate function. It accepts three parameters initilally,
 * which are input text file, a string symbol, and an integer times. Firstly, when reading the text,
 * the program will build a map of which key is non-terminal and value is its corresponding grammar
 * rule(s), which are either terminal or non-terminal. Next part is to randomly generate texts according
 * to the grammar. The program will use input symbol as a start, then it randomly picks up a grammar rule.
 * If the rule is non-terminal, the program will recursively look up the map until it finds the terminal,
 * and finally gather all non-terminal words into a text for this grammar. The program will generate
 * # = times of texts. Enter is to quit.
 *
 * @author Liu Jiang (Caci)
 * @version 10/16/2019
*/
#include "grammarsolver.h"
#include <iostream> // for cout, endl
#include <fstream> // for istream
#include <string> // for string class
#include "strlib.h" // for getline
#include "map.h" // import Map collection
#include "vector.h" // import vector collection
#include "random.h" // for randomInteger

using namespace std;

/* Function prototype declarations */
Map<string, Vector<string>> readInput(istream& input);
void generateElements(string symbol,const Map<string, Vector<string>>& map, string& text);
void repeatedGenerate(string symbol, int times, const Map<string, Vector<string>>& map, Vector<string>& allVec);

/*
 * This is the major function which is directly called by main program.
 * This function will call helperfunction of readInput to build a map which records the grammar in the
 * input text file. Then it calls helper function of repeatedGenerated, which is to generate # = times
 * of random texts according to the grammar. Lastly, it returns a vector containing all the generated
 * texts.
 * @param input is input text file
 * @param symbol is the starting symbol input by user
 * @param times defines how many texts to generate
 * @return a vector of randomly generated texts
*/
Vector<string> grammarGenerate(istream& input, string symbol, int times) {
    // throw an error if the symbol parameter passed to your function is empty, ""
    if(symbol == "") {
        throw "Empty symbol is invalid.";
    }
    Map<string, Vector<string>> map = readInput(input); // build the map for grammar rules
    Vector<string> v;
    repeatedGenerate(symbol, times, map, v); // randomly generate texts
    return v;
}

/*
 * This helper function is to build the map of grammar rules when reading the input text file line by line.
 * The map's key is the non-terminal, and value is a vector of either terminal or non-terminal.
 * @param input is input text file
 * @return map containing grammar rules
*/
Map<string, Vector<string>> readInput(istream& input) {
    Map<string, Vector<string>> map; // initalize the map for grammar rules
    string line; // current line when reading the file
    // iterate across input file line by line
    while(getline(input, line)) {
        line = trim(line); // remove outer whitespaces in the line
        Vector<string> lineVec = stringSplit(line, "::="); // split the line into symbol and grammar rules
        string name = lineVec[0]; // symbol which is non-terminal
        // Throw an error if the grammar contains more than one line for the same non-terminal
        if(map.containsKey(name)) {
            throw "There are duplicate symbols defined in different lines.";
        } else {
            Vector<string> rules = stringSplit(lineVec[1], "|"); // a vector of terminals or non-terminals
            map[name] = rules;
        }
    }
    return map;
}

/*
 * This helper function is to call another helper function of generateElements to randomly generate
 * texts according to the grammar for # = times.
 * @param symbol is the starting symbol input by user
 * @param map contains grammar rules
 * @param allVec will add generated text into a vector at each time
 * @return allVec
*/
void repeatedGenerate(string symbol, int times, const Map<string, Vector<string>>& map, Vector<string>& allVec) {
    // loop to generate # = times of random texts
    for(int i  =  0; i < times; i++) {
        string elements = ""; // initialize an empty string for generated text
        generateElements(symbol, map, elements); // call helper function to generate text recursively
        elements = trim(elements); // remove the whitespace at the end
        allVec.add(elements);
    }
}

/*
 * This helper function is to randomly generate text according to the grammar recursively.
 * Base case: the symbol is a terminal. Then it will be appended to output text.
 * Recursive case: the symbol is a non-terminal. The function will look up its grammar rules and pass the
 * rules to call itself again.
 * @param symbol is either terminal or non-terminal
 * @param map contains grammar rules
 * @param text appends fundamental word to form the output sentence
*/
void generateElements(string symbol, const Map<string, Vector<string>>& map, string& text) {
    // base case if the symbol is terminal, and add it to output text.
    if(!map.containsKey(symbol)) {
        text.append(symbol+" ");
        return;
    }
    // recursive case if the symbol is non-terminal
    Vector<string> rules = map[symbol]; // find the corresponding grammar rules
    string rule = rules[randomInteger(0, rules.size()-1)]; // randomly choose a grammar rule
    Vector<string> tokens = stringSplit(rule, " ");
    // look up all tokens inside a rule
    for(string token : tokens) {
        generateElements(token, map, text);
    }
}
