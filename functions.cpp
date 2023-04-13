
/**
 * This is where you'll place your function definitions.
 *
 * Note that functions.hpp contains prototypes for each function listed below.
 *
 * You may choose to create additional helper functions if you wish,
 * 	but their prototypes must also be added to functions.hpp
 *
 */

/**
 * Header includes
 * The first header include has been added for you.
 * You may add additional headers below, if needed
 */
#include "functions.hpp"
#include <cstddef>
#include <fstream>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
//	TODO: More header includes?


/**
 * Globals
 * We're using globals in this Assignment for simplicity
 * (they will allow us to avoid using classes)
 */

/********************************************
 * Probably don't want to mess with this area
 */
bool G_USE_MUTEXES = true;
const char * G_FILE_NAME = "sync.txt";
const size_t G_LINE_COUNT_PER_TYPE = 50000;
/********************************************/

/**
 * If you need to add custom globals, add them here:
 */
 // Declare mutex as global to make things simpler
 pthread_mutex_t mtx;
// TODO: More globals?


/**
 * beginWriting()
 *
 * Consider this function the main entry point for your code.
 *
 * First, it should look at the incoming value of use_mutexes,
 *	and assign it to the global variable above, G_USE_MUTEXES.
 *
 * Next, it should spawn two threads: One for each of Thread-A and Thread-B.
 * Each thread enters its same-named function.
 * If a thread fails to launch, throw an std::runtime_error!
 *
 * The function should then join each thread before returning.
 */
void beginWriting(bool use_mutexes)
{
	// Make sure the file is clear and then close it to prepare to write to file
	std::ofstream myFile(G_FILE_NAME, std::ios_base::trunc);
	myFile.close();
	// Assigns G_USE_MUTEX to the value that has been passed into the function
	G_USE_MUTEXES = use_mutexes;

	// To store id value of thread A
	pthread_t t_id_A;
	// To store id value of thread B
	pthread_t t_id_B;

	// If we are using mutexes, initialize our mutex
	if(G_USE_MUTEXES) {
		int a = pthread_mutex_init(&mtx, NULL);
		// If an error occurs, print out error message and abort
		if(a < 0) {
			std::cout << "Failure to create mutex\n";
			std::abort();
		}
	}

	// Create threads
	if(pthread_create(&t_id_A, NULL, threadAEntry, NULL) < 0) {
		std::cout << "Failure to create thread\n";
		std::abort();
	}
	if(pthread_create(&t_id_B, NULL, threadBEntry, NULL) < 0) {
		std::cout << "Failure to create thread\n";
		std::abort();
	}

	// Join threads
	pthread_join(t_id_A, NULL);
	pthread_join(t_id_B, NULL);

	// If we were using mutexes, destroy the mutex afterwards
	if(G_USE_MUTEXES) {
		pthread_mutex_destroy(&mtx);
	}
}


/**
 * threadAEntry()
 *
 * This is the entry point for Thread-A
 *
 * It should write one type of line to your output file, many times.
 * (in fact, G_LINE_COUNT_PER_TYPE times)
 *
 * It should have the ability to choose whether to use mutexes, or not
 * Have it examine the value of the global variable G_USE_MUTEXES,
 * 	and use knowledge to decide whether to lock/unlock mutexes, or not.
 *
 * You can read the global variable G_FILE_NAME for the target file name.
 *
 * If you get a warning about the return value, just return nullptr or NULL
 */
void* threadAEntry(void* param)
{
	// Use ofstream to write to file
	std::ofstream myfile(G_FILE_NAME, std::ios_base::app);

	// Loop G_LINE_COUNT_PER_TYPE times and print out 0 -> 9 on each iteration
	for(unsigned int i = 0; i < G_LINE_COUNT_PER_TYPE; i++) {
		// Lock mutex if we are using mutexes in this run
		if(G_USE_MUTEXES) { pthread_mutex_lock(&mtx); }
		// This inner loop writes the numbers to the file
		for(int j = 0; j < 10; j++) {
			// Write number and space to file
			myfile << j;
			myfile << std::flush;
			myfile << std::flush;
			myfile << " ";
			myfile << std::flush;
			myfile << std::flush;
		}
		// After entire line is written, advance to next line in file
		myfile << "\n";
		myfile << std::flush;
		myfile << std::flush;
		// Unlock mutex if we are using them in this run
		if(G_USE_MUTEXES) { pthread_mutex_unlock(&mtx); }
	}
	// Close file
	myfile.close();
	// Return null to prevent compiler warnings
	return NULL;
}

/**
 * threadBEntry()
 *
 * This function should behave the same way as threadAEntry(),
 * 	but write a different type of line.
 */
void* threadBEntry(void* param)
{
	// Use ofstream to write to file
	std::ofstream myfile(G_FILE_NAME, std::ios_base::app);

	// Loop G_LINE_COUNT_PER_TYPE times
	for(unsigned int i = 0; i < G_LINE_COUNT_PER_TYPE; i++) {
		// Lock mutex if we are using them
		if(G_USE_MUTEXES) { pthread_mutex_lock(&mtx); }
		// Using ASCII codes to write characters to file
		for(int j = 65; j < 91; j++) {
			myfile << char(j);
			myfile << std::flush;
			myfile << std::flush;
			myfile << " ";
			myfile << std::flush;
			myfile << std::flush;
		}
		// After entire line has been written, advance to next line in file
		myfile << "\n";
		myfile << std::flush;
		myfile << std::flush;

		// If using mutexes, unlock the mutex here
		if(G_USE_MUTEXES) { pthread_mutex_unlock(&mtx); }
	}
	// Close file
	myfile.close();
	// Return null to prevent compiler warnings
	return NULL;
}
