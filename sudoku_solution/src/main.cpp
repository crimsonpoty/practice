//============================================================================
// Name        : sudoku_solution.cpp
// Author      : crimson_88@naver.com
// Version     : V0.2
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Sudoku.h"

using namespace std;


int main() {
	CSudoku _sudoku;

	_sudoku.Input();
	_sudoku.Prepare();

#if 0 // test mSub
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			cout << "(" << i << "," << j << ")" << endl;
			for(int k = 0; k < _sudoku.mSub[i][j].size(); k++) {
				cout << _sudoku.mSub[i][j][k] << " ";
			}
			cout << endl;
		}
	}
#endif

	int count = 0;
	while(_sudoku.Solve()) {
		if(++count > 1000) break;
	};

	cout << endl << endl << "count: " << count << " / ";

	if(_sudoku.FinalInspection()) {
		_sudoku.Print();
	}
	else {
		cout << "Do not solve yet..." << endl;
	}

	return 0;
}
