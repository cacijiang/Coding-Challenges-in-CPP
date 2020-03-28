/*
 * CS 106B/X Fractal Recursion Problems
 * This instructor-provided file defines the members of a class representing the
 * overall graphical user interface (GUI) for this part of the program.
 * See fractalgui.h for the class and member declarations.
 *
 * You don't need to modify this file.
 * Your recursive functions should work with an unmodified version of this file.
 *
 * @author Marty Stepp
 * @version 2019/04/27
 * - add rectangles txt file to fix randomness on all platforms
 * - fixed canvas size issues
 * - added auto testing of Sierpinski and Flood Fill using Ctrl+1...9
 * - speed up Mac drawing
 * - clean up functional/ifdef code
 * - comments
 * @version 2019/04/23
 * - 19sp update: add thread for drawing operations to improve animation on Mac
 * @version 2018/10/11
 * - 18au 106B initial version;
 *   added H-tree (but disabled);
 *   new Qt-based GUI;
 *   crosshairs on drag rectangle;
 *   2px-thick Sierpinski lines; etc.
 * @version 2018/01/25
 * - removed Mandelbrot set for CS 106B version
 * @version 2017/10/14
 * - modified Complex parse logic to use default on failure
 *   (addresses unusual parsing failures on some systems)
 * @version 2017/10/12
 * - initial 17au version;
 *   modified with additional fractals (Mandelbrot Set) based on cgregg 17wi/sp version
 * - added spinner to show while graphics are drawing
 * @version 2016/10/16
 * - modified for 16au with additional fractals (tree)
 * @version 2015/04/24
 * - modified for 15sp version, including exception-handling
 * @version 2014/10/13
 * - modified for 14au version, including new version of floodFill
 */

#include "fractalgui.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "filelib.h"
#include "gcolor.h"
#include "grid.h"
#include "gspacer.h"
#include "gthread.h"
#include "queue.h"
#include "random.h"
#include "strlib.h"
#include "timer.h"
#include "fractals.h"

/*static*/ const int FractalGUI::WINDOW_WIDTH             = 700;
/*static*/ const int FractalGUI::WINDOW_HEIGHT            = 650;
/*static*/ const int FractalGUI::FLOOD_FILL_NUM_SHAPES    = 100;
/*static*/ const int FractalGUI::FLOOD_FILL_MIN_RECT_SIZE =  20;
/*static*/ const int FractalGUI::FLOOD_FILL_MAX_RECT_SIZE = 100;
/*static*/ const int FractalGUI::FLOOD_FILL_RANDOM_SEED   = 106;
/*static*/ const bool FractalGUI::FLOOD_FILL_READ_FILE    = true;
/*static*/ const int FractalGUI::DRAG_RECT_COLOR          = 0xaa8822;
/*static*/ const int FractalGUI::MIN_ORDER                = -10;
/*static*/ const int FractalGUI::MAX_ORDER                = 50;
/*static*/ const std::string FractalGUI::FLOOD_FILL_RECTS_FILE_NAME = "rectangles.txt";

/*static*/ FractalGUI* FractalGUI::_instance = nullptr;

/*static*/ FractalGUI* FractalGUI::instance() {
    if (!_instance) {
        _instance = new FractalGUI();
        _instance->initialize();
    }
    return _instance;
}

FractalGUI::FractalGUI()
        : _window(nullptr),
          _sierpinskiButton(nullptr),
          _htreeButton(nullptr),
          _treeButton(nullptr),
          _mandelbrotButton(nullptr),
          _floodFillButton(nullptr),
          _colorChooser(nullptr),
          _floodFillRectsButton(nullptr),
          _floodFillRandomBox(nullptr),
          _diffButton(nullptr),
          _clearButton(nullptr),
          _xField(nullptr),
          _yField(nullptr),
          _sizeField(nullptr),
          _orderField(nullptr),
          _animationBox(nullptr),
          _loadingLabel(nullptr),
          _statusBar(nullptr),
          _dragRect(nullptr),
          _dragLineH(nullptr),
          _dragLineV(nullptr),
          _fillMode(false),
          _fillInProgress(false),
          _lastComplexRange("") {
    // empty
}

FractalGUI::~FractalGUI() {
    // empty
}

void FractalGUI::clear() {
    _window->clearCanvas();
    if (_dragRect) {
        delete _dragRect;
        _dragRect = nullptr;
    }
    if (_dragLineH) {
        delete _dragLineH;
        _dragLineH = nullptr;
    }
    if (_dragLineV) {
        delete _dragLineV;
        _dragLineV = nullptr;
    }
    _window->repaint();
}

void FractalGUI::doEnabling(bool enabled) {
    Vector<GInteractor*> interactors {
        _sierpinskiButton,
        _htreeButton,
        _treeButton,
        _mandelbrotButton,
        _floodFillButton,
        _colorChooser,
        _floodFillRectsButton,
        _floodFillRandomBox,
        _diffButton,
        _clearButton,
        _xField,
        _yField,
        _sizeField,
        _orderField
    };
    for (GInteractor* interactor : interactors) {
        if (interactor) {
            interactor->setEnabled(enabled);
        }
    }
}

void FractalGUI::doFloodFill(double x, double y) {
    doEnabling(false);
    std::string colorStr = _colorChooser->getSelectedItem();
    int color = getColorMap()[colorStr];

    std::ostringstream out;
    out << "Flood fill: x=" << std::dec << x << " y=" << y
        << " color " << std::hex << std::setw(6) << std::setfill('0') << color
        << std::dec << " ...";
    loadingBegin();
    std::string statusText = out.str();
    setStatusText(statusText);
    _window->setColor(0x0);
    _window->setLineWidth(1);
    _fillInProgress = true;

    auto func = [this, x, y, color]() {
        Timer timmy;
        timmy.start();

        int pixelsColored = floodFill(*_window, x, y, color);
        if (!isAnimated()) {
            _window->repaint();
        }

        _fillInProgress = false;
        long elapsed = timmy.stop();

        std::string newStatusText = integerToString(pixelsColored) + " pixels colored "
                + " (" + longToString(elapsed) + "ms)";
        setStatusText(newStatusText);
        loadingEnd();
        doEnabling(true);
    };
    runGraphicalFunction(func);
}

void FractalGUI::doHTree() {
#ifdef H_TREE_ENABLED
    if (!hasAllCoords()) {
        setStatusText("Error: You must set an x, y, size, and order first.", /* isError */ true);
        return;
    }
    _fillMode = false;
    doEnabling(false);
    int x = _xField->getValueAsInteger();
    int y = _yField->getValueAsInteger();
    int size = _sizeField->getValueAsInteger();
    int centerX = x + size / 2;
    int centerY = y + size / 2;
    int order = _orderField->getValueAsInteger();

    std::string statusText = "H-tree: centerX=" + integerToString(centerX)
            + " centerY=" + integerToString(centerY)
            + " size=" + integerToString(size)
            + " order=" + integerToString(order);
    setStatusText(statusText + " ...");
    loadingBegin();
    _window->setColor(0x0);   // black; drawTree should change it


    auto func = [this, centerX, centerY, size, order, statusText]() {
        Timer timmy;
        timmy.start();

        drawHTree(*_window, centerX, centerY, size, order);
        if (!isAnimated()) {
            _window->repaint();
        }

        long elapsed = timmy.stop();
        std::string newStatusText = statusText + " (" + longToString(elapsed) + "ms)";
        setStatusText(newStatusText);
        loadingEnd();
        doEnabling(true);
    };
    runGraphicalFunction(func);
#else // H_TREE_ENABLED
    // empty
#endif // H_TREE_ENABLED
}

void FractalGUI::doMandelbrot() {
#ifdef MANDELBROT_SET_ENABLED
    if (!hasAllCoords()) {
        setStatusText("Error: You must set an x, y, size, and order first.", /* isError */ true);
        return;
    }
    _fillMode = false;
    doEnabling(false);
    int x = _xField->getValueAsInteger();
    int y = _yField->getValueAsInteger();
    int size = _sizeField->getValueAsInteger();          // TODO: size => incX/Y?
    int iterations = _orderField->getValueAsInteger();   // "order" == iterations here
    std::string colorStr = _colorChooser->getSelectedItem();
    int color = getColorMap()[colorStr];

    // try to read and parse complex number range
    Complex min(-2, -1);
    Complex max(1, 1);
    if (!readComplexRange(min, max)) {
        return;
    }

    std::string statusText = "Mandelbrot Set: x=" + integerToString(x)
            + " y=" + integerToString(y)
            + " size=" + integerToString(size)
            + " iterations=" + integerToString(iterations)
            + " color=" + colorStr;
    setStatusText(statusText + " ...");
    loadingBegin();
    _window->setColor(0x0);   // black

    auto func = [this, x, y, size, min, max, iterations, color, statusText]() {
        Timer timmy;
        timmy.start();

        drawMandelbrotSet(*_window, x, y, size,
                          min, max,
                          iterations, color);
        if (!isAnimated()) {
            _window->repaint();
        }

        long elapsed = timmy.stop();
        std::string newStatusText = statusText + " (" + longToString(elapsed) + "ms)";
        setStatusText(newStatusText);
        loadingEnd();
        doEnabling(true);
    };
    runGraphicalFunction(func);
#else // MANDELBROT_SET_ENABLED
    // empty
#endif // MANDELBROT_SET_ENABLED
}

void FractalGUI::doSierpinski() {
#ifdef SIERPINSKI_ENABLED
    if (!hasAllCoords()) {
        setStatusText("Error: You must set an x, y, size, and order first.", /* isError */ true);
        return;
    }
    _fillMode = false;
    doEnabling(false);
    int x = _xField->getValueAsInteger();
    int y = _yField->getValueAsInteger();
    int size = _sizeField->getValueAsInteger();
    int order = _orderField->getValueAsInteger();
    std::string statusText = "Sierpinski triangle: x=" + integerToString(x)
            + " y=" + integerToString(y)
            + " size=" + integerToString(size)
            + " order=" + integerToString(order);
    setStatusText(statusText + " ...");
    loadingBegin();
    _window->setColor(0x0);   // black
    _window->setLineWidth(2);

    auto func = [this, x, y, size, order, statusText]() {
        Timer timmy;
        timmy.start();

        drawSierpinskiTriangle(*_window, x, y, size, order);
        if (!isAnimated()) {
            _window->repaint();
        }

        long elapsed = timmy.stop();
        std::string newStatusText = statusText + " (" + longToString(elapsed) + "ms)";
        setStatusText(newStatusText);
        loadingEnd();
        doEnabling(true);
    };
    runGraphicalFunction(func);
#else // SIERPINSKI_ENABLED
    // empty
#endif // SIERPINSKI_ENABLED
}

void FractalGUI::doTree() {
#ifdef TREE_ENABLED
    if (!hasAllCoords()) {
        setStatusText("Error: You must set an x, y, size, and order first.", /* isError */ true);
        return;
    }
    _fillMode = false;
    doEnabling(false);
    int x = _xField->getValueAsInteger();
    int y = _yField->getValueAsInteger();
    int size = _sizeField->getValueAsInteger();
    int order = _orderField->getValueAsInteger();

    std::string statusText = "Recursive tree: x=" + integerToString(x)
            + " y=" + integerToString(y)
            + " size=" + integerToString(size)
            + " order=" + integerToString(order);
    setStatusText(statusText + " ...");
    loadingBegin();
    _window->setColor(0x0);   // black; drawTree should change it

    auto func = [this, x, y, size, order, statusText]() {
        Timer timmy;
        timmy.start();

        drawTree(*_window, x, y, size, order);
        if (!isAnimated()) {
            _window->repaint();
        }

        long elapsed = timmy.stop();
        std::string newStatusText = statusText + " (" + longToString(elapsed) + "ms)";
        setStatusText(newStatusText);
        loadingEnd();
        doEnabling(true);
    };
    runGraphicalFunction(func);
#else // TREE_ENABLED
    // empty
#endif // TREE_ENABLED
}

void FractalGUI::floodFill_addShapes() {
    // read rectangle coordinates if needed
    Queue<int> rectCoords;
    if (!_floodFillRandomBox->isChecked()) {
        if (FLOOD_FILL_READ_FILE && fileExists(FLOOD_FILL_RECTS_FILE_NAME)) {
            // not random, from file; read integer coords/colors from the file
            std::ifstream input;
            std::string token;
            input.open(FLOOD_FILL_RECTS_FILE_NAME.c_str());
            while (input >> token) {
                if (startsWith(token, "#")) {
                    // a hex color such as #ff00aa
                    rectCoords.enqueue(stringToInteger(token.substr(1), /* radix */ 16));
                } else {
                    // a normal decimal integer such as an x/y/w/h value
                    rectCoords.enqueue(stringToInteger(token));
                }
            }
        } else {
            // not random, but not from file; use seed for consistent rectangles
            setRandomSeed(FLOOD_FILL_RANDOM_SEED);
        }
    }

    // use buffered image to store individual pixels
    int canvasWidth  = static_cast<int>(_window->getCanvasWidth());
    int canvasHeight = static_cast<int>(_window->getCanvasHeight());
    Vector<std::string> colorVector = getColorMap().keys();
    for (int i = 0; i < FLOOD_FILL_NUM_SHAPES; i++) {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        int color = 0;
        if (rectCoords.isEmpty()) {
            w  = randomInteger(FLOOD_FILL_MIN_RECT_SIZE, FLOOD_FILL_MAX_RECT_SIZE);
            h  = randomInteger(FLOOD_FILL_MIN_RECT_SIZE, FLOOD_FILL_MAX_RECT_SIZE);
            x  = randomInteger(-w, canvasWidth);
            y  = randomInteger(-h, canvasHeight);
            color = getColorMap()[randomElement(colorVector)];
        } else {
            x  = rectCoords.dequeue();
            y  = rectCoords.dequeue();
            w  = rectCoords.dequeue();
            h  = rectCoords.dequeue();
            color = rectCoords.dequeue();
        }
        _window->setColor(color);
        _window->setFillColor(color);
        _window->setLineWidth(1);
        _window->fillRect(x, y, w, h);

        // print coords to save in file
        std::cout << std::dec
                  << std::setw(3) << std::setfill(' ') << x << " "
                  << std::setw(3) << std::setfill(' ') << y << " "
                  << std::setw(3) << std::setfill(' ') << w << " "
                  << std::setw(3) << std::setfill(' ') << h << " "
                  << "#" << std::hex << std::setw(6) << std::setfill('0') << color << std::endl;
    }
    if (!isAnimated()) {
        _window->repaint();
    }
}

const Map<std::string, int>& FractalGUI::getColorMap() {
    static Map<std::string, int> colorMap;
    if (colorMap.isEmpty()) {
        colorMap["Red"]    = 0x8c1515;   // Stanford red
        colorMap["Yellow"] = 0xeeee00;   // yellow
        colorMap["Blue"]   = 0x0000cc;   // blue
        colorMap["Green"]  = 0x00cc00;   // green
        colorMap["Purple"] = 0xcc00cc;   // purple
        colorMap["Orange"] = 0xff8800;   // orange
    }
    return colorMap;
}

void FractalGUI::handleKeyEvent(const GEvent& keyEvent) {
    if (keyEvent.getEventType() == KEY_PRESSED
            && keyEvent.isCtrlOrCommandKeyDown()) {
        char ch = keyEvent.getKeyCode();
        if (tolower(ch) == 's') {
            // Ctrl+S => Save canvas
            saveCanvas();
        } else if (ch >= '0' && ch <= '9') {
            if ('1' <= ch && ch <= '5') {
                // Sierpinski testing
                int order = ch - '0';
                testSierpinski(order);
            } else {
                // flood fill testing
                int num = ch == '0' ? 5 : ch - '5';
                testFloodFill(num);
            }
        }
    }
}

void FractalGUI::handleMouseEvent(const GEvent& mouseEvent) {
    int mx = (int) mouseEvent.getX();
    int my = (int) mouseEvent.getY();
    if (mx < 0 || my < 0
            || mx >= _window->getCanvasWidth()
            || my >= _window->getCanvasHeight()) {
        return;
    }
    if (mouseEvent.getType() == MOUSE_MOVED) {
        if (!_fillInProgress) {
            int px = _window->getPixel(mx, my);
            std::string statusText = "(x=" + integerToString(mx) + ", y=" + integerToString(my)
                    + "), color=" + GColor::convertRGBToColor(px);
            setStatusText(statusText);
        }
    } else if (mouseEvent.getType() == MOUSE_PRESSED) {
        if (!mouseEvent.isLeftClick()) {
            if (_dragRect) {
                _window->remove(_dragRect);
                _window->remove(_dragLineH);
                _window->remove(_dragLineV);
                delete _dragRect;
                delete _dragLineH;
                delete _dragLineV;
                _dragRect = nullptr;
                _dragLineH = nullptr;
                _dragLineV = nullptr;
                _window->repaint();
                return;
            }
        }

        _xField->setValue(mx);
        _yField->setValue(my);

        if (_fillMode && !_fillInProgress) {
            doFloodFill(mx, my);
        }
    } else if (mouseEvent.getEventType() == MOUSE_DRAGGED) {
        if (!_fillMode) {
            int x = _xField->getValueAsInteger();
            int y = _yField->getValueAsInteger();
            if (mx > x && my > y) {
                int size = std::min(mx - x, my - y);
                _sizeField->setValue(size);
                if (_dragRect) {
                    _dragRect->setBounds(x, y, size, size);
                    _dragLineH->setStartPoint(x, y + size / 2);
                    _dragLineH->setEndPoint(x + size, y + size / 2);
                    _dragLineV->setStartPoint(x + size / 2, y);
                    _dragLineV->setEndPoint(x + size / 2, y + size);
                } else {
                    _dragRect = new GRect(x, y, size, size);
                    _dragRect->setColor(DRAG_RECT_COLOR);
                    _dragLineH = new GLine(x, y + size / 2,
                                          x + size, y + size / 2);
                    _dragLineH->setColor(DRAG_RECT_COLOR);
                    _dragLineH->setLineStyle(GLine::LINE_DASH);
                    _dragLineV = new GLine(x + size / 2, y,
                                          x + size / 2, y + size);
                    _dragLineV->setColor(DRAG_RECT_COLOR);
                    _dragLineV->setLineStyle(GLine::LINE_DASH);
                    _window->add(_dragRect);
                    _window->add(_dragLineH);
                    _window->add(_dragLineV);
                }
            }
        }
    } else if (mouseEvent.getEventType() == MOUSE_RELEASED) {
        if (!_fillMode && _dragRect) {
            _window->remove(_dragRect);
            _window->remove(_dragLineH);
            _window->remove(_dragLineV);
            delete _dragRect;
            delete _dragLineH;
            delete _dragLineV;
            _dragRect = nullptr;
            _dragLineH = nullptr;
            _dragLineV = nullptr;
        }
    }
}

bool FractalGUI::hasAllCoords() const {
    return _xField->valueIsInt() && _yField->valueIsInt()
            && _sizeField->valueIsInt() && _orderField->valueIsInt();
}

bool FractalGUI::isAnimated() const {
    return this->_animationBox->isChecked();
}

bool FractalGUI::isMac() const {
#ifdef __APPLE__
    return true;
#else // __APPLE__
    return false;
#endif // __APPLE__
}

void FractalGUI::loadingBegin() {
#ifdef LOADING_LABEL_ENABLED
    _loadingLabel->setVisible(true);
#else // LOADING_LABEL_ENABLED
    // empty
#endif // LOADING_LABEL_ENABLED
}

void FractalGUI::loadingEnd() {
#ifdef LOADING_LABEL_ENABLED
    _loadingLabel->setVisible(false);
#else // LOADING_LABEL_ENABLED
    // empty
#endif // LOADING_LABEL_ENABLED
}

/*
 * Helper to create one button in the GUI.
 */
static GButton* createButton(const std::string& cmd, const std::string& icon) {
    GButton* button = new GButton();
    if (!cmd.empty()) {
        button->setText(cmd);
    }
    button->setTextPosition(GInteractor::TEXT_UNDER_ICON);
    button->setActionCommand(cmd);
    button->setTooltip(cmd);
    if (!icon.empty()) {
        button->setIcon(icon);
    }
    return button;
}

void FractalGUI::initialize() {
    GObject::setAntiAliasing(false);
    _fillMode = false;
    _fillInProgress = false;
    _dragRect = nullptr;
    _dragLineH = nullptr;
    _dragLineV = nullptr;
    _lastComplexRange = "(-2-1i) .. (1+1i)";

    // create window
    _window = new GWindow(/* visible */ false);
    _window->setResizable(true);
    _window->setCanvasSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    _window->setTitle("CS 106B/X Fractals");
    _window->setExitOnClose(true);

    // hotkeys to make it easier to test specific inputs
    auto keyFunc = [this](GEvent keyEvent) {
        this->handleKeyEvent(keyEvent);
    };
    _window->setKeyListener(keyFunc);

    _window->setMouseListener([this](GEvent mouseEvent) {
        this->handleMouseEvent(mouseEvent);
    });

    // create controls
#ifdef SIERPINSKI_ENABLED
    _sierpinskiButton = createButton("S.Triangle", "triangle-icon.png");
    _sierpinskiButton->setActionListener([this]() {
        this->doSierpinski();
    });
    _sierpinskiButton->setKeyListener(keyFunc);
#endif // SIERPINSKI_ENABLED

#ifdef H_TREE_ENABLED
    _htreeButton = createButton("H Tree", "h-icon.png");
    _htreeButton->setActionListener([this]() {
        this->doHTree();
    });
    _htreeButton->setKeyListener(keyFunc);
#endif // H_TREE_ENABLED

#ifdef TREE_ENABLED
    _treeButton = createButton("Tree", "tree-icon.png");
    _treeButton->setActionListener([this]() {
        this->doTree();
    });
    _treeButton->setKeyListener(keyFunc);
#endif // TREE_ENABLED

#ifdef MANDELBROT_SET_ENABLED
    _mandelbrotButton = createButton("M.Set", "mandelbrot-icon.png");
    _mandelbrotButton->setActionListener([this]() {
        this->doMandelbrot();
    });
    _mandelbrotButton->setKeyListener(keyFunc);
#endif // MANDELBROT_SET_ENABLED

#ifdef FLOOD_FILL_ENABLED
    _floodFillButton = createButton("Flood Fill", "paintbrush-icon.png");
    _floodFillButton->setActionListener([this]() {
        this->_fillMode = true;
    });
    _floodFillButton->setKeyListener(keyFunc);

    _floodFillRandomBox = new GCheckBox("Random?", /* checked */ FLOOD_FILL_RANDOM_SEED < 0);

    _floodFillRectsButton = createButton("Rects", "");
    _floodFillRectsButton->setActionListener([this]() {
        this->setStatusText("Flood fill: adding rectangles");
        this->floodFill_addShapes();
        this->_fillMode = true;
    });
    _floodFillRectsButton->setKeyListener(keyFunc);
#endif // FLOOD_FILL_ENABLED

    _diffButton = createButton("Diff", "diff-icon.png");
    _diffButton->setActionListener([this](GEvent event) {
        if (event.isShiftKeyDown()) {
            saveCanvas();
        } else {
            // pop up DiffImage tool
            std::string filename = GFileChooser::showOpenDialog(
                        /* parent */ this->_window->getWidget(),
                        "Choose image to compare to",
                        "output/");
            if (!filename.empty()) {
                this->_window->compareToImage(filename);
            }
        }
    });
    _diffButton->setKeyListener(keyFunc);

    _clearButton = createButton("Clear", "recycle-icon.png");
    _clearButton->setActionListener([this]() {
        this->clear();
        this->_fillMode = false;
        this->setStatusText("Graphics cleared.");
    });
    _clearButton->setKeyListener(keyFunc);

    _colorChooser = new GChooser();
    _colorChooser->setTooltip("Fill Color");
    _colorChooser->addItems(getColorMap().keys());

    _statusBar = new GLabel(" ");

    _animationBox = new GCheckBox("Animate?", /* checked */ true);
    _animationBox->setActionListener([this]() {
        this->_window->setAutoRepaint(this->_animationBox->isChecked());
    });
    _animationBox->setKeyListener(keyFunc);

    // equalize width of west widgets
    Vector<GInteractor*> westInteractors {
        _sierpinskiButton,
        _htreeButton,
        _treeButton,
        _mandelbrotButton,
        _floodFillButton,
        _floodFillRectsButton,
        _floodFillRandomBox,
        _diffButton,
        _clearButton,
        _colorChooser
    };

    double maxWidth = -1;
    for (GInteractor* interactor : westInteractors) {
        if (!interactor) { continue; }
        maxWidth = std::max(maxWidth, interactor->getWidth());
    }
    for (GInteractor* interactor : westInteractors) {
        if (!interactor) { continue; }
        interactor->setWidth(maxWidth);
    }

    int screenWidth  = std::max(500, static_cast<int>(GWindow::getScreenWidth()));
    int screenHeight = std::max(500, static_cast<int>(GWindow::getScreenHeight()));
    _xField = new GTextField(/* value */ 0, /* min */ MIN_ORDER, /* max */ screenWidth);
    _xField->setMaxLength(4);
    _xField->setPlaceholder("x");
    _xField->setKeyListener(keyFunc);

    _yField = new GTextField(/* value */ 0, /* min */ MIN_ORDER, /* max */ screenHeight);
    _yField->setPlaceholder("y");
    _yField->setKeyListener(keyFunc);

    _sizeField = new GTextField(/* value */ 0, /* min */ MIN_ORDER, /* max */ std::max(screenWidth, screenHeight));
    _sizeField->setPlaceholder("size");
    _sizeField->setKeyListener(keyFunc);

    _orderField = new GTextField(/* value */ 0, /* min */ MIN_ORDER, /* max */ MAX_ORDER);
    _orderField->setMaxLength(2);
    _orderField->setValue(1);
    _orderField->setPlaceholder("order");
    _orderField->setKeyListener(keyFunc);

    _loadingLabel = new GLabel();
    _loadingLabel->setIcon("progress.gif");
    _loadingLabel->setEnabled(false);
    _loadingLabel->setVisible(false);

    // perform layout
    _window->addToRegion(new GLabel("x"), "North");
    _window->addToRegion(_xField, "North");
    _window->addToRegion(new GLabel("y"), "North");
    _window->addToRegion(_yField, "North");
    _window->addToRegion(new GLabel("size"), "North");
    _window->addToRegion(_sizeField, "North");
    _window->addToRegion(new GLabel("order"), "North");
    _window->addToRegion(_orderField, "North");
    _window->addToRegion(_animationBox, "North");
    _window->addToRegion(_loadingLabel, "North");
    _loadingLabel->setVisible(false);

#ifdef SIERPINSKI_ENABLED
    _window->addToRegion(_sierpinskiButton, "West");
#endif // SIERPINSKI_ENABLED

#ifdef H_TREE_ENABLED
    _window->addToRegion(_htreeButton, "West");
#endif // H_TREE_ENABLED

#ifdef TREE_ENABLED
    _window->addToRegion(_treeButton, "West");
#endif // TREE_ENABLED

#ifdef MANDELBROT_SET_ENABLED
    _window->addToRegion(_mandelbrotButton, "West");
#endif // MANDELBROT_SET_ENABLED

#ifdef FLOOD_FILL_ENABLED
    _window->addToRegion(new GSpacer(1, 10), "West");
    _window->addToRegion(_floodFillButton, "West");
    _window->addToRegion(_colorChooser, "West");
    _window->addToRegion(_floodFillRectsButton, "West");
    _window->addToRegion(_floodFillRandomBox, "West");
    _window->addToRegion(new GSpacer(1, 10), "West");
#endif // FLOOD_FILL_ENABLED

    _window->addToRegion(_diffButton, "West");
    _window->addToRegion(_clearButton, "West");

    _window->addToRegion(_statusBar, "South");

    _loadingLabel->setVisible(false);

    _window->pack();
    _window->center();
    _window->setVisible(true);
    _window->setCanvasSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

bool FractalGUI::readComplexRange(Complex& min, Complex& max) {
#ifdef MANDELBROT_SET_ENABLED
    std::string complexRange = GOptionPane::showInputDialog(
                "Complex number range, in (min .. max) format?",
                /* title */ "",
                /* initial value */ _lastComplexRange);
    if (complexRange.empty()) {
        return false;   // canceled
    }

    // add a bit of spacing to the complex number string;
    // this seems to help with istream parsing inconsistencies on Mac OS
    complexRange = stringReplace(complexRange, "i", " i");
    complexRange = stringReplace(complexRange, "+", " +");
    complexRange = stringReplace(complexRange, "-", " -");

    std::string throwaway;   // for ".."
    std::istringstream rangeInput(complexRange);
    if (!(rangeInput >> min >> throwaway >> max) || min.real() >= max.real() || min.imag() >= max.imag()) {
        std::string status = "Error: Invalid complex number range; using default of "
                + min.toString() + " .. " + max.toString();
        setStatusText(status, /* isError */ true);
        std::cout << status << std::endl;
        return false;
    } else {
        _lastComplexRange = complexRange;
    }

    return true;
#else // MANDELBROT_SET_ENABLED
    // trivially refer to the parameters so we won't get warnings about them being unused
    (void) min;
    (void) max;
    return false;
#endif // MANDELBROT_SET_ENABLED
}

void FractalGUI::runGraphicalFunction(std::function<void()> func) {
    if (isMac() && isAnimated()) {
        // Mac needs animation to be run in a separate thread for some reason;
        // else no repaints will be seen until the end.
        // This causes animation to be slower on Mac, but, oh well.
        // That's what you get for buying inferior hardware.
        GThread::runInNewThreadAsync(func);
    } else {
        // all good computers (Windows + Linux) don't need a silly thread
        func();
    }
}

void FractalGUI::saveCanvas() {
    // save current window to an image (probably for teacher only)
    std::string filename = GFileChooser::showSaveDialog();
    if (!filename.empty()) {
        if (!fileExists(filename)
                || GOptionPane::showConfirmDialog("File " + getTail(filename) + " exists. Overwrite?")) {
            _window->saveCanvasPixels(filename);
            setStatusText("Graphical output saved to " + getTail(filename) + ".");
        }
    }
}

void FractalGUI::setStatusText(const std::string& text, bool isError) {
    _statusBar->setColor(isError ? 0xbb0000 : 0x0);
    _statusBar->setText(text);
}

void FractalGUI::testCompareOutput(const std::string& imageFile) {
    for (std::string path : {imageFile, "output/" + imageFile}) {
        if (fileExists(path)) {
            this->_window->compareToImage(path);
            break;
        }
    }
}

void FractalGUI::testFloodFill(int num) {
    _floodFillRandomBox->setChecked(false);
    _animationBox->setChecked(false);
    this->clear();
    this->_fillMode = false;
    floodFill_addShapes();

    // flood fill 1: initial
    // flood fill 2: click on (350, 75) blue
    // flood fill 3: click on (360, 320) red
    // flood fill 4: click on (360, 320) again, yellow
    // flood fill 5: click on (420, 315) green
    if (num >= 2) {
        _colorChooser->setSelectedItem("Blue");
        doFloodFill(350, 75);
    }
    if (num >= 3) {
        _colorChooser->setSelectedItem("Red");
        doFloodFill(360, 320);
    }
    if (num >= 4) {
        _colorChooser->setSelectedItem("Yellow");
        doFloodFill(360, 320);
    }
    if (num >= 5) {
        _colorChooser->setSelectedItem("Green");
        doFloodFill(420, 315);
    }
    std::string imageFile = "floodfill-" + std::to_string(num) + ".png";
    testCompareOutput(imageFile);
}

void FractalGUI::testSierpinski(int order) {
#ifdef SIERPINSKI_ENABLED
    _xField->setValue(10);
    _yField->setValue(30);
    _sizeField->setValue(300);
    _orderField->setValue(order);
    _animationBox->setChecked(false);
    this->clear();
    this->_fillMode = false;
    doSierpinski();
    std::string imageFile = "sierpinski-x10-y30-size300-order"
            + std::to_string(order) + ".png";
    testCompareOutput(imageFile);
#else // SIERPINSKI_ENABLED
    // empty
#endif // SIERPINSKI_ENABLED
}


/*
 * Main function to run the program.
 */
int main() {
    FractalGUI::instance();
    return 0;
}
