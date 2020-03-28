/*
 * File: tilelist.cpp
 * -----------------------
 * Assignment 6 Part C Tiles
 *
 * This file provides the implementation of the bodies of the member functions, constructor and destructor
 * for the TileList class declared in tilelist.h. The tile list is formed by doubly linked tile nodes. The
 * constructor creates front pointer and back pointer, to the top and bottom of the list respectively.
 * Member functions allow clients to add nodes at the front or back of the list, highlight a single node,
 * raise or lower a node, merge nodes, and remove nodes.
 *
 * @author Liu Jiang (Caci)
 * @version 11/15/2019
*/


#include "tilelist.h"
#include <algorithm>
#include "strlib.h"
#include <cmath> // for max and min
using namespace std;

TileList::TileList() {
    _front = nullptr; // initialize front pointer
    _back = nullptr; // initialize back pointer
}

TileList::~TileList() {
    // call helper function of detachNode to delete node one by one
   while(_front != nullptr) {
       detachNode(_front);
   }
   _back = nullptr; // set back pointer to null so it won't point to a dirty address
}

void TileList::addBack(int x, int y, int width, int height, string color) {
    // create a new node
    TileNode* newNode = new TileNode(x, y, width, height, color);
    // set both front and back pointers to the new node if the list is empty
    if(_front == nullptr && _back == nullptr) {
        _front = newNode;
        _back = newNode;
    } else {
        // otherwise only consider manipulating back pointer
        _back->next = newNode;
        newNode->prev = _back;
        _back = _back->next;
    }
}

void TileList::addFront(int x, int y, int width, int height, string color) {
    // create a new node
    TileNode* newNode = new TileNode(x, y, width, height, color);
    // set both front and back pointers to the new node if the list is empty
    if(_front == nullptr && _back == nullptr) {
        _front = newNode;
        _back = newNode;
    } else {
        // otherwise only consider manipulating back pointer
        _front->prev = newNode;
        newNode->next = _front;
        _front = _front->prev;
    }
}


void TileList::clear() {
    // call helper function of detachNode to delete node one by one
    while(_front != nullptr) {
        detachNode(_front);
    }
    _back = nullptr; // set back pointer to null so it won't point to a dirty address
}

void TileList::debug() {
    TileNode* curr = _front; // create a new pointer to the front
    int index = 0;
    // iterate across the list from top to bottom and print each node's x coordinate
    while(curr != nullptr) {
        cout << "no." << index++ << " x: " << curr->x << endl;
        curr = curr->next;
    }
    TileNode* curr2 = _back; // create a new pointer to the back
    index--;
    // iterate across the list from bottom to top and print each node's x coordinate
    while(curr2 != nullptr) {
        cout << "no." << index-- << " x: " << curr2->x << endl;
        curr2 = curr2->prev;
    }
    cout << "front x: " << getFront()->x << endl; // test getFront function
    cout << "back x: " << getBack()->x << endl; // test getBack function
}

void TileList::drawAll(GWindow& window) const {
    TileNode* curr = _back; // initialize a pointer to the back
    // draw the tile node one by one from bottom to top
    while(curr != nullptr) {
        curr->draw(window);
        curr = curr->prev;
    }
}

TileNode* TileList::getBack() const {
    return _back; // get the back pointer
}

TileNode* TileList::getFront() const {
    return _front; // get the front pointer
}

bool TileList::highlight(int x, int y) {
    TileNode* curr = searchNode(x, y); // call helper function of searchNode and point to the target node
    if(curr == nullptr) {
        return false; // if target node is not found
    }
    curr->color = "yellow"; // change the target node's color to yellow
    return true;
}

bool TileList::lower(int x, int y) {
    TileNode* curr = searchNode(x, y); // call helper function of searchNode and point to the target node
    if(curr == nullptr) {
        return false; // if target node is not found
    }
    if(curr != _back) { // do nothing if the target node is at bottom
        curr->next->prev = curr->prev;
        if(curr != _front) {
            curr->prev->next = curr->next;
        } else {
            _front = curr->next; // handle the special case that target node is at top
        }
        curr->next = nullptr; // move the target node to the bottom
        curr->prev = _back;
        _back->next = curr;
        _back = curr; // update back pointer
    }
    return true;
}

void TileList::merge(int x, int y) {
    // initialize the coordinates and color for merged rectangle
    int x1 = INT_MAX, x2 = INT_MIN, y1 = INT_MAX, y2 = INT_MIN;
    string color = "";
    // keep searching the list if there's still node covering user-clicked location
    while(true){
        TileNode* curr = searchNode(x, y); // call searchNode and point to the target node
        if(curr != nullptr) {
            // if there's target node, update leftmost, rightmost, topmost and bottommost coordinates
            x1 = min(x1, curr->x);
            x2 = max(x2, curr->x+curr->width);
            y1 = min(y1, curr->y);
            y2 = max(y2, curr->y+curr->height);
            if(color.empty()) {
                color = curr->color; // assign the color of topmost target node
            }
            remove(x, y);
        } else{
            break; // exit the loop if no target node found
        }
    }
    int width = x2-x1;
    int height = y2-y1;
    if(!color.empty()) {
        addFront(x1, y1, width, height, color); // add the merged node at front
    }
}

bool TileList::raise(int x, int y) {
    TileNode* curr = searchNode(x, y); // call helper function of searchNode and point to the target node
    if(curr == nullptr) {
        return false; // if target node is not found
    }
    if(curr != _front) { // do nothing if the target node is at top
        curr->prev->next = curr->next;
        if(curr != _back) {
            curr->next->prev = curr->prev;
        } else {
            _back = curr->prev; // handle the special case that target node is at bottom
        }
        curr->prev = nullptr;
        curr->next = _front; // move the target node to the front
        _front->prev = curr;
        _front = curr; // update front pointer
    }
    return true;
}

bool TileList::remove(int x, int y) {
    TileNode* curr = searchNode(x, y); // call helper function of searchNode and point to the target node
    if(curr == nullptr) {
        return false; // if target node is not found
    }
    if(_front == _back) { // if target node is the only node in the list
        detachNode(_front);
        _back = nullptr; // set both front and back pointers to null
    } else {
        if(curr == _front) { // handle the special case that target node is at top
            _front = _front->next;
            _front->prev = nullptr;
        } else if(curr == _back) { // handle the special case that target node is at bottom
            _back = _back->prev;
            _back->next = nullptr;
        } else {
            curr->prev->next = curr->next; // case that target node is at middle of the list
            curr->next->prev = curr->prev;
        }
        detachNode(curr); // call detachNode to remove target node
    }
    return true;
}

int TileList::removeAll(int x, int y) {
    int counter = 0; // initialize the counter to record how many nodes have been deleted
    // keep searching the list if there's still node covering user-clicked location
    while(remove(x, y)) {
        counter++; // increment the counter if successfully remove a target node
    }
    return counter;   // remove this
}

void TileList::detachNode(TileNode*& curr) {
    // delete the node if curr does not point to null
    if(curr != nullptr) {
        TileNode* trash = curr;
        curr = curr->next;
        delete trash; // free up the memory occupied by that node
    }
}

TileNode* TileList::searchNode(int x, int y) {
    TileNode* curr = _front; // start the search from the top of the list
    // iterate across the list
    while(curr != nullptr) {
        // return curr if current node covers the user-clicked location
        if(x >= curr->x && x <= curr->x+curr->width && y >= curr->y && y <= curr->y+curr->height) {
            return curr;
        }
        curr = curr->next; // otherwise move to the next node
    }
    return curr;
}
