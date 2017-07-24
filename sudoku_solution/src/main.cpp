//============================================================================
// Name        : main.cpp
// Author      : crimson_88@naver.com
// Version     : V0.2
// Copyright   : 
// Description : Sudoku Solution in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "Sudoku.h"

using namespace std;


int main() {
	CSudoku _sudoku;

	_sudoku.Input();

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

	if(!_sudoku.Solve()) {
		cout << "Do not solve yet..." << endl;
	}

	return 0;
}
