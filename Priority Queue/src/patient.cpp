/*
 * CS 106B/X Patient Queue
 * This file implements the members of the Patient structure.
 * See patient.h for declaration and documentation of each member.
 *
 * Please do not modify this provided file. Your turned-in files should work
 * with an unmodified version of all provided code files.
 *
 * @version 2019/05/04
 */

#include "patient.h"

Patient::Patient(std::string name, int priority) {
    this->name = name;
    this->priority = priority;
}

std::ostream& operator <<(std::ostream& out, const Patient& node) {
    return out << node.name << " (" << node.priority << ")";
}

bool operator <(const Patient& p1, const Patient& p2) {
    return p1.priority < p2.priority
            || (p1.priority == p2.priority && p1.name < p2.name);
}

bool operator >(const Patient& p1, const Patient& p2) {
    return !(p1 <= p2);
}

bool operator <=(const Patient& p1, const Patient& p2) {
    return (p1 < p2) || (p1 == p2);
}

bool operator >=(const Patient& p1, const Patient& p2) {
    return (p1 > p2) || (p1 == p2);
}

bool operator ==(const Patient& p1, const Patient& p2) {
    return p1.priority == p2.priority && p1.name == p2.name;
}

bool operator !=(const Patient& p1, const Patient& p2) {
    return !(p1 == p2);
}
