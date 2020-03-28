/*
 * File: tilelist.h
 * -----------------------
 * Assignment 6 Part C Tiles
 *
 * This function declares a class named TileList. It implements the collection of tile nodes using a
 * doubly linked list.
 *
 * @author Liu Jiang (Caci)
 * @version 11/15/2019
*/

#ifndef _tilelist_h
#define _tilelist_h

#include <iostream>
#include <string>
#include "gwindow.h"
#include "tilenode.h"
using namespace std;

class TileList {
public:
    /* Constructor. It will create an doubly linked list. Big O: O(1). */
    TileList();

    /* Destructor. It will free up any memory used by TileList. Big O: O(N) */
    ~TileList();

    /*
     * This function will add new tile node at the back of the list. Big O: O(1).
     * @param x is top-left x coordinate of new node.
     * @param y is top-left y coordinate of new node.
     * @param width of new node.
     * @param height of new node.
     * @string color of new node.
    */
    void addBack(int x, int y, int width, int height, string color);

    /*
     * This function will add new tile node at the front of the list. Big O: O(1).
     * @param x is top-left x coordinate of new node.
     * @param y is top-left y coordinate of new node.
     * @param width of new node.
     * @param height of new node.
     * @string color of new node.
    */
    void addFront(int x, int y, int width, int height, string color);

    /* This function will remove all tile nodes from the list. Big O: O(N).  */
    void clear();

    /*
     * Optional function: it will print the x coodinate of each node forward and backward across the
     * list, and test getFront and getBack functions. Big O: O(N).
    */
    void debug();

    /*
     * This function is to draw all of the tiles in the list on the given window. The tiles will be drawn
     * in bottom-to-top (back-to-front) order so that the tiles at the top of the z-ordering will appear
     * on top of ones that are lower in the z-ordering. Big O: O(N).
     * @param: window is the given graphical window.
    */
    void drawAll(GWindow& window) const;

    /*
     * This function returns the pointer to the tile node representing back of the list (bottom of the
     * z-ordering). Big O: O(1).
     * @return: back pointer.
    */
    TileNode* getBack() const;

    /*
     * This function returns the pointer to the tile node representing the front of your list (the
     * top of the z-ordering). Big O: O(1).
     * @return: front pointer.
    */
    TileNode* getFront() const;

    /*
     * This function will highlight a tile in a yellow color when it is clicked. Big O: O(N)
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * @return true if the topmost target tile node is found and highlighted, and false if no target node
     * is found.
    */
    bool highlight(int x, int y);

    /*
     * This function is to handle lowering a clicked tile to the bottom of the z-ordering (the back of
     * the list). Big O: O(N).
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * @return true if the topmost target tile node has been handled, and false if no target node found.
    */
    bool lower(int x, int y);

    /*
     * This function is to combine all tiles from the list that touch the given x/y coordinates into a
     * single tile whose size is exactly large enough to cover the entire area of all of the original
     * rectangles. Big O: O(N^2).
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
    */
    void merge(int x, int y);
    /*
     * This function is to handle raising a clicked tile to the top of the z-ordering (the front of the
     * list). Big O: O(N).
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * @return true if the topmost target tile node has been handled, and false if no target node found.
    */
    bool raise(int x, int y);

    /*
     * This function is to handle removing a clicked tile from the list. Big O: O().
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * @return true if the topmost target tile node has been removed, and false if no target node found.
    */
    bool remove(int x, int y);

    /*
     * This function is to handle removing all tiles from the list that touch the given x/y coordinates.
     * Big O: O(N^2).
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * return counter is the number of delected nodes.
    */
    int removeAll(int x, int y);

private:
    TileNode* _front; // pointer to the top node in the list
    TileNode* _back; // pointer to the bottom node in the list

    /*
     * This helper function is to detach a target node from the list.
     * @param curr is the pointer to the node to delete.
    */
    void detachNode(TileNode*& curr);

    /*
     * This helper function is to search a topmost target node covering x/y coordinate clicked by user.
     * @param x is the x coordinate of user-clicked location.
     * @param y is the y coordinate of user-clicked location.
     * @return a pointer to the searched node, and nullptr if no target node found.
    */
    TileNode* searchNode(int x, int y);
};

#endif // _tilelist_h
