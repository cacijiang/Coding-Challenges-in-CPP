/*
 * File: fractals-eextra.cpp
 * -----------------------
 * Assignment 3 Part D: Coding Fractals and Graphics
 *
 * Extra feature 1 is Sierpinski colors. Sierpinski triangles draw different levels in different colors.
 * Extra feature 2 is flood fill tolerance threshold. a modified version of floodFill that also accepts
 * a tolerance amount as an integer and keeps going until the pixel color distance exceeds that threshold.
 *
 * This program mainly writes down drawSierpinskiTriangle function and floodFill function.
 * drawSierpinskiTriangle function will draw Sierpinski triangles recursively at user-required position.
 * Each time, the triangle will be drawn inside a previous one and shrink its size, untill it reaches the
 * level input by user.
 * floodFill function will change the color at a position clicked by user. It will automatically fill the
 * surronding area covered the previous color by recursion.
 *
 * @author Liu Jiang (Caci)
 * @version 10/19/2019
*/
/* Function prototype declarations */
#include "fractals.h"
#include <iostream>
#include <cmath>
#include "gwindow.h"
using namespace std;

const string COLORS[] = {"MAGENTA", "BLUE", "CYAN", "GREEN", "YELLOW", "ORANGE", "RED"}; // rainbow colors
const int N_COLORS = sizeof(COLORS) / sizeof(string);

/* Function prototype declarations */
void triangleHelper(GWindow& window, double x, double y, double size, int order, int n);
void floodHelperExtra(GWindow& window, int x, int y, int preColor, int color, int threshold, int& counter);

/*
 * This is the major function to draw Sierpinski triangles which is directly called by main program.
 * This function will valid user's input at first, and then call a helper function of triangleHelper
 * to draw triangles recursively.
 * @param window is the graphics window to use as a canvas for drawing
 * @param x, y are (x,y) coords input by user
 * @param size is the edge length of triangle
 * @param order is the desired levels of depth for the fractal drawing
*/
void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order) {
    // throw an error if the x, y, order, or size passed is negative
    if(x < 0 || y < 0 || size < 0 || order < 0) {
        throw "Negative value(s) for x, y, or size";
    } else if (size == 0.0 || order == 0.0) {
        return;
    } else {
        window.clear();
        triangleHelper(window, x, y, size, order, 1); // call helper function to draw recursively
    }
}

/*
 * This is the major function for flood fill which is directly called by main program.
 * This function will valid user's input at first, and then call a helper function of floodHelper
 * to change the color of surronding images recursively.
 * @param window is the graphics window to use as a canvas for drawing
 * @param x, y are (x,y) coords input by user
 * @param color is the desired color to fill images
 * @param threshold is the absolute values of the differences between the red, green, and blue
 *        components of the two colors
*/
int floodFill(GWindow& window, int x, int y, int color, int threshold) {
    // throw an error if the x, y is negative or outside the boundary of canvas
    if(x < 0 || y < 0 || !window.inCanvasBounds(x, y)) {
        throw "Invalid region to set color.";
    } else if (color == window.getPixel(x, y)){
        // directly return if the desired color is the same as current image color
        return 0;
    }
    int counter = 0; // records how many pixels have been changed
    int preColor = window.getPixel(x, y); // original color of the image
    // call recursive function to fill color over the region
    floodHelperExtra(window, x, y, preColor, color, threshold, counter);
    return counter;
}

/*
 * This is a helper recursive function to draw triangles in different color for different level.
 * Base case: order = 1. The initial big triangle at user-input (x, y) with user-input size.
 * Recursive case: draw three triangles. Change the positions and divide the size by 2. Continue untill
 * it reaches the required order.
 * @param window is the graphics window to use as a canvas for drawing
 * @param x, y are (x,y) coords input by user
 * @param size is the edge length of triangle
 * @param order is the desired levels of depth for the fractal drawing
 * @param n is the current level of fractal drawing
*/
void triangleHelper(GWindow& window, double x, double y, double size, int order, int n) {
    if(n <= order) {
        int colorsIndex = n % N_COLORS; // % to ensure valid index
        window.setColor(COLORS[colorsIndex]);
        // base case is the first order
        if(n == 1) {
            window.drawPolarLine(x, y, size, 0);
            window.drawPolarLine(x+size, y, size, 240);
            window.drawPolarLine(x, y, size, -60);
            double newX = x+size/2; // for next time start from the midpoint of the horizontal edge
            double newSize = size/2;
            // call itself again even it's a base case.
            // the function will exit next time without implementing recursive case if input order is 1
            triangleHelper(window, newX, y, newSize, order, n+1);
        } else if(n > 1 && n <= order) {
            // recursive case
            // each triangle will introduce three smaller triangles at its left, right and bottom
            window.drawPolarLine(x, y, size, 240);
            double xLeft = x-size/2;
            window.drawPolarLine(x, y, size, -60);
            double xRight = x+size/2;
            double yDown = y+size*sqrt(3)/2;
            window.drawPolarLine(xLeft, yDown, size, 0);
            double newSize = size/2;
            // draw the next three triangles
            triangleHelper(window, xLeft, y, newSize, order, n+1);
            triangleHelper(window, xRight, y, newSize, order, n+1);
            triangleHelper(window, x, yDown, newSize, order, n+1);
        }
    }
}

/*
 * This is a helper recursive function to fill color over a region.
 * Base case 1: (x, y) goes outside of canvas
 * Base case 2: current pixel has been filled by desired color or is the same as desired color.
 * Recursive case: the current pixel is the original color or within the threshold. Then change the color
 * of current pixel. Update counter. Explore and continue filling in four directions: up, right, down, left.
 * @param window is the graphics window to use as a canvas for drawing
 * @param x, y are (x,y) coords input by user
 * @param preColor is the original color of the image
 * @param color is the desired color to fill images
 * @counter records how many pixels have been changed color
*/
void floodHelperExtra(GWindow& window, int x, int y, int preColor, int color, int threshold, int& counter) {
    // base case 1
    if(!window.inCanvasBounds(x, y)) {
        return;
    }
    // base case 2
    if(window.getPixel(x, y) == color) {
        return;
    }
    // recursive case
    if(window.getPixel(x, y) == preColor || abs(color-window.getPixel(x, y)) <= threshold) {
        window.setPixel(x, y, color);
        counter++;
        floodHelperExtra(window, x, y-1, preColor, color, threshold, counter);
        floodHelperExtra(window, x+1, y, preColor, color, threshold, counter);
        floodHelperExtra(window, x, y+1, preColor, color, threshold, counter);
        floodHelperExtra(window, x-1, y, preColor, color, threshold, counter);
    }
}


