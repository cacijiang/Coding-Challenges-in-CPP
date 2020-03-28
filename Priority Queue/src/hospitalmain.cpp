/*
 * CS 106B/X Patient Queue
 * This file contains the main program and user interface for testing your
 * various patient queues.
 *
 * You can modify this file if you like, to add other tests and so on.
 * But your turned-in files should work properly with an unmodified version of
 * all provided code files.
 *
 * @version 2019/05/03
 * - 19sp initial version; heap-based PQ only, supports min or max heap
 * @version 2017/10/31
 * - 17au version; supports multiple types of PQs
 */

#include <algorithm>  // For sort, reverse
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "console.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "patientqueue.h"
using namespace std;

static const bool RIG_RANDOM_NUMBERS = true;  // true to use same random sequence every time
static const int RIG_RANDOM_SEED = 106;       // seed for fixing random numbers
static const int RANDOM_STRING_LENGTH = 6;    // max length of random strings in bulk en/deQ

// function prototype declarations
string randomString(int maxLength);
void menu(PatientQueue& queue);
void bulkDequeue(PatientQueue& queue, int count);
void bulkEnqueue(PatientQueue& queue, int count);
void easterEgg();

int main() {
    cout << "CS 106B Hospital Patient Check-in System" << endl;
    cout << "========================================" << endl;

    if (RIG_RANDOM_NUMBERS) {
        setRandomSeed(RIG_RANDOM_SEED);
    }

    bool isMinHeap = getYesOrNo("Process the minimum priority patients first (Y/N)? ");

    // these {} braces are so that your queue's destructor will be called sooner
    {
        PatientQueue queue(isMinHeap);
        menu(queue);
    }
    // your queue's destructor will be called here

    cout << endl;
    cout << "Exiting." << endl;
    return 0;
}

/*
 * Returns a randomly generated string of letters up to the given length.
 */
string randomString(int maxLength) {
    int length = randomInteger(1, maxLength);
    ostringstream out;
    for (int i = 0; i < length; i++) {
        char ch = (char) ('a' + randomInteger(0, 25));
        out << ch;
    }
    return out.str();
}

/*
 * Prompts the user to perform tests on your patient queue.
 * Each method of the queue has a corresponding letter or symbol to type that
 * will call that method on the queue and display the results as appropriate.
 */
void menu(PatientQueue& queue) {
    while (true) {
        cout << endl;
        queue.debug();
        cout << "Current patient queue:" << endl;
        cout << queue;
        cout << "  (size=" << queue.size() << ")";
        if (queue.isEmpty()) {
            cout << " (empty)" << endl;
        } else {
            cout << " (not empty)" << endl;
        }

        string prompt = "(E)nqueue, (D)equeue, (P)eek, (C)hange, c(L)ear, (B)ulk, (Q)uit?";
        string choice = toUpperCase(trim(getLine(prompt)));
        if (choice.empty() || choice == "Q") {
            break;
        } else if (choice == "E") {
            string value = getLine("Name? ");
            int priority = getInteger("Priority? ");
            queue.enqueue(value, priority);
        } else if (choice == "D") {
            string value = queue.dequeue();
            cout << "Dequeued patient: \"" << value << "\"" << endl;
        } else if (choice == "P") {
            string name = queue.peek();
            int pri = queue.peekPriority();
            cout << "Front of queue is \"" << name << "\" with priority " << pri << endl;
        } else if (choice == "C") {
            string value = getLine("Name? ");
            int newPriority = getInteger("New priority? ");
            queue.changePriority(value, newPriority);
        } else if (choice == "L") {
            queue.clear();
        } else if (choice == "B") {
            int count = getInteger("How many patients? ");
            string choice2 = toUpperCase(trim(getLine("(E)nqueue or (D)equeue: ")));
            if (choice2 == "E") {
                bulkEnqueue(queue, count);
            } else if (choice2 == "D") {
                bulkDequeue(queue, count);
            }
        } else if (choice == "TNG") {
            easterEgg();
        }
    }
}

/*
 * Dequeues the given number of patients from the queue.
 * Helpful for bulk testing.
 */
void bulkDequeue(PatientQueue& queue, int count) {
    for (int i = 1; i <= count; i++) {
        string value = queue.dequeue();
        cout << "#" << i << ", processing patient: \"" << value << "\"" << endl;
    }
}

/*
 * Enqueues the given number of patients into the queue, priorities either random
 * or in ascending or descending order.
 * Helpful for bulk testing.
 */
void bulkEnqueue(PatientQueue& queue, int count) {
    string choice2;

    while (true) {
        choice2 = trim(toUpperCase(getLine("R)andom, A)scending, D)escending? ")));
        if (choice2 == "R" || choice2 == "A" || choice2 == "D") {
            break;
        }
        cout << "Your choice was invalid. Please try again." << endl;
    }

   Vector<int> priorities;
   if (choice2 == "R") {
        for (int i = 0; i < count; i++) {
            priorities.add(randomInteger(1, count));
        }
    } else if (choice2 == "A") {
        for (int i = 1; i <= count; i++) {
            priorities.add(i);
        }
    } else if (choice2 == "D") {
        for (int i = count; i > 0; i--) {
            priorities.add(i);
        }
    }
    for (int p : priorities) {
        string value = randomString(RANDOM_STRING_LENGTH);
        queue.enqueue(value, p);
        cout << "New patient \"" << value << "\" with priority " << p << endl;
    }
}


/**
 * This assignment is about a queue, so here is a silly hidden function that
 * prints some ASCII art and text about the character Q from the TV show,
 * "Star Trek: The Next Generation".
 *
 * Does it have very much to do with this assignment? No.
 * Did that stop us from adding it? NO.
 *
 * Total number of students who have ever mentioned to us that they found this: 7.
 *
 * If you haven't seen ST:TNG, well, I don't know what to tell you.  Go watch it.
 *
 * Picard?  Engaged.
 * Data?    Intriguing.
 * Riker?   Rikery.
 * Worf?    Son of Mogh.
 * Wesley?  Shut up Wesley!
 * Q?       An irritable godlike pandimensional being? Money.
 * Troi?    ... Well, you can't win 'em all.
 *
 * Text courtesy of: http://en.memory-alpha.org/wiki/Q
 * ASCII art courtesy of: http://www.chris.com/ascii/index.php?art=television/star%20trek
 */
void easterEgg() {
    cout << "                                _____..---========+*+==========---.._____" << endl;
    cout << "   ______________________ __,-='=====____  =================== _____=====`=" << endl;
    cout << "  (._____________________I__) - _-=_/    `---------=+=--------'" << endl;
    cout << "      /      /__...---===='---+---_'" << endl;
    cout << "     '------'---.___ -  _ =   _.-'    *    *    *   *" << endl;
    cout << "                    `--------'" << endl;
    cout << endl;
    cout << "                 _____.-----._____" << endl;
    cout << "    ___----~~~~~~. ... ..... ... .~~~~~~----___" << endl;
    cout << " =================================================" << endl;
    cout << "    ~~~-----......._____________.......-----~~~" << endl;
    cout << "     (____)          \\   |   /          (____)" << endl;
    cout << "       ||           _/   |   \\_           ||" << endl;
    cout << "        \\\\_______--~  //~~~\\\\  ~--_______// " << endl;
    cout << "         `~~~~---__   \\\\___//   __---~~~~'     " << endl;
    cout << "                   ~~-_______-~~" << endl;
    cout << endl;
    cout << "                  xxxXRRRMMMMMMMMMMMMMMMxxx,." << endl;
    cout << "              xXXRRRRRXXXVVXVVXXXXXXXRRRRRMMMRx," << endl;
    cout << "            xXRRXRVVVVVVVVVVVVVVVXXXXXRXXRRRMMMMMRx." << endl;
    cout << "          xXRXXXVVVVVVVVVVVVVVVVXXXXVXXXXXXRRRRRMMMMMxx." << endl;
    cout << "        xXRRXXVVVVVttVtVVVVVVVVVtVXVVVVXXXXXRRRRRRRMMMMMXx" << endl;
    cout << "      xXXRXXVVVVVtVttttttVtttttttttVXXXVXXXRXXRRRRRRRMMMMMMXx" << endl;
    cout << "     XRXRXVXXVVVVttVtttVttVttttttVVVVXXXXXXXXXRRRRRRRMMMMMMMMVx" << endl;
    cout << "    XRXXRXVXXVVVVtVtttttVtttttittVVVXXVXVXXXRXRRRRRMRRMMMMMMMMMX," << endl;
    cout << "   XRRRMRXRXXXVVVXVVtttittttttttttVVVVXXVXXXXXXRRRRRMRMMMMMMMMMMM," << endl;
    cout << "   XXXRRRRRXXXXXXVVtttttttttttttttttVtVXVXXXXXXXRRRRRMMMMMMMMMMMMM," << endl;
    cout << "   XXXXRXRXRXXVXXVtVtVVttttttttttttVtttVXXXXXXXRRRRRMMMMMMMMMMMMMMMR" << endl;
    cout << "   VVXXXVRVVXVVXVVVtttititiitttttttttttVVXXXXXXRRRRRMRMMMMMMMMMMMMMMV" << endl;
    cout << "   VttVVVXRXVVXtVVVtttii|iiiiiiittttttttitXXXRRRRRRRRRRMMMMMMMMMMMMMM" << endl;
    cout << "   tiRVVXRVXVVVVVit|ii||iii|||||iiiiiitiitXXXXXXXXRRRRRRMMMMMMMMMMMMM" << endl;
    cout << "    +iVtXVttiiii|ii|+i+|||||i||||||||itiiitVXXVXXXRRRRRRRRMMMMMMRMMMX" << endl;
    cout << "    `+itV|++|tttt|i|+||=+i|i|iiii|iiiiiiiitiVtti+++++|itttRRRRRMVXVit" << endl;
    cout << "     +iXV+iVt+,tVit|+=i|||||iiiiitiiiiiiii|+||itttti+=++|+iVXVRV:,|t" << endl;
    cout << "     +iXtiXRXXi+Vt|i||+|++itititttttttti|iiiiitVt:.:+++|+++iXRMMXXMR" << endl;
    cout << "     :iRtiXtiV||iVVt||||++ttittttttttttttttXXVXXRXRXXXtittt|iXRMMXRM" << endl;
    cout << "      :|t|iVtXV+=+Xtti+|++itiiititittttVttXXXXXXXRRRXVtVVtttttRRMMMM|" << endl;
    cout << "        +iiiitttt||i+++||+++|iiiiiiiiitVVVXXRXXXRRRRMXVVVVttVVVXRMMMV" << endl;
    cout << "         :itti|iVttt|+|++|++|||iiiiiiiittVVXRRRMMMMMMRVtitittiVXRRMMMV" << endl;
    cout << "           `i|iitVtXt+=||++++|++++|||+++iiiVVXVRXRRRV+=|tttttttiRRRMMM|" << endl;
    cout << "             i+++|+==++++++++++++++|||||||||itVVVViitt|+,,+,,=,+|itVX'" << endl;
    cout << "              |+++++.,||+|++++=+++++++|+|||||iitt||i||ii||||||itXt|" << endl;
    cout << "              t||+++,.=i+|+||+++++++++++++|i|ittiiii|iiitttttXVXRX|" << endl;
    cout << "              :||+++++.+++++++++|++|++++++|||iii||+:,:.-+:+|iViVXV" << endl;
    cout << "              iii||+++=.,+=,=,==++++++++++|||itttt|itiittXRXXXitV'" << endl;
    cout << "             ;tttii||++,.,,,.,,,,,=++++++++++|iittti|iiiiVXXXXXXV" << endl;
    cout << "            tVtttiii||++++=,,.  . ,,,=+++++++|itiiiiiii||||itttVt" << endl;
    cout << "           tVVttiiiii||||++++==,. ..,.,+++=++iiiiiitttttVVXXRRXXV" << endl;
    cout << "        ..ttVVttitttii||i|||||+|+=,.    .,,,,==+iittVVVXRRMXRRRV" << endl;
    cout << "...'''ittitttttitVttttiiiiii|ii|++++=+=..... ,.,,||+itiVVXXVXV" << endl;
    cout << "      ,|iitiiitttttttiiiii||ii||||||||+++++,.i|itVt+,,=,==........." << endl;
    cout << "        ,|itiiiVtVtiii||iiiiii|||||||++||||tt|VXXRX|  ....  ..     ' ' '." << endl;
    cout << "          ,,i|ii||i||+|i|i|iiiiiiii||||ittRVVXRXRMX+, .  ...   .         ," << endl;
    cout << "    .       .,+|++|||||ii|i|iiiitttVVttXVVXVXRRRRXt+. .....  . .       ,. ." << endl;
    cout << "  . .          ,,++|||||||i|iiitVVVXXXXVXXVXXRRRV+=,.....  ....  ..       .." << endl;
    cout << "                  .,,++|||i|iittXXXXRMViRXXXXRVt+=, ..    ...... .        .." << endl;
    cout << "                   ,XX+.=+++iitVVXXXRXVtXXVRRV++=,..... .,, .              ." << endl;
    cout << "            ....       +XX+|i,,||tXRRRXVXti|+++,,. .,,. . . .. .      . ...." << endl;
    cout << "  . .          .      ..  ..........++,,..,...,.... ..             .. ..." << endl;
    cout << endl;
    cout << "Q was a highly powerful entity from a race of omnipotent, godlike "
              << "beings also known as the Q. Q appeared to the crews of several "
              << "Starfleet vessels and outposts during the 2360s and 2370s. "
              << "All command level officers in Starfleet are briefed on his "
              << "existence. One such briefing was attended by Benjamin Sisko "
              << "in 2367. He typically appears as a Humanoid male (though "
              << "he can take on other forms if he wishes), almost "
              << "always dressed in the uniform of a Starfleet captain." << endl;
    cout << "In every appearance, he demonstrated superior capabilities, but "
              << "also a mindset that seemed quite unlike what Federation scientists "
              << "expected for such a powerful being. He has been described, in "
              << "turn, as 'obnoxious,' 'interfering,' and a 'pest'. However, "
              << "underneath his acerbic attitude, there seemed to be a hidden "
              << "agenda to Q's visits that often had the best interests of "
              << "Humanity at their core." << endl;
    cout << endl;
    cout << "\"Worf... Eat any good books lately?\" -- Q" << endl;
}
