/*
 * File: encoding.cpp
 * -----------------------
 * Assignment 7 Part C Huffman Coding
 *
 * This file provides the implementation of a data compression and decompression operation using binary
 * trees, which is in the form of Huffman Tree. This program mainly achieves compress and decompress
 * operations, accompanied by a series of helper functions to assis encoding and decoding. The encoded
 * output file would be customized HuffmanFile, including header as the guidance to decode the content.
 *
 * @author Liu Jiang (Caci)
 * @version 12/05/2019
*/

#include "encoding.h"
#include "priorityqueue.h"
#include "filelib.h"
using namespace std;

/*
 * This function will build a map, of which key is the character that occurs in input file, and value is
 * the character's corresponding frequency.
 * @param input is input file stream.
 * @return a map that records character and corresponding frequency.
*/
Map<char, int> buildFrequencyTable(istream& input)
{
    Map<char, int> freqTable;
    char ch; // initialize the buffer to store character later read from input file
    // read file line by line
    while(input.get(ch)) {
        freqTable[ch]++;
    }
    return freqTable;
}

/*
 * This function is to construct binary tree, also Huffman Tree, according to frequency table.
 * @param freqTable is a map that records character and corresponding frequency.
 * @return the root of binary tree.
*/
HuffmanNode* buildEncodingTree(Map<char, int>& freqTable)
{
    PriorityQueue<HuffmanNode*> pq; // initialize priorityqueue to assist in building binary tree
    // loop over frequency table to create and add each leaf HuffmanNode to priorityqueue
    for(char ch : freqTable.keys()) {
        HuffmanNode* node = new HuffmanNode(ch);
        pq.enqueue(node, freqTable[ch]);
    }
    // construct the tree by combining nodes untile only root node remains in the priorityqueue
    while(pq.size() > 1) {
        double leftPriority = pq.peekPriority();
        HuffmanNode* leftNode = pq.dequeue();
        double rightPriority = pq.peekPriority();
        HuffmanNode* rightNode = pq.dequeue();
        HuffmanNode* node = new HuffmanNode(leftNode, rightNode);
        pq.enqueue(node, leftPriority+rightPriority);
    }
    return pq.dequeue();
}

/*
 * This is a helper function for flattening the tree into a string header, which conducts pre-order
 * traversal over the tree.
 * @param node is current tree node
 * @param res is the string to be built in this process
*/
void preorderFlatten(HuffmanNode* node, string& res) {
    if(node->isLeaf()) {
        res += string(".")+node->ch;
    } else {
        res += "(";
        preorderFlatten(node->zero, res);
        preorderFlatten(node->one, res);
        res += ")";
    }
}

/*
 * This function is to create a header string for the latter decoding purpose, which will implement
 * a helper function.
 * @param t is the root of binary tree.
 * @return header string.
*/
string flattenTreeToHeader(HuffmanNode* t)
{
    string res = "";
    preorderFlatten(t, res);
    return res;
}

/*
 * This function is to recreate the binary tree based on the header string. Assume the input header is
 * valid.
 * @param str is the input header string
 * @return the root of binary tree
*/
HuffmanNode* recreateTreeFromHeader(string str)
{
    Stack<HuffmanNode*> stack;
    HuffmanNode* root = nullptr; // initialize the pointer to the root
    for(size_t i = 0; i < str.length(); i++) {
        char ch = str.at(i);
        if(ch == '(') {
            continue; // do nothing if current character is open parenthese
        } else if(ch == ')') {
            // combine node if current character is close parenthese
            HuffmanNode* rightNode = stack.pop();
            HuffmanNode* leftNode = stack.pop();
            HuffmanNode* node = new HuffmanNode(leftNode, rightNode);
            if(i == str.length()-1) {
                // update root node if we reach the end of header
                root = node;
            } else {
                stack.push(node);
            }

        } else if(ch == '.') {
            // create leaf node based on the character following a dot character
            char letter = str.at(i+1);
            HuffmanNode* node = new HuffmanNode(letter);
            stack.push(node);
            i++;
        }
    }
    return root;
}

/*
 * This is a helper function that perform pre-order traversal over the tree to build up ecoding map.
 * @param encodingMap contains character for key and corresponding binary code for value.
 * @param coding is currently formed code.
 * @param node is current node in tree.
*/
void traverseTree(Map<char, string> & encodingMap, string coding, HuffmanNode* node) {
    if(node->isLeaf()) {
        encodingMap[node->ch] = coding;
    } else {
       traverseTree(encodingMap, coding+"0", node->zero);
       traverseTree(encodingMap, coding+"1", node->one);
    }
}

/*
 * This function is to build ecoding map based on Huffman tree.
 * @param encodingTree is the root of binary tree.
 * @return encodingMap contains character for key and corresponding binary code for value.
*/
Map<char, string> buildEncodingMap(HuffmanNode* encodingTree)
{
    Map<char, string> encodingMap;
    string prefix = "";
    traverseTree(encodingMap, prefix, encodingTree);
    return encodingMap;
}

/*
 * This function is to delete binary tree and free up its occupied memory, of which the implementation
 * follows post-order traversal.
 * @param t is the root of binary tree.
*/
void freeTree(HuffmanNode* t)
{
    if(t == nullptr) {
        return;
    }
    freeTree(t->zero);
    freeTree(t->one);
    delete t;
}

/*
 * This function is to compress the input stream and create a HuffmanOutputFile by gathering a series of
 * helper functions. HuffmanOutputFile will contain header and codes derived from original characters.
 * @param input is input stream.
 * @param HuffmanOutputFile is the output file.
*/
void compress(istream& input, HuffmanOutputFile& output)
{
    Map<char, int> freqTable = buildFrequencyTable(input);
    HuffmanNode* root = buildEncodingTree(freqTable);
    Map<char, string> encodingMap = buildEncodingMap(root);
    string header = flattenTreeToHeader(root);
    output.writeHeader(header);
    rewindStream(input); // read input stream again
    string coding;
    char ch;
    // iterate across input stream to write down binary code
    while (input.get(ch)) {
            coding = encodingMap[ch];
            for (size_t i = 0; i < coding.length(); i++)
                output.writeBit((coding.at(i)-'0'));
    }
    freeTree(root); // free the memory occupied by tree
}

/*
 * This function is to decompress HuffmanInputFile and retrieve its original stream. At first, it will
 * recreate the binary tree according to input file's header. Then it reads binary codes and write down
 * original text by referring to encoding tree.
 * @param input is the input huffmanInputFile.
 * @output is the output stream.
*/
void decompress(HuffmanInputFile& input, ostream& output)
{
    string header = input.readHeader(); // get header
    HuffmanNode* root = recreateTreeFromHeader(header); // construct binary tree
    HuffmanNode* curr = root;
    while(true) {
        int bit = input.readBit();
        // end of file
        if(bit == -1) {
            break;
        }
        if(bit == 0) {
            curr = curr->zero;
        } else if(bit == 1) {
            curr = curr->one;
        }
        // put character into stream if it reaches a leaf node and restart at root node for decoding next
        // character
        if(curr->isLeaf()) {
            output.put(curr->ch);
            curr = root;
        }
    }
    freeTree(root); // free the memory occupied by tree
}
