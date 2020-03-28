/*
 * CS 106B/X Tiles
 * This file contains the declaration of the TileNode structure.
 *
 * Please DO NOT MODIFY this provided file. Your turned-in files should work
 * with an unmodified version of all provided code files.
 *
 * @version 2019/05/16
 * - 19sp 106B initial version
 * @version 2018/11/01
 * - 18au 106B initial version
 * @version 2016/11/03
 * - 16au 106X version
 */

#include "tilenode.h"
#include "gcolor.h"
#include <sstream>

// declare static variables
int TileNode::nextNodeNumber = 1;
int TileNode::instanceCount = 0;

TileNode::TileNode(int x, int y, int width, int height,
                   std::string color, TileNode* prev, TileNode* next)
        : x(x),
          y(y),
          width(width),
          height(height),
          color(color),
          prev(prev),
          next(next),
          number(nextNodeNumber++) {
    instanceCount++;
}

TileNode::~TileNode() {
    if (instanceCount > 0) {
        instanceCount--;
    }
}

// implementation note:
// 'contains' is <= bottom/right side because of 1px border width
bool TileNode::contains(int x, int y) const {
    return this->x <= x && x <= this->x + this->width
        && this->y <= y && y <= this->y + this->height;
}

void TileNode::draw(GWindow& window) const {
    window.setColor(color);
    window.fillRect(x, y, width, height);
    window.setColor(GColor::BLACK);
    window.drawString("#" + integerToString(number), x + 3, y + 16);
    window.setColor(GColor::WHITE);
    window.drawString("#" + integerToString(number), x + 2, y + 15);
    window.setColor(GColor::BLACK);
    window.drawRect(x, y, width, height);   // rectangle outline
}

std::string TileNode::toString() const {
    std::ostringstream out;
    out << *this;   // calls operator <<
    return out.str();
}

std::ostream& operator <<(std::ostream& out, const TileNode& node) {
    return out << "Tile #" << node.number << " @" << &node
            << "{x=" << node.x << ",y=" << node.y
            << ",w=" << node.width << ",h=" << node.height
            << ",color=\"" << node.color << "\""
            << ",next=" << node.next << "}";
}

int TileNode::getInstanceCount() {
    return instanceCount;
}

void TileNode::resetInstanceCount() {
    instanceCount = 0;
}
