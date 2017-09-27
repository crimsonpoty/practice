///============================================================================
/// Name        : main.cpp
/// Author      : crimson_88@naver.com
/// Version     : V0.2
/// Copyright   :
/// Description : Sudoku Solution in C++, Ansi-style
///============================================================================

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Sudoku.h"


using namespace std;


int main() {
///> [Stack]
/*
	CSudoku _sudoku;

	_sudoku.Input();

	if(!_sudoku.Solve()) {
		cout << "Do not solve yet..." << endl;
	}
*/


///> [Heap]
	CSudoku *_sudoku;

	try {
		_sudoku = new CSudoku;
	}
	catch(std::bad_alloc &e) {
		cout << e.what() << endl;
	}

	_sudoku->Input();

	if(_sudoku->Solve()) {
		if(_sudoku->Inspection()) {
			_sudoku->Print();
		}
		else {
			cout << "Inspection Fail" << endl;
		}
	}
	else {
		cout << "Can not solved..." << endl;
	}

	delete _sudoku;

	return 0;
}
