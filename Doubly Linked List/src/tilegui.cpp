/*
 * CS 106B/X Tiles
 * This GUI uses your TileList class and contains the 'main'
 * function to set up the overall program's graphical user interface.
 * Some commands print debug messages that appear on the bottom console.
 *
 * Please DO NOT MODIFY this provided file. Your turned-in files should work
 * with an unmodified version of all provided code files.
 *
 * @version 2019/05/16
 * - 19sp 106B initial version
 * @version 2017/10/30
 * - 17au 106X initial version
 * @version 2016/11/03
 * - 16au 106X initial version
 */

#include "tilegui.h"
#include <iomanip>
#include "gthread.h"
#include "random.h"
#include "sound.h"
#include "stack.h"
#include "strlib.h"

TileGui::TileGui() {
    // possibly use the same random numbers every time for testing
    if (!TileGui::RANDOM) {
        setRandomSeed(106);
    }

    // set up window
    _window = new GWindow(WINDOW_WIDTH, WINDOW_HEIGHT, /* visible */ false);
    _window->center();
    _window->getCanvas()->setBackground(GWindow::getDefaultInteractorBackgroundColorInt());   // looks better on dark mode
    _window->setWindowTitle("CS 106B Tiles");
    _window->setRepaintImmediately(false);
    _window->setExitOnClose(true);
    _window->setMouseListener([this](GEvent event) {
        processMouseEvent(event);
    });
    _window->setWindowListener([this](GEvent event) {
        if (_window->isVisible() && event.getEventType() == WINDOW_RESIZED) {
            clearAndRedraw();
        }
    });

    // set up buttons and radio buttons
    _gbAddFront = new GButton("Add Front");
    _gbAddFront->setActionCommand("addFront");
    _gbAddFront->setAccelerator("ctrl F");
    _gbAddFront->setActionListener([this]() {
        addRandomTiles(/* toFront */ true);
    });

    _gbAddBack = new GButton("Add Back");
    _gbAddBack->setActionCommand("addBack");
    _gbAddBack->setAccelerator("ctrl B");
    _gbAddBack->setActionListener([this]() {
        addRandomTiles(/* toFront */ false);
    });

    _gbClear = new GButton("Clear");
    _gbClear->setActionCommand("clear");
    _gbClear->setActionListener([this]() {
        std::cout << "clear" << std::endl;
        _tileList->clear();
        clearAndRedraw();
    });

    _gbDebug = new GButton("Debug");
    _gbDebug->setActionCommand("debug");
    _gbDebug->setActionListener([this]() {
        std::cout << "debug" << std::endl;
        _tileList->debug();
        clearAndRedraw();
    });

    _grbHighlight = new GRadioButton("Highlight");
    _grbHighlight->setActionCommand("highlight");
    _grbHighlight->setAccelerator("ctrl H");
    _grbHighlight->setSelected(true);

    _grbRaise = new GRadioButton("Raise");
    _grbRaise->setActionCommand("raise");
    _grbRaise->setAccelerator("ctrl R");

    _grbLower = new GRadioButton("Lower");
    _grbLower->setActionCommand("lower");
    _grbLower->setAccelerator("ctrl L");

    _grbRemove = new GRadioButton("Remove");
    _grbRemove->setActionCommand("remove");
    _grbRemove->setAccelerator("ctrl M");

    _grbRemoveAll = new GRadioButton("Remove All");
    _grbRemoveAll->setActionCommand("removeAll");
    _grbRemoveAll->setAccelerator("ctrl V");

    _grbMerge = new GRadioButton("Merge");
    _grbMerge->setActionCommand("merge");
    _grbMerge->setAccelerator("ctrl G");

    _gtfAddNumber = new GTextField(/* value */ TILE_COUNT, /* min */ 1, /* max */ 999);
    _gtfAddNumber->requestFocus();

    _glMousePos = new GLabel("(x=___, y=___)");
    _glMousePos->setFont("Monospaced");

    // perform layout
    _window->addToRegion(_grbHighlight, "North");
    _window->addToRegion(_grbRaise, "North");
    _window->addToRegion(_grbLower, "North");
    _window->addToRegion(_grbRemove, "North");
    _window->addToRegion(_grbRemoveAll, "North");
    _window->addToRegion(_grbMerge, "North");

    _window->addToRegion(_gbAddFront, "South");
    _window->addToRegion(_gbAddBack, "South");
    _window->addToRegion(_gtfAddNumber, "South");
    _window->addToRegion(_gbClear, "South");
    _window->addToRegion(_glMousePos, "South");
    _window->addToRegion(_gbDebug, "South");

    _tileList = new TileList();
    _dragTile = nullptr;
    _dragX = 0;
    _dragY = 0;

    _window->setVisible(true);
    _window->setCanvasSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _window->center();
}

// commenting out delete calls due to some kind of GUI memory bug; not a big deal
TileGui::~TileGui() {
//    delete _gbAddFront;
//    delete _gbAddBack;
//    delete _gbClear;
//    delete _gbDebug;
//    delete _grbHighlight;
//    delete _grbRaise;
//    delete _grbLower;
//    delete _grbRemove;
//    delete _grbRemoveAll;
//    delete _glMousePos;
//    delete _window;
    _gbAddBack = nullptr;
    _gbAddFront = nullptr;
    _gbClear = nullptr;
    _gbDebug = nullptr;
    _grbHighlight = nullptr;
    _grbRaise = nullptr;
    _grbLower = nullptr;
    _grbRemove = nullptr;
    _grbRemoveAll = nullptr;
    _grbMerge = nullptr;
    _glMousePos = nullptr;
    _window = nullptr;
}

void TileGui::addRandomTiles(bool toFront) {
    if (!_gtfAddNumber->valueIsInteger()) {
        return;
    }

    int count = _gtfAddNumber->getValueAsInteger();
    for (int i = 0; i < count; i++) {
        // pick random coords and size
        int width = randomInteger(MIN_SIZE, MAX_SIZE);
        int height = randomInteger(MIN_SIZE, MAX_SIZE);
        int x = randomInteger(0, WINDOW_WIDTH - width);
        int y = randomInteger(0, WINDOW_HEIGHT - height);

        // pick random color
        std::string color = randomColorString(MIN_COLOR, MAX_COLOR);

        std::cout << (toFront ? "addFront" : "addBack")
                  << " (x=" << x << ", y=" << y << ", w=" << width
                  << ", h=" << height << ", color=\"" << color << "\")" << std::endl;
        if (toFront) {
            // add a tile to the front of the list
            _tileList->addFront(x, y, width, height, color);
        } else {
            // add a tile to the back of the list
            _tileList->addBack(x, y, width, height, color);
        }
    }

    clearAndRedraw();
}

void TileGui::clearAndRedraw() {
    std::cout << "drawAll" << std::endl;
    GThread::runOnQtGuiThread([this]() {
        // draw the tiles (in reverse order)
        _window->clearCanvas();
        _tileList->drawAll(*_window);
        _window->repaint();
    });
}

void TileGui::processMouseEvent(GEvent& mouseEvent) {
    int x = static_cast<int>(mouseEvent.getX());
    int y = static_cast<int>(mouseEvent.getY());
    int type = mouseEvent.getEventType();
    if (type == MOUSE_PRESSED) {
        // call the student's highlight, raise, lower, remove, or removeAll function
        if (_grbHighlight->isSelected()) {
            std::cout << "highlight (x=" << x << ", y=" << y << ")" << std::endl;
            bool result = _tileList->highlight(x, y);
            std::cout << "(returned " << std::boolalpha << result << ")" << std::endl;
        } else if (_grbRaise->isSelected()) {
            std::cout << "raise (x=" << x << ", y=" << y << ")" << std::endl;
            if (mouseEvent.isShiftKeyDown()) {
                raiseTheRoof();   // easter egg
            }
            bool result = _tileList->raise(x, y);
            if (result) {
                // initiate dragging of the front tile
                _dragTile = _tileList->getFront();
                if (_dragTile) {
                    _dragX = x;
                    _dragY = y;
                }
            }
            std::cout << "(returned " << std::boolalpha << result << ")" << std::endl;
        } else if (_grbLower->isSelected()) {
            std::cout << "lower (x=" << x << ", y=" << y << ")" << std::endl;
            bool result = _tileList->lower(x, y);
            std::cout << "(returned " << std::boolalpha << result << ")" << std::endl;
        } else if (_grbRemove->isSelected()) {
            std::cout << "remove (x=" << x << ", y=" << y << ")" << std::endl;
            bool result = _tileList->remove(x, y);
            std::cout << "(returned " << std::boolalpha << result << ")" << std::endl;
        } else if (_grbRemoveAll->isSelected()) {
            std::cout << "removeAll (x=" << x << ", y=" << y << ")" << std::endl;
            int result = _tileList->removeAll(x, y);
            std::cout << "(returned " << result << ")" << std::endl;
        } else if (_grbMerge->isSelected()) {
            std::cout << "merge (x=" << x << ", y=" << y << ")" << std::endl;
            _tileList->merge(x, y);
        }
        clearAndRedraw();
    } else if (type == MOUSE_MOVED) {
        std::ostringstream out;
        out << "(x=" << std::setw(3) << std::setfill('0') << x << ", y="
            << std::setw(3) << std::setfill('0') << y << ")";
        _glMousePos->setText(out.str());
    } else if (type == MOUSE_DRAGGED) {
        if (_dragTile) {
            // move tile based on difference in mouse position
            _dragTile->x += x - _dragX;
            _dragTile->y += y - _dragY;
            _dragX = x;
            _dragY = y;
            clearAndRedraw();
        }
    } else if (type == MOUSE_RELEASED) {
        _dragTile = nullptr;
        _dragX = 0;
        _dragY = 0;
    }
}

// This is a very silly "easter egg" function that plays music
// and recolors all the tiles on a timer.
void TileGui::raiseTheRoof() {
    static bool isRaising = false;   // stop multiple threads
    if (isRaising) return;

    GThread::runInNewThreadAsync([this]() {
        isRaising = true;
        Sound::playSound("dance-beat-short.mp3");

        int timeSleepIncr = 290;       // roughly timed to match beat of song
        int soundClipLength = 22000;   // rough length of song in ms
        for (int timeSlept = 0; timeSlept < soundClipLength; timeSlept += timeSleepIncr) {
            // recolor all tiles
            TileNode* front = _tileList->getFront();
            while (front) {
                // pick random color
                front->color = randomColorString(MIN_COLOR, MAX_COLOR);
                front = front->next;
            }
            clearAndRedraw();
            _window->sleep(timeSleepIncr);
        }
        isRaising = false;
    });
}


// main function for overall program
int main() {
    // set up GUI and widgets
    new TileGui();
    return 0;
}
