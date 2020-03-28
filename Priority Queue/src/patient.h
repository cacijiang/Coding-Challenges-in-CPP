/*
 * CS 106B/X Patient Queue
 * This file declares the Patient structure.
 * Each Patient structure represents a single element of your
 * priority queue, storing a string of data and an int of priority.
 *
 * Please do not modify this provided file. Your turned-in files should work
 * with an unmodified version of all provided code files.
 *
 * @version 2019/05/04
 */

#ifndef _patient_h
#define _patient_h

#include <iostream>
#include <string>
using namespace std;

struct Patient {
public:
    std::string name;
    int priority;

    /**
     * Constructor to initialize a patient with the given name and priority.
     * If any values are omitted, zero-equivalents will be used.
     */
    Patient(std::string name = "", int priority = 0);
};

/**
 * Overloaded operator to print a patient to an output stream for debugging.
 */
std::ostream& operator <<(std::ostream& out, const Patient& node);

/**
 * Overloaded operators to compare patients.
 */
bool operator <(const Patient& node1, const Patient& node2);
bool operator >(const Patient& node1, const Patient& node2);
bool operator <=(const Patient& node1, const Patient& node2);
bool operator >=(const Patient& node1, const Patient& node2);
bool operator ==(const Patient& node1, const Patient& node2);
bool operator !=(const Patient& node1, const Patient& node2);

#endif
