/*
 * File: boggle.cpp
 * -----------------------
 * Assignment 4 Part C Coding Boggle
 * This program mainly plays a board game between human and computer. The board is a 4x4 sqaure grid of
 * letter cubes. At the start of game, user has two options: enter a string of 16 letter characters, or
 * lay letter cubes out randomly on the board. The human player plays first, entering words one by one,
 * at least 4 words long. The program will validate the word, search on board using backtracking and
 * accumulate the scores. Then the computer searches through the board using recursive backtracking to
 * find all the possible words that can be formed. Human can continue playing while enter is to quit.
 *
 * @author Liu Jiang (Caci)
 * @version 10/21/2019
*/

#include "boggle.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "lexicon.h"
#include "random.h"
#include "set.h"
#include "shuffle.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "gui.h"
using namespace std;

// function prototype declarations
void intro();
void menu();
bool allLetters(string str);
void generateBoard(Grid<char>& board);
void loadDictionary(Lexicon& dictionary, string filename);
int humanTurn(Grid<char> board, Lexicon& dictionary, Set<string>& humanWords);
bool preValidateWord(Lexicon& dictionary, Set<string>& humanWords, string word);
bool humanWordSearch(Grid<char>& board, string word);
int calculatePoints(string word);
bool DFS1(Grid<char>& board, Grid<bool>& isVisited, int i, int j, string word);
int computerTurn(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords);
Set<string> computerWordSearch(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords);
void DFS2(Grid<char>& board, Grid<bool>& isVistied, Lexicon& dictionary, Set<string>& computerWords,
          Set<string>& humanWords, int i, int j, string word);

/* Mian program */
int main() {
    gui::initialize(BOARD_SIZE, BOARD_SIZE);
    intro();
    menu();
    cout << "Have a nice day." << endl;
    return 0;
}

/*
 * Prints a welcome message that introduces the program to the user.
*/
void intro() {
    cout << "Welcome to CS 106B Boggle!" << endl;
    cout << "This game is a search for words on a 2-D board of letter cubes." << endl;
    cout << "The good news is that you might improve your vocabulary a bit." << endl;
    cout << "The bad news is that you're probably going to lose miserably to" << endl;
    cout << "this little dictionary-toting hunk of silicon." << endl;
    cout << "If only YOU had 16 gigs of RAM!" << endl;
    cout << endl;
}

/*
 * This function will ask whether the user wants to enter the game at first, and then call loadDictionary
 * function to load dictionary file, call generateBoard function to generate boad of letters, call
 * humanTurn to let human play at first, call computerTurn function to allow computer to play, and then
 * compare the two scores. Finally it will ask whether the user wants to continue.
*/
void menu() {
    string startChoice = getLine("Press Enter to begin the game ...");
    if(startChoice.empty()) {
        Lexicon dictionary;
        loadDictionary(dictionary, DICTIONARY_FILE); // load dictionary file
        bool playAgain = false;
        do {
            if(playAgain) {
                gui::initialize(BOARD_SIZE, BOARD_SIZE); // clear the board on GUI if user plays again
            }
            cout << endl;
            Grid<char> board(BOARD_SIZE, BOARD_SIZE);
            generateBoard(board); // generate board
            cout << "It's your turn!" << endl;
            Set<string> humanWords; // initialize the set which will store validated words from human
            int humanScore = humanTurn(board, dictionary, humanWords); // human's turn
            int computerScore = computerTurn(board, dictionary, humanWords); // computer's turn
            // compare the scores
            if(humanScore < computerScore) {
                cout << "Ha ha ha, I destroyed you. Better luck next time, puny human!" << endl << endl;
            } else {
                cout << "WOW, you defeated me! Congratulations!" << endl << endl;
            }
            playAgain = getYesOrNo("Play again?");
        } while(playAgain); // loop again if the user decides to continue
    }
}

/*
 * This function will generate 4x4 sqaure grid of letter cubes. User is provided with two options.
 * Option1: "shake" the letter cubes to randomize the letters.
 * Option2: manual board configuration, which is that user enters a string of 16 characters representing
 * the cubes from left to right, top to bottom.
 * @param board is the square grid
*/
void generateBoard(Grid<char>& board) {
    string boardString = ""; // initialize the string which will form the board letters
    if(getYesOrNo("Generate a random board?")) {
        // option1
        for(int i = 0; i < LETTER_CUBES.size(); i++) {
            string letters = LETTER_CUBES[i];
            // randomly choose the letter from each cube
            char letter = letters[size_t(randomInteger(0, int(letters.length())-1))];
            boardString = boardString+letter;
        }
        // each cube letter is placed at a random location
        boardString = shuffle(boardString);
    } else {
        // option 2
        // loop untill the user inputs a valid board string
        while(true) {
            boardString = getLine("Type the 16 letters on the board:");
            boardString = toUpperCase(boardString);
            if(boardString.length() == BOARD_SIZE*BOARD_SIZE && allLetters(boardString)) {
                break; // exit the loop if the string's length is 16, only containing letters
            }
            cout << "Invalid board string. Try again." << endl;
        }
    }
    gui::labelCubes(boardString); // display grid on GUI
    // assign each letter of the string into grid, and display the grid on console
    for(size_t i = 0; i < boardString.length(); i++) {
        board[int(i)/BOARD_SIZE][int(i)%BOARD_SIZE] = boardString[i];
        cout << boardString[i];
        if(i%BOARD_SIZE == BOARD_SIZE-1) {
            cout << endl;
        }
    }
    cout << endl;
}

/*
 * This function is to check whether the user-input string only contains letters
 * @param str is user-input string
 * @return a boolean: true if input string only contains letters
*/
bool allLetters(string str) {
    for(size_t i = 0; i < str.length(); i++) {
        // note that str has been capitalized before
        if(!(str[i] >= 'A' && str[i] <= 'Z')) {
            return false;
        }
    }
    return true;
}

/*
 * This function is to load dictionary file into a lexicon.
 * @param dictionary is the lexicon which will be filled with valid English words
 * @param filename is the file name of dictionary file
*/
void loadDictionary(Lexicon& dictionary, string filename) {
    ifstream infile;
    if(openFile(infile, filename)) {
        string word;
        // iterate across dictionary file word by word
        while(infile >> word) {
            dictionary.add(word);
        }
    }
}

/*
 * This function allows human to play the game. It will prompt user to input word he/she spots and
 * validate the word by considering some constraints and calling humanWordSearch function to find
 * whether the word can be formed on the board. It will also calculate the scores of human player.
 * Enter is to exit human's turn.
 * @param board is the square grid
 * @param dictionary containing all valid English words
 * @param humanWords is the set containing valid words already found by human
 * @return humanScore which is the total scores of human player
*/
int humanTurn(Grid<char> board, Lexicon& dictionary, Set<string>& humanWords) {
    bool exitFlag = false; // initialize the boolean whether human wants to quit his/her turn
    string word; // initialize the input word by human
    int humanScore = 0; // initialize the score of human
    while(!exitFlag) {
        cout << "Your words: " << humanWords << endl;
        cout << "Your score: " << humanScore << endl;
        // ask the user to re-input if the word fails to meet some constraints
        while(true) {
            word = getLine("Type a word (or Enter to stop):");
            toUpperCaseInPlace(word);
            if(word.empty()) {
                exitFlag = true;
                break; // enxit loop if the user wants to quit
            } else if(preValidateWord(dictionary, humanWords, word)) {
                // call helper function to validate input word
                break; // exit loop if input word does not violate constraints
            }
        }
        if(!exitFlag) {
            // call humanWordSearch function to search the word on board
            if(humanWordSearch(board, word)) {
                // add the word to set and update scores if the word can be formed on board
                cout << "You found a new word! \"" << word << "\"" << endl << endl;
                humanWords.add(word);
                gui::recordWord("human", word);
                humanScore += calculatePoints(word);
                gui::setScore("human", humanScore);
            } else {
                cout << "That word can't be formed on this board." << endl;
            }
        }
    }
    cout << endl;
    return humanScore;
}

/*
 * This function is to validate the input word by considering whether it is included in dictionary,
 * whether its length is larger than MIN_WORD_LENGTH, and whether it has already been found by human
 * player.
 * @param dictionary containing all valid English words
 * @param humanWords is the set containing valid words already found by human
 * @param word is input word by user
 * @return boolean: true if input word meets all requirements
*/
bool preValidateWord(Lexicon& dictionary, Set<string>& humanWords, string word) {
    if(!dictionary.contains(toLowerCase(word))) {
        cout << "That word is not found in the dictionary." << endl;
        return false;
    }
    if(word.length() < MIN_WORD_LENGTH) {
        cout << "The word must have at least " << MIN_WORD_LENGTH << " letters." << endl;
        return false;
    }
    if(humanWords.contains(word)) {
        cout << "You have already found that word." << endl;
        return false;
    }
    return true;
}

/*
 * This function will start from each cube of the board to call a helper function DFS1 to find whether the
 * word can be formed on board.
 * @param board is the square grid
 * @param word is input word by user
 * @return a boolean: true if the word can be found on board
*/
bool humanWordSearch(Grid<char>& board, string word) {
    gui::clearHighlighting();
    // initialize a boolean grid to mark whether the cube has been searched during the recursion
    Grid<bool> isVisited(BOARD_SIZE, BOARD_SIZE, false);
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(board[i][j] == word[0]) {
                if(DFS1(board, isVisited, i, j, word)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 * This is a helper function to find whether user's input word can be formed, by using recursion and
 * backtracking.
 * Base case 1: the word is empty, which means the search of word is successful. Base case 2: search
 * point is out of grid. Base case 3: if the search point has been visited before.
 * Recusive case: for each direction (left, right, up, down, diagonal), call itself to continue searching
 * @param board is the square grid
 * @param isVisited is the boolean grid to mark whether the cube has been visited during the search
 * @param i is the row number of current point
 * @param j is the column number of current point
 * @param word contains characters not yet formed on board
 * @return a boolean: true if the word can be formed.
*/
bool DFS1(Grid<char>& board, Grid<bool>& isVisited, int i, int j, string word) {
    // base case 1
    if(word == "") {
        return true;
    }
    // base case 2
    if(i < 0 || i >= BOARD_SIZE || j < 0 || j >= BOARD_SIZE) {
        return false;
    }
    // base case 3
    if(isVisited[i][j]) {
        return false;
    }
    isVisited[i][j] = true; // choose the current point
    gui::setHighlighted(i, j, true);
    // recursive case
    bool isMatch = false;
    // if current point matches the first character of word
    if(board[i][j] == word[0]) {
        string nextWord = word.substr(1); // rest part of word
        // loop eight direactions surrounding the current point
        for(int r = -1; r <= 1 && !isMatch; r++) {
            for(int c = -1; c <= 1; c++) {
                if(!(r == 0 && c == 0)) {
                    isMatch = isMatch || DFS1(board, isVisited, i+r, j+c, nextWord);
                    if(isMatch) {
                        return true; // successfully find the word if any of the direction returns true
                    }
                }
            }
        }
    }
    isVisited[i][j] = false; // unchoose the current point
    gui::setHighlighted(i, j, false);
    return false;
}

/*
 * This function is to calculate the score of a word based on its length.
 * @param word is either found by human player or computer player.
 * @return an integer which is the score of the word
*/
int calculatePoints(string word) {
    size_t len = word.length();
    switch(len) {
    case 4:
        return 1;
    case 5:
        return 2;
    case 6:
        return 3;
    case 7:
        return 5;
    default:
        return 11;
    }
}

/*
 * This function allows computer to take its turn. Computer player will call computerWords function to
 * find all possible valid English words on board, excluding the ones already found by human player. It
 * will also update the set of words found by computer and calculate its scores.
 * @param board is the square grid
 * @param dictionary containing all valid English words
 * @param humanWords is the set containing valid words already found by human
 * @return computerScore is the total points earned by computer player
*/
int computerTurn(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords) {
    int computerScore = 0; // initialize the score of computer player
    cout << "It's my turn!" << endl;
    // initialize the set of words found by computer
    // call computerWords function to conduct exaustive search
    Set<string> computerWords = computerWordSearch(board, dictionary, humanWords);
    // loop the set of computer's words to update its scores
    for(string word : computerWords) {
        computerScore += calculatePoints(word);
        gui::recordWord("computer", word);
    }
    cout << "My words: " << computerWords << endl;
    cout << "My score: " << computerScore << endl;
    gui::setScore("computer", computerScore);
    return computerScore;
}

/*
 * This function will loop each point on board to start a recursive search of possible valid words by
 * calling a helper function of DFS2.
 * @param board is the square grid
 * @param dictionary containing all valid English words
 * @param humanWords is the set containing valid words already found by human
 * @return words is the set of words found by computer
*/
Set<string> computerWordSearch(Grid<char>& board, Lexicon& dictionary, Set<string>& humanWords) {
    gui::clearHighlighting();
    Set<string> words; // initialize the set of computer's words
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            // initialize a empty string which will be concatenated with board's character
            string prefix = "";
            // initialize boolean grid to mark visited points
            Grid<bool> isVisited(BOARD_SIZE, BOARD_SIZE, false);
            // call DFS2 function to do backtracking search
            DFS2(board, isVisited, dictionary, words, humanWords, i, j, prefix);
        }
    }
    return words;
}

/*
 * This function is to find all possible words from a starting point by using recursion and backtracking,
 * and add them to computer's words set.
 * Base case 1: current point is out of board. Base case 2: current point has already been visited.
 * Base case 3: the dictionary does not contain current word as prefix, then immediately stop current
 * search.
 * Recursive case: for each direction (left, right, up, down, diagonal), call itself to continue searching
 * @param board is the square grid
 * @param isVisited is the boolean grid to mark visited points
 * @param dictionary containing all valid English words
 * @param computerWords is the set of words already found by computer
 * @param humanWords is the set containing valid words already found by human
 * @param i is the row number of current point
 * @param j is the column number of current point
 * @param word contains characters currently found by computer
*/
void DFS2(Grid<char>& board, Grid<bool>& isVistied, Lexicon& dictionary, Set<string>& computerWords,
          Set<string>& humanWords, int i, int j, string word) {
    // base case 1
    if(i < 0 || i >= BOARD_SIZE || j < 0 || j >= BOARD_SIZE) {
        return;
    }
    // base case 2
    if(isVistied[i][j]) {
        return;
    }
    // base case 3
    if(!dictionary.containsPrefix(toLowerCase(word))) {
        return;
    }
    isVistied[i][j] = true; // choose the current point
    word += board[i][j];
    // add current word to computer's words set if it meets the requirements
    if(word.length() >= MIN_WORD_LENGTH && dictionary.contains(toLowerCase(word)) &&
            !(humanWords.contains(word) || computerWords.contains(word))) {
        computerWords.add(word);
    }
    // loop across eight directions to continue search
    for(int r = -1; r <= 1; r++) {
        for(int c = -1; c <= 1; c++) {
            if(!(r == 0 && c == 0)) {
                 DFS2(board, isVistied, dictionary, computerWords, humanWords, i+r, j+c, word);
            }
        }
    }
    isVistied[i][j] = false; // unchoose the current point
}
