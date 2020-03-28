/*
 * Assignment 1 Part C Coding MadLibs
 * "Mad Libs" are short stories that have blanks called placeholders to be filled in.
 * @author Liu Jiang (Caci)
 * @version 10/01/2019
*/

#include <fstream> // for ifstream
#include <iomanip> // for setprecision, fixed
#include <iostream> // for cout, endl
#include <sstream>
#include <string> // for string class
#include "console.h" // for graphical console
#include "filelib.h" // for promptUserForFile
#include "simpio.h" // for getLine, getYesOrNo
#include "strlib.h"

using namespace std;

// Function prototype declarations
void intro();

int inputFile(string& outText);

void userFillIn(string& text, int& counter);

bool printStory(const string& outText, const int& counterPH, int& counterT, int& counterS);

void endGame(const int& counterT, const int& counterS);

int main() {
    intro();
    int counterStory = 0; // counts the number of Mad Lib stories created by user
    int counterTotal = 0; // counts total placeholders filled by user
    bool flag; // whether the user wants to continue playing Mad Lib
    // continue reading input, prompting user's response and printing story if the user wants to play again
    do{
        string output; // buffer for story which will be printed later
        int counterPlaceHolders = inputFile(output);
        flag = printStory(output, counterPlaceHolders, counterTotal, counterStory);
    } while(flag);
    cout << endl;
    endGame(counterTotal, counterStory);
    return 0;
}

// Print out introduction
void intro() {
    cout << "Welcome to CS 106B Mad Libs!" << endl;
    cout << "I will ask you to provide various words" << endl;
    cout << "and phrases to fill in a story." << endl;
    cout << "At the end, I will display your story to you." << endl << endl;
}

// Read input file, and store the text and response filled in by user into a buffer string.
int inputFile(string& outText) {
    /*
     * parameter: outText, which is the buffer string to store output
     * return: counter, which counts the number of placeholders in one Mad Lib story
*/
    ifstream infile; // input file
    int counter = 0;
    promptUserForFile(infile, "Mad Lib input file?");
    cout << endl;
    string line; // iterate across infile line by line
    while(getline(infile, line)) {
        if((line.find("<") != string::npos) && (line.find(">") != string::npos)) {
            // Once a pair of <> encountered in a line, call userFillIn function to
            // to manipulate that line by replacing placeholder by response
            // and increase the counter of placeholders.
            userFillIn(line, counter);
        }
       outText.append(line+"\n"); // update output string line by line
    }
    infile.close(); // close input file once reading finished
    cout << endl;
    return counter;
}

// Prompt the user for response
void userFillIn(string& text, int& counter) {
    /*
     * parameters: text, which is the line manipulated later
     *             counter, which is the number of placeholders (increased per valid token)
*/
    while((text.find("<") != string::npos) && (text.find(">") != string::npos)) {
        counter++; // update counter for placeholders in one story
        string response; // user's response assigned later
        int left = text.find('<'); // index of '<'
        int right = text.find('>'); // index of '>'
        string placeholder = text.substr(left+1, right-left-1);
        char firstChar = placeholder.at(0); // first character of placeholder
        string prompt; // prompt message appearing on console later
        // control whether the placeholder begins with a vowel
        if(firstChar == 'a' || firstChar == 'A' || firstChar == 'e' || firstChar == 'E' || firstChar == 'i'
                || firstChar == 'I' || firstChar == 'o' || firstChar == 'O' || firstChar == 'u'
                || firstChar == 'U') {
            prompt = "Please type an ";
        } else {
            prompt = "Please type a ";
        }
        prompt.append(placeholder+":");
        response = getLine(prompt); // store user's response
        text.replace(left, right-left+1, response); // replace placeholder with user's response
    }
}

// Print out the story
bool printStory(const string& outText, const int& counterPH, int& counterT, int& counterS) {
    /*
     * parameters: outText, which is the story for output
     *             counterPH, which is the number of replaced placeholders in that Mad Lib story
     *             counterT, which is the number of total replaced placeholders in all created Mab Lib stories
     *             counterS, which is the number of created stories
     * return response, which is bool type about whether the user wants to do next Mad Lib story
*/
    cout << "Your Mad Lib story:" << endl;
    cout << outText << endl;
    cout << counterPH << " placeholder(s) replaced." << endl;
    counterT += counterPH; // update the counter of total placeholders
    counterS++; // update the counter for Mad Lib stories
    bool response = getYesOrNo("Do another Mad Lib (Y/N)?"); // ask whether the user wants to continue
    return response;
}

// Print out the statistics about the number of created stories and total placeholders
// when user stops playing Mad Lib story
void endGame(const int& counterT, const int& counterS) {
    /*
     * parameters: counterT, which is the number of total replaced placeholders in all created Mab Lib stories
     *             counterS, which is the number of created stories
*/
    cout << "Mad Lib stories you created: " << counterS << endl;
    // convert to double type for how many placeholders per story by multiplying 1.0
    cout << "Total placeholders replaced: " << counterT << " (" << setprecision(1) << fixed
         << 1.0*counterT/counterS << " per story)" << endl;
}
