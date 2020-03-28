/*
 * File: patientqueue-extra.h
 * -----------------------
 * Assignment 5 Part C Coding Patient Queue
 *
 * Extra feature 1: Known list of diseases. Ask what illness or disease they have, and use that to
 * initialize the priority of newly checked-in patients. Then keep a table of known diseases and their
 * respective priorities. Then enqueue function will accept disease name instead of priority.
 *
 * This function declares a class named PriorityQueue. It represents an internal array of patients with
 * their own priority, which is essentially either a min heap or a max heap.
 *
 * @author Liu Jiang (Caci)
 * @version 11/07/2019
*/

#ifndef _patientqueue_h
#define _patientqueue_h

#include <iostream>
#include <string>
#include "patient.h"
#include "vector.h"
#include "map.h"

using namespace std;

class PatientQueue {
public:
    /*
     * Constructor. It will create an internal array with default capacity and initialize the number of
     * patients to be zero. Thus, the first cell' priority of _myPatients is used to denote whether it is
     * a min heap (by 0) or max heap (by 1). It will also create a table of diseases.
     * @param isMinHelp is to determine whether the PatientQueue is min heap.
    */
    PatientQueue(bool isMinHeap = true);

    /* Destructor. It will free up any memory used by the queue's internal array. */
    ~PatientQueue();

    /*
     * This function you will modify the priority of a given existing patient in the queue. This may cause
     * the mofified patient "bubble" the patient forward or backward in the queue. If the given patient
     * name occurs multiple times in the queue, only the priority of the first occurrence of patient will
     * be modified.
     * @param value is the patient's name which will be modified.
     * @param newPriority is the assigned new priority of that patient.
    */
    void changePriority(string value, int newPriority);

    /* This function will remove all elements from PatientQueue. */
    void clear();

    /* This is an optional debug function.*/
    void debug();

    /* This function will remove the frontmost patient in the queue, and return their name as a string. */
    string dequeue();

    /*
     * This function will add the given person into the PatientQueue with the given priority. "Bubble up"
     * appropriately to keep the array in proper heap order. If there is no room in the queue's internal
     * array, the function will resize it to a larger array with twice the capacity.
     * @param value is the patient's name which will be added.
     * @param disease is the patient's disease.
    */
    void enqueue(string value, string disease);

    /*
     * This function is to check whether the queue has no patient.
     * @return a bool, true if the patient queue does not contain any lements and false if it does contain
     * at least one patient.
    */
    bool isEmpty() const;

    /*
     * This function extracts the name of the frontmost patient in the queue, without removing it or
     * altering the state of the queue.
     * @return the name of frontmost patient.
    */
    string peek() const;

    /*
     * This function extracts the integer priority of the frontmost patient in the queue, without removing
     * it or altering the state of the queue.
     * @return the priority of frontmost patient.
    */
    int peekPriority() const;

    /*
     * This function extracts how many patients are in the queue.
     * @return the number of patients currently in queue.
    */
    int size() const;

    /*
     * This operator is to print the patient queue to an output stream such as the console. Each element
     * is printed in the format value (priority).
    */
    friend ostream& operator <<(ostream& out, const PatientQueue& queue);

private:
    /* Member Variables (instance variables / fields) */
    Patient* _myPatients; // internal array
    unsigned long _numPatients; // number of patients
    unsigned long _capacity; // the capacity of internal array
    Map<string, int> _listOfDiseases; // the table of diseases and corresponding priorities

    /*
     * This helper function is to perform "bubble up" to maintain the ordering in a heap. Bubble ith
     * patient up by swaping the positions of its "parent" patient and it.
     * @param i is the index in the array indicating which element is going to move.
     * @bool isMinHeap is true if it's a min heap while false for max heap.
    */
    void bubbleUp(unsigned long i, bool isMinHeap);

    /*
     * This helper function is to perform "bubble down" to maintain the ordering in a heap. Bubble ith
     * patient down by swaping the positions of its "child" patient and it.
     * @param i is the index in the array indicating which element is going to move
     * @bool isMinHeap is true if it's a min heap while false for max heap.
    */
    void bubbleDown(unsigned long i, bool isMinHeap);

    /*
     * This helper function is to swap the positions of two patients with index a and index b.
     * @param a is the index in the queue
     * @param b is another index in the queue
*/
    void swap(unsigned long a, unsigned long b);
};

#endif
