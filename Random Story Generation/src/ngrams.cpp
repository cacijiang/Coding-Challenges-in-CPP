/*
 * File: ngrams.cpp
 * ----------------
 * Assignment 2 Part C N-Grams
 *
 * This program will prompt the user to input a text file and an integer N at first. When reading the
 * text, the program build up a map from prefixes to suffixes, of which each key contains N-1 words
 * and value contains a collection of the corresponding N-th word. Next part is to prompt the user to
 * input text length and generate random text containing that number of words. The program will choose
 * a random starting point. Then the map will look up all possible next words that follow the current
 * N-1 words. The program will prompt the user to input next text length, while 0 for exiting the program.
 *
 * @author Liu Jiang (Caci)
 * @version 10/01/2019
*/

#include <iostream> // for cout, endl
#include <fstream> // ifstream
#include "filelib.h" // for promptUserForFile
#include "simpio.h" // for getInteger
#include "console.h" // for graphical console
#include "map.h" // Import Map collection
#include "vector.h"// Import Vector collection
#include "queue.h" // Import Queue collection
#include <string> // for string class
#include "random.h" // for randomInteger

using namespace std;

/* Function prototype declarations */
void intro();
void inputFile(int& nValue, Map<Queue<string>, Vector<string>>& map);
void buildMap(ifstream& infile, const int& nValue, Map<Queue<string>, Vector<string>>& map);
void wrapMap(Map<Queue<string>, Vector<string>>& map, Queue<string>& window, Queue<string>& frontWords);
void inputTextLength(const int& nValue, const Map<Queue<string>, Vector<string>>& map,  string& text);
string generateText(const int& nValue, const Map<Queue<string>, Vector<string>>& map, const int& textLen);
void printText(const string& text);

/* Mian program */
int main() {
    intro();
    int nValue; // declare N-grams which will be input by user later
    Map<Queue<string>, Vector<string>> map; // key is queue containing prefixes of N-1 words,
    // and value is vector containing all subsequent suffixes
    string output; // declare random generated text
    inputFile(nValue, map);
    inputTextLength(nValue, map, output);
    cout << "Exiting." << endl;
    return 0;
}

/*
 * Function: intro
 * Usage: intro();
 * ---------------
 * Print a welcome message to introduce the program.
*/
void intro() {
    cout << "Welcome to CS 106B/X Random Writer ('N-Grams')!" << endl;
    cout << "This program generates random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups" << endl;
    cout << "of words, and I'll create random text for you." << endl << endl;
}

/*
 * Function: inputFile
 * Usage: inputFile(nValue, map);
 * ------------------------------
 * This function prompts the user to input a text file and an integer N for N-grams.
 * Then the function will call buildMap function to read the file and build the map.
*/
void inputFile(int& nValue, Map<Queue<string>, Vector<string>>& map) {
    ifstream infile; // input file
    promptUserForFile(infile, "Input file name?");
    // Prompt the user to input nValue again if the current input integer is less than 2
    while(true) {
        nValue = getInteger("Value of N?");
        if(nValue >= 2) {
            break; // exit the loop if the input nValue is valid
        } else {
            cout << "N must be 2 or greater." << endl;
        }
    }
    cout << endl;
    buildMap(infile, nValue, map); // pass the input file, map and nValue to buildMap function to build map
    infile.close(); // close input file once reading finished
}

/*
 * Function: buildMap
 * Usage: buildMap(infile, nValue, map);
 * -------------------------------------
 * This function is to build the map while reading the input text file word by word.
 * Then the function will call wrapMap function to wrap map around.
*/
void buildMap(ifstream& infile, const int& nValue, Map<Queue<string>, Vector<string>>& map) {
    string word; // current word when reading the file
    Queue<string> window; // a sliding window containing N-1 words
    Queue<string> frontWords; // keeps front words which will be used later for wraping the map around
    // iterate across infile word by word
    while(infile >> word) {
        // build the first sliding window
        if(window.size() < nValue-1) {
            window.enqueue(word);
            frontWords.enqueue(word);
        } else {
            map[window].add(word); // add the suffix for the current window into the corresponding vector
            window.dequeue(); // dequeue the first word inside the window
            window.enqueue(word); // shift the window by one word by adding the suffix
        }
    }
    // Assume the infile length is not less than N-1. Otherwise throw an error.
    if(window.size() < nValue-1) {
        throw "Assertion failed: user typed value of N that exceeds size of file.";
    }
    wrapMap(map, window, frontWords); // wrap map around
}

/*
 * Function: wrapMap
 * Usage: wrapMap(map, window, frontWords);
 * ----------------------------------------
 * This function is to wrap the map around when file reading reaches the end. The last words will utilize
 * frontWords saved earlier as suffixes.
*/
void wrapMap(Map<Queue<string>, Vector<string>>& map, Queue<string>& window, Queue<string>& frontWords) {
    // iterate across front words
    while(!frontWords.isEmpty()) {
        string curr = frontWords.dequeue();
        map[window].add(curr); // add the each front word as suffix
        if(frontWords.isEmpty()) {
            break; // exit the loop once front words have been all added
        }
        window.dequeue();
        window.enqueue(curr); // some last words will combine some front words to form new window
    }
}

/*
 * Function: inputTextLength
 * Usage: inputTextLength(nValue, map, text);
 * ------------------------------------------
 * This function is to prompt the user to input length for generated random text. Once the input length is
 * valid, it will call generateText function to generate text and printText function to print the text.
 * Then it will ask whether the user wants to play again.
*/
void inputTextLength(const int& nValue, const Map<Queue<string>, Vector<string>>& map, string& text) {
    int response = -1; // initialize input length
    // loop to ask whether the user wants to continue. 0 for exit.
    while(response != 0) {
        // prompt the user to input the length again if the previous one is not valid
        while(true) {
            response = getInteger("# of random words to generate (0 to quit)?");
            if(response == 0 || response >= nValue) {
                break; // exit the loop if the input length is valid
            } else {
                cout << "Must be at least " << nValue << " words." << endl << endl;
            }
        }
        if(response != 0) {
            text = generateText(nValue, map, response); // generate random text
            printText(text); // print generated text
        }
    }
}

/*
 * Function: generateText
 * Usage: string text = generateText(nValue, map, textLen);
 * --------------------------------------------------------
 * This function is to generate random text of user-required length.
*/
string generateText(const int& nValue, const Map<Queue<string>, Vector<string>>& map,  const int& textLen) {
    int n = textLen; // make copy of text length
    string text; // initialize output text
    Vector<Queue<string>> keys; // hold all keys from the map
    Queue<string> window;
    // add all keys into the vector for the ease of randomly picking a starting point later
    for(Queue<string> window : map) {
        keys.add(window);
    }
    int keyLen = keys.size(); // the number of all keys
    // iterate to generate text word by word, from 1st word to (n-(nValue-1))th word.
    while(n > nValue-1) {
        // randomly select a key from the map at first
        if(n == textLen) {
            int index = randomInteger(0, keyLen-1);
            window = keys[index];
        }
        Vector<string> nextWords = map[window]; // find the corresponding collection of suffixes
        int len = nextWords.size(); // size of the suffixes vector
        int index = randomInteger(0, len-1);
        string word = nextWords[index]; // randomly pick up next word
        text.append(window.dequeue()+" "); // extract the first word in the window and append it into output
        window.enqueue(word); // shift the window by adding the next word
        n--;
    }
    // iterate across the last window to add the last (nValue-1) words into output text
    while(!window.isEmpty()) {
        text.append(window.dequeue()+" ");
    }
    return text;
}

/*
 * Function: printText
 * Usage: printText(text);
 * -----------------------
 * Print generated random text.
*/
void printText(const string& text) {
    cout << "... " << text << "..." << endl << endl;
}
