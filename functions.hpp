#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

/**
 * This file contains prototypes for all functions inside functions.cpp
 *
 * Prototypes for starter functions have been provided for you.
 *
 * If you choose to add any helper functions to functions.cpp,
 * 	you'll need to place their prototypes here.
 *
 * If you don't remember how prototypes work,
 * 	quickly look back and forth between this file and functions.cpp
 */

//
void beginWriting(bool use_mutexes);
void* threadAEntry(void* param);
void* threadBEntry(void* param);


#endif
