
/**
 * This file will not be graded. It exists purely for your own debugging pleasure.
 * You may change this file to anything you like, to help your development.
 */


//
#include "functions.hpp"
#include <iostream>
#include <string>


//
using std::cout;
using std::cin;
using std::endl;
using std::string;


//
int main(int argc, char * argv[])
{
	//
	cout << "main() - Begin!" << endl;
	
	//	Support CLI argument to control whether mutexes are used
	bool use_mutexes = true;
	if ( argc == 2 ) {
		string mode = argv[1];
		if ( mode == "on" || mode == "true" || mode == "yes" || mode == "mutex" ) {
			use_mutexes = true;
		}
		else if ( mode == "off" || mode == "false" || mode == "no" || mode == "race" ) {
			use_mutexes = false;
		}
	}
	cout << "Use mutexes: " << (use_mutexes ? "TRUE" : "FALSE") << endl;
	
	//	Call main entry function
	beginWriting(use_mutexes);
	
	cout << "main() - End!" << endl;

	
	
	return 0;
}


