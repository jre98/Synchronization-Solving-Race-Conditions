/**
 * DO NOT MODIFY THIS SOURCE FILE.
 */


//
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <thread>
#include <chrono>


//
#include "functions.hpp"
#include "puhp-tests/PuhPTests.h"


//
using std::cout, std::cin, std::endl;
using std::ifstream, std::ofstream;
using std::string, std::to_string;
using std::stringstream;
using std::vector;


//
using PuhPTests::Tests, PuhPTests::Test, PuhPTests::OutputCapture;


//	CONSTANTS
extern const char * G_FILE_NAME;			//	<-- Should be present in student source code
const char * G_FILE_NAME_WITH_MUTEXES		= "sync-with-mutexes.txt";
const char * G_FILE_NAME_WITHOUT_MUTEXES	= "sync-without-mutexes.txt";
const size_t G_LINE_COUNT_PER_TYPE_MINIMUM	= 50000;

const string TESTS_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string TESTS_NUMBERS = "0123456789";

string SYNC_WITH_MUTEXES = "";
string SYNC_WITHOUT_MUTEXES = "";
vector<string> SYNC_WITH_MUTEXES_LINES;
vector<string> SYNC_WITHOUT_MUTEXES_LINES;


//	PROTO
void loadFiles();
string loadFile(string filename);
vector<string> splitToLines(string s);
bool hasAllCharacters(string s, string chars);
bool hasAnyCharacters(string s, string chars);
bool hasOnlyTheseCharacters(string s, string chars);
size_t checkForCharacters(string s, string chars);
//
void testInitFiles(Tests& tests);
void testCharacters(Tests& tests);
void testRaceCondition(Tests& tests);
void testCriticalSection(Tests& tests);


//
int main()
{
	//
	cout << "Begin tests" << endl;
	
	//
	Tests tests(30);
	
	//
	vector<std::function<void(Tests&)>> functions = {
		testInitFiles,
		testCharacters,
		testRaceCondition,
		testCriticalSection
	};
	
	//
	for ( auto f : functions ) {
		f(tests);
	}
	
	//
	tests.run(true);
	
	//
	cout << "Tests complete" << endl;
	
	return 0;
}

//
void loadFiles()
{
	//
	SYNC_WITH_MUTEXES = loadFile(G_FILE_NAME_WITH_MUTEXES);
	SYNC_WITHOUT_MUTEXES = loadFile(G_FILE_NAME_WITHOUT_MUTEXES);
	
	SYNC_WITH_MUTEXES_LINES		= splitToLines(SYNC_WITH_MUTEXES);
	SYNC_WITHOUT_MUTEXES_LINES	= splitToLines(SYNC_WITHOUT_MUTEXES);
}
string loadFile(string filename)
{
	//
	ifstream f(filename);
	string line, lines;
	
	//
	while( std::getline(f, line) ) {
		lines += line + "\n";
	}
	
	return lines;
}
vector<string> splitToLines(string s)
{
	//
	vector<string> lines;
	
	//
	stringstream ss(s);
	string line;
	
	//
	while ( std::getline(ss, line) )
	{
		lines.push_back(line);
	}
	
	return lines;
}

//
bool hasAllCharacters(string s, string chars)
{
	//
	return checkForCharacters(s, chars) == chars.size();
}
//
bool hasAnyCharacters(string s, string chars)
{
	//
	return checkForCharacters(s, chars) > 0;
}
//
bool hasOnlyTheseCharacters(string s, string chars)
{
	for ( char c1 : s ) {
		bool found = false;
		if ( c1 == '\n' || c1 == '\r' || c1 == ' ' ) {
			continue;
		}
		for ( char c2 : chars ) {
			if ( c1 == c2 ) {
				found = true;
				break;
			}
		}
		if ( !found ) {
			return false;
		}
	}
	
	return true;
}

//
size_t checkForCharacters(string s, string chars)	// Return number of characters in "chars" present in s
{
	//
	size_t count = 0;
	
	//
	for ( char c1 : chars ) {
		for ( char c2 : s ) {
			if ( c1 == c2 ) {
				count++;
				break;
			}
		}
	}
	
	return count;
}

//
void testInitFiles(Tests& tests)
{
	//
	Test t("Generate files");
	
	//
	t.assertNoException(
		[]()
		{
			//	Remove the files
			//	Don't trust student code, lel
			{
				remove(G_FILE_NAME);
				remove(G_FILE_NAME_WITH_MUTEXES);
				remove(G_FILE_NAME_WITHOUT_MUTEXES);
			}
		},
		0, "Erase existing output files"
	);
	
	//	Generate mutexed output file
	t.assertNoException(
		[]()
		{
			//	Write the file with mutexes
			beginWriting(true);
			
			//
			if ( rename(G_FILE_NAME, G_FILE_NAME_WITH_MUTEXES) != 0 ) {
				throw std::runtime_error(string("Failed to rename sync file from ") + G_FILE_NAME + " to " + G_FILE_NAME_WITH_MUTEXES + "; Are you joining correctly, and closing files?");
			}
		},
		1, "Call beginWriting(true)"
	);
	
	//	Generate race conditioned output file
	t.assertNoException(
		[]()
		{
			//	Write the file without mutexes (race condition)
			beginWriting(false);
			
			//
			if ( rename(G_FILE_NAME, G_FILE_NAME_WITHOUT_MUTEXES) != 0 ) {
				throw std::runtime_error(string("Failed to rename sync file from ") + G_FILE_NAME + " to " + G_FILE_NAME_WITHOUT_MUTEXES + "; Are you joining correctly, and closing files?");
			}
		},
		1, "Call beginWriting(false)"
	);
	
	//	Load files into global vars
	t.assertNoException(
		[]()
		{
			loadFiles();
		},
		1, "Loading generated files"
	);
	
	//	Flush this test so the globals will actually hold something.
	tests.log("Flushing init tests so globals hold something");
	t.run();
	
	//
	t.assertTrue(SYNC_WITH_MUTEXES_LINES.size() >= G_LINE_COUNT_PER_TYPE_MINIMUM * 2, 0, string("At least ") + to_string(G_LINE_COUNT_PER_TYPE_MINIMUM * 2) + " lines should be produced by the mutexed version.");
	t.assertTrue(SYNC_WITHOUT_MUTEXES_LINES.size() >= G_LINE_COUNT_PER_TYPE_MINIMUM, 0, string("At least ") + to_string(G_LINE_COUNT_PER_TYPE_MINIMUM) + " lines should be produced by the race conditioned version.");
	t.assertFalse(SYNC_WITH_MUTEXES.empty(), 1, "Loading from file generated with mutexes, should not be an empty string");
	t.assertFalse(SYNC_WITHOUT_MUTEXES.empty(), 1, "Loading from file generated with mutexes, should not be an empty string");
	
	tests << t;
}

//
void testCharacters(Tests& tests)
{
	//
	Test t("Characters");
	
	//
	string letterNum = TESTS_LETTERS + TESTS_NUMBERS;
	
	//	Mutex output should not have more than the accepted character set
	t.assertTrue(hasOnlyTheseCharacters(SYNC_WITH_MUTEXES, letterNum), 1, "Mutex output should only have characters from the set: " + letterNum);
	
	//	Mutex output should have one of each character
	t.assertTrue(hasAllCharacters(SYNC_WITH_MUTEXES, TESTS_LETTERS), 1, "Mutex output should contain at least one of each letter");
	t.assertTrue(hasAllCharacters(SYNC_WITH_MUTEXES, TESTS_NUMBERS), 1, "Mutex output should contain at least one of each number");
	
	//	Non-mutex output should not have more than the accepted character set
	t.assertTrue(hasOnlyTheseCharacters(SYNC_WITHOUT_MUTEXES, letterNum), 1, "Raced output should only have characters from the set: " + letterNum);
	
	//	Non-mutex output should also have one of each character
	t.assertTrue(hasAllCharacters(SYNC_WITHOUT_MUTEXES, TESTS_LETTERS), 1, "Raced output should contain at least one of each letter");
	t.assertTrue(hasAllCharacters(SYNC_WITHOUT_MUTEXES, TESTS_NUMBERS), 1, "Raced output should contain at least one of each number");
	
	tests << t;
}

//
void testRaceCondition(Tests& tests)
{
	//
	Test t("Race condition");
	
	//	We WANT a race condition
	bool found_race = false;
	for ( string line : SYNC_WITHOUT_MUTEXES_LINES ) {
		if ( hasAnyCharacters(line, TESTS_LETTERS) && hasAnyCharacters(line, TESTS_NUMBERS) ) {
			found_race = true;
			break;
		}
	}
	t.assertTrue(found_race, 10, "When not using mutexes, find a line containing a race condition.");
	
	tests << t;
}

//
void testCriticalSection(Tests& tests)
{
	//
	Test t("Critical section");
	
	//	We do not want any race conditions
	{
		bool found_race = false;
		string found_line = "";
		for ( string line : SYNC_WITH_MUTEXES_LINES ) {
			if ( hasAnyCharacters(line, TESTS_LETTERS) && hasAnyCharacters(line, TESTS_NUMBERS) ) {
				found_race = true;
				found_line = line;
				break;
			}
		}
		t.assertFalse(found_race, 10, "When using a critical section, should not be able to find evidence of a race condition.");
	}
	
	// We want to see whole lines of both line types
	{
		bool found_letter_line = false, found_number_line = false;
		for ( string line : SYNC_WITH_MUTEXES_LINES ) {
			if ( line.find("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z") != string::npos ) {
				found_letter_line = true;
			}
			else if ( line.find("0 1 2 3 4 5 6 7 8 9") != string::npos ) {
				found_number_line = true;
			}
			if ( found_letter_line && found_number_line ) {
				break;
			}
		}
		t.assertTrue(found_letter_line, 1, "Looking for unbroken letter line");
		t.assertTrue(found_number_line, 1, "Looking for unbroken number line");
	}
	
	//	We would also like to see evidence of swapping line types
	//	(not whole blocks each)
	{
		size_t found_letter_to_number = 0, found_number_to_letter = 0;
		string line_last = "";
		for ( string line : SYNC_WITH_MUTEXES_LINES ) {
			if ( !line_last.empty() ) {
				
				//
				if
				(
					line.find("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z") != string::npos
					&& line_last.find("0 1 2 3 4 5 6 7 8 9") != string::npos
				)
				{
					found_letter_to_number++;
				}
				else if
				(
					line.find("0 1 2 3 4 5 6 7 8 9") != string::npos
					&& line_last.find("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z") != string::npos
				)
				{
					found_number_to_letter++;
				}
				
				//
				if ( found_letter_to_number > 1 && found_number_to_letter > 1 ) {
					break;
				}
				
			}
			line_last = line;
		}
		t.assertTrue(found_letter_to_number > 1 && found_number_to_letter > 1, 5, "Looking for evidence of line type switching back and forth. If you're sure this works but you're failing, try running again.");
	}
	
	tests << t;
}








