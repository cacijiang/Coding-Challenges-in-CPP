/*
 * CS 106B/X Fractal Recursion Problems
 * This instructor-provided file declares a class representing the overall
 * graphical user interface (GUI) for this part of the program.
 * See fractalgui.cpp for the implementation of each member.
 *
 * You don't need to modify this file.
 * Your recursive functions should work with an unmodified version of this file.
 *
 * @author Marty Stepp
 * @version 2019/04/27
 * - 19sp improvements: speed up Mac drawing; clean up functional/ifdef code; comments
 * @version 2019/04/23
 * - 19sp update: add thread for drawing operations to improve animation on Mac
 * @version 2018/10/11
 * - 18au 106B initial version;
 *   added H-tree (but disabled);
 *   new Qt-based GUI;
 *   crosshairs on drag rectangle;
 *   2px-thick Sierpinski lines; etc.
 * @version 2018/01/25
 * - disabled Mandelbrot set for CS 106B version
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

#ifndef _fractalgui_h
#define _fractalgui_h

// uncomment to indicate which fractals are enabled
#define SIERPINSKI_ENABLED 1
#define FLOOD_FILL_ENABLED 1
//#define H_TREE_ENABLED 1
//#define TREE_ENABLED 1
//#define MANDELBROT_SET_ENABLED 1

// display a "loading" animated GIF (doesn't work in current version of lib)
//#define LOADING_LABEL_ENABLED 1


#include <functional>
#include <string>
#include "complex.h"
#include "map.h"
#include "ginteractors.h"
#include "gobjects.h"
#include "gwindow.h"

class FractalGUI {
public:
    /**
     * Constructs the GUI.
     */
    FractalGUI();

    /**
     * Frees up memory used by the GUI.
     */
    virtual ~FractalGUI();

    /**
     * Removes all shapes from the screen.
     */
    void clear();

    /**
     * Creates and configures all graphical controls in the GUI.
     */
    void initialize();

    /**
     * Returns a pointer to the singleton FractalGUI object.
     * If that object has not yet been created, creates it.
     */
    static FractalGUI* instance();

private:
    /**
     * Enables/disables buttons during drawing operations.
     */
    void doEnabling(bool enabled);

    /**
     * Initiates a flood fill operation on the window.
     */
    void doFloodFill(double x, double y);

    /**
     * Initiates a recursive H-tree operation on the window.
     */
    void doHTree();

    /**
     * Initiates a Mandelbrot Set operation on the window.
     */
    void doMandelbrot();

    /**
     * Initiates a draw Sierpinski triangle operation on the window.
     */
    void doSierpinski();

    /**
     * Initiates a recursive tree operation on the window.
     */
    void doTree();

    /**
     * Adds a bunch of rectangles to the screen for use with the floodFill function.
     */
    void floodFill_addShapes();

    /**
     * Returns a map of available colors for flood fill.
     */
    const Map<std::string, int>& getColorMap();

    /**
     * Responds to keyboard events in the window.
     */
    void handleKeyEvent(const GEvent& keyEvent);

    /**
     * Responds to mouse events in the window.
     */
    void handleMouseEvent(const GEvent& mouseEvent);

    /**
     * Returns true if all text fields have valid integer coordinates entered into them.
     */
    bool hasAllCoords() const;

    /**
     * Returns true if the Animation box is checked
     */
    bool isAnimated() const;

    /**
     * Returns true if the GUI is running on a Mac.
     * We need to know this due to some OS-specific implementation details
     * related to animation and painting.
     */
    bool isMac() const;

    /**
     * Signal the beginning and ending of a loading operation, such as a long
     * drawing or animation or fractal.
     */
    void loadingBegin();
    void loadingEnd();

    /**
     * Tries to read a complex number range for Mandelbrot Set
     * and store it in the given two Complex number objects.
     * Returns true if the GUI should proceed with drawing the set.
     */
    bool readComplexRange(Complex& min, Complex& max);

    /**
     * Runs the given graphical code, possibly in a separate thread.
     */
    void runGraphicalFunction(std::function<void()> func);

    /**
     * Pops up a file chooser to save the pixels of the window's canvas.
     */
    void saveCanvas();

    /**
     * Scales the fractal based on zoom factor and where user clicked.
     */
    // void scale(int x, int y, double zoomFactor);

    /**
     * Sets the bottom status bar text to display in the southern area.
     * If isError is true, shows it in a red error color.
     */
    void setStatusText(const std::string& text, bool isError = false);

    /**
     * Helpers for testing predefined inputs for some drawing functions.
     */
    void testFloodFill(int num);
    void testSierpinski(int num);
    void testCompareOutput(const std::string& filename);


    // constants
    static const int WINDOW_WIDTH;               // size of window's canvas
    static const int WINDOW_HEIGHT;
    static const int FLOOD_FILL_NUM_SHAPES;      // num rects to add during f.fill
    static const int FLOOD_FILL_MIN_RECT_SIZE;   // range of sizes of random rects
    static const int FLOOD_FILL_MAX_RECT_SIZE;
    static const int DRAG_RECT_COLOR;            // color of temporary 'drag' lines
    static const int MIN_ORDER;                  // range of orders allowed for fractals
    static const int MAX_ORDER;
    static const int FLOOD_FILL_RANDOM_SEED;     // to 'rig' random number generator; set to <= 0 to disable
    static const bool FLOOD_FILL_READ_FILE;      // if true, load same rects every time from text file
    static const std::string FLOOD_FILL_RECTS_FILE_NAME;

    // overall window
    GWindow* _window;

    // west buttons/controls to draw fractals
    GButton* _sierpinskiButton;
    GButton* _htreeButton;
    GButton* _treeButton;
    GButton* _mandelbrotButton;
    GButton* _floodFillButton;
    GChooser* _colorChooser;          // flood fill / Mandelbrot colors drop-down
    GButton* _floodFillRectsButton;
    GCheckBox* _floodFillRandomBox;
    GButton* _diffButton;
    GButton* _clearButton;

    // north text fields for figure parameters
    GTextField* _xField;
    GTextField* _yField;
    GTextField* _sizeField;
    GTextField* _orderField;
    GCheckBox* _animationBox;
    GLabel* _loadingLabel;

    // south status bar
    GLabel* _statusBar;

    // temporary dragging rectangle for figure size
    GRect* _dragRect;
    GLine* _dragLineH;
    GLine* _dragLineV;

    // temporary flags to remember if we are in midst of a flood fill/Mandelbrot
    bool _fillMode;
    bool _fillInProgress = false;

    // last complex number range typed for Mandelbrot Set
    std::string _lastComplexRange;

    static FractalGUI* _instance;
};

#endif // _fractalgui_h
