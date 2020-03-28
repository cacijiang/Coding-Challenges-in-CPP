/*
 * File: patientqueue-extra.cpp
 * -----------------------
 * Assignment 5 Part C Coding Patient Queue
 *
 * This file provides the implementation of the bodies of the member functions, constructor and destructor
 * for the PatientQueue class declared in patientqueue.h. Each patient in the PatientQueue contains its
 * name, a string, and a priority, an integer. The constructor provides the options for a min heap
 * or max heap, ordered by the patient's priority. Member functions allow clients to change patient's
 * priority, dequeue, enqueue, and peek the frontmost patient's name and priority.
 *
 * @author Liu Jiang (Caci)
 * @version 11/07/2019
*/

#include "patientqueue.h"
#include "map.h"
#include "vector.h"

static unsigned long defaultCapacity = 10; // default capacity of PatientQueue

PatientQueue::PatientQueue(bool isMinHeap) {
    _capacity = defaultCapacity;
    _myPatients = new Patient[_capacity+1]; // the patient will start from index 1.
    _numPatients = 0;
    if(isMinHeap) {
        _myPatients[0].priority = 0; // to denote min heap
    } else {
        _myPatients[0].priority = 1; // to denote max heap
    }
    // a list of diseases recorded in medical system
    Vector<string> diseases = {"brain injury", "heart disease", "gastroenteritis", "cancer", "flu",
                               "asthma", "HIV", "kidney disease", "diabetes", "obesity"};
    // build the table of diseases with their associated priority
    for(int i = 0; i < diseases.size(); i++) {
        _listOfDiseases[diseases[i]] = i+1;
    }
}

PatientQueue::~PatientQueue() {
    delete[] _myPatients;
}

void PatientQueue::changePriority(string value, int newPriority) {
    unsigned long index = 1;
    // loop across the queue until it finds the patient with the name of "value"
    while(index <= _numPatients) {
        if(_myPatients[index].name == value) {
            break;
        }
        index++;
    }
    // if the function finds the patient
    if(index <= _numPatients) {
        int oldPriority = _myPatients[index].priority; // original priority of the patient
        _myPatients[index].priority = newPriority;
        bool isMinHeap = _myPatients[0].priority == 0; // know whether it's a min heap from the 1st cell
        // do nothing if the original priority equals to changed value
        if(newPriority == oldPriority) {
            return;
        } else {
            // perform different operations (bubble up or down) by calling helper functions for min heap
            // and max heap
            if(isMinHeap) {
                if(newPriority < oldPriority) {
                    bubbleUp(index, true);
                } else {
                    bubbleDown(index, true);
                }
            } else {
                if(newPriority < oldPriority) {
                    bubbleDown(index, false);
                } else {
                    bubbleUp(index, false);
                }
            }
        }
    }
}

void PatientQueue::clear() {
    Patient* emptyPatients = new Patient[_capacity+1] (); // initialize an empty array
    emptyPatients[0] = _myPatients[0]; // maintain the min heap or max heap property of the queue
    delete[] _myPatients;
    _myPatients = emptyPatients;
    _numPatients = 0; // update the number of patients in the queue
}

void PatientQueue::debug() {
    // empty
}

string PatientQueue::dequeue() {
    // throw an error if the user tries to dequeue in an empty queue
    if(isEmpty()) {
        throw "Can't dequeue from an empty patient queue!";
    }
    string patientName = _myPatients[1].name; // the frontmost patient's name
    // replace the frontmost patient by the fartherest patient at first
    _myPatients[1] = _myPatients[_numPatients];
    _myPatients[_numPatients] = Patient();
    _numPatients--; // update the number of patient
    bool isMinHeap = _myPatients[0].priority == 0; // get the heap property
    // perform bubble down by calling helper function bubbleDown if needed
    if(isMinHeap) {
        bubbleDown(1, true);
    } else {
        bubbleDown(1, false);
    }
    return patientName;
}

void PatientQueue::enqueue(string value, string disease) {
    // throw an error if the disease is not contained in table
    if(!_listOfDiseases.containsKey(disease)) {
        throw "Sorry! Your disease is not in the record of our medical system!";
    }
    int priority = _listOfDiseases[disease];
    // if the queue is full
    if(_numPatients == _capacity) {
        // initialize a new array with doubled capacity
        Patient* longerPatients = new Patient[2*_capacity+1] ();
        // copy the patient information from the old array
        for(unsigned long i = 0; i <= _numPatients; i++) {
            longerPatients[i] = _myPatients[i];
        }
        _capacity *= 2; // update capacity
        delete[] _myPatients;
        _myPatients = longerPatients;
    }
    _numPatients++; // update the number of patients
    unsigned long i = _numPatients;
    _myPatients[i] = Patient(value, priority); // assgin new patient
    bool isMinHeap = _myPatients[0].priority == 0; // get the heap property
     // perform bubble up by calling helper function bubbleUp if needed
    if(isMinHeap) {
        bubbleUp(i, true);
    } else {
        bubbleUp(i, false);
    }
}

bool PatientQueue::isEmpty() const {
    return _numPatients == 0;
}

string PatientQueue::peek() const {
    // throw an error if the user tries to peek in an empty queue
    if(isEmpty()) {
        throw "Can't peek from an empty patient queue!";
    }
    return _myPatients[1].name;   // frontmost patient's name
}

int PatientQueue::peekPriority() const {
    // throw an error if the user tries to peek in an empty queue
    if(isEmpty()) {
        throw "Can't peek from an empty patient queue!";
    }
    return _myPatients[1].priority; // frontmost patient's priority
}

int PatientQueue::size() const {
    return int(_numPatients);  // the number of patients in queue
}

void PatientQueue::bubbleUp(unsigned long i, bool isMinHeap) {
    unsigned long parentI = i/2; // index of ith's "parent" patient
    if(isMinHeap) {
        // bubble up ith patient if the parent's priority is bigger than ith's one in a min heap
        if(parentI > 0 && (_myPatients[parentI] > _myPatients[i])) {
            swap(i, parentI);
            bubbleUp(parentI, true); // continue checking whether it still needs to bubble up
        }
    } else {
        // bubble up ith patient if the parent's priority is smaller than ith's one in a max heap
        if(parentI > 0 && (_myPatients[parentI] < _myPatients[i])) {
            swap(i, parentI);
            bubbleUp(parentI, false);
        }
    }
}

void PatientQueue::bubbleDown(unsigned long i, bool isMinHeap) {
    unsigned long leftI = 2*i, rightI = 2*i+1; // index of ith's left "child" and right "child"
    if(isMinHeap) {
        // if both children exist and one of the children's priority is smaller than ith's one in min heap
        if(((leftI <= _numPatients && rightI <= _numPatients)
            && (_myPatients[i] > _myPatients[leftI] || _myPatients[i] > _myPatients[rightI]))) {
            // swap ith patient with the child having smaller priority
            if(_myPatients[leftI] < _myPatients[rightI]) {
                swap(i, leftI);
                bubbleDown(leftI, true); // continue checking whether it still needs to bubble down
            } else {
                swap(i, rightI);
                bubbleDown(rightI, true);
            }
        } else if((leftI <= _numPatients && rightI > _numPatients)
                  && (_myPatients[i] > _myPatients[leftI])){
            // if only the left child exists and the child's priority is smaller than ith's one
            swap(i, leftI);
            bubbleDown(leftI, true);
        }
    } else {
        // if both children exist and one of the children's priority is bigger than ith's one in max heap
        if((leftI <= _numPatients && rightI <= _numPatients)
                && (_myPatients[i] < _myPatients[leftI] || _myPatients[i] < _myPatients[rightI])) {
            // swap ith patient with the child having bigger priority
            if(_myPatients[leftI] > _myPatients[rightI]) {
                swap(i, leftI);
                bubbleDown(leftI, false);
            } else {
                swap(i, rightI);
                bubbleDown(rightI, false);
            }
        } else if((leftI <= _numPatients && rightI > _numPatients)
                  && (_myPatients[i] < _myPatients[leftI])) {
            // if only the left child exists and the child's priority is bigger than ith's one
            swap(i, leftI);
            bubbleDown(leftI, false);
        }
    }
}

void PatientQueue::swap(unsigned long a, unsigned long b) {
    Patient temp = _myPatients[a];
    _myPatients[a] = _myPatients[b];
    _myPatients[b] = temp;
}

// this operator is declared as a 'friend' of the queue class
ostream& operator <<(ostream& out, const PatientQueue& queue) {
    out << "{";
    if(!queue.isEmpty()) {
        // print each patient by the name and priority in the queue
        for(unsigned long i = 1; i <= queue._numPatients; i++) {
            out << queue._myPatients[i];
            if(i != queue._numPatients) {
                out << ", ";
            }
        }
    }
    out << "}";
    return out;
}
