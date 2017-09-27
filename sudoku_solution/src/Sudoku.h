
/**
 * Sudoku.h
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#define __USE_MINGW_ANSI_STDIO 0	// For solve error "multiple definition of 'vsnprintf'"

#include <array>
#include <string>

using std::array;
using std::string;


// 참조: http://sunnyholic.com/81
// 참조사이트의 알고리즘 반영

struct SSudoku
{
	int row;
	int col;
	bool num[9];
};


class CSudoku
{
public:
	CSudoku();

	void Input();
	void Input(const string & InputStr);
	bool Solve();
	bool Inspection();

	string GetSolvedSudoku();

	void Print();

private:
	SSudoku findMinSubCount();
	SSudoku findPossibleSub(int Row, int Col);
	int sizeNums(SSudoku s);

private:
	typedef array<array<int, 9>, 9> SudokuArray;
	typedef array<array<int*, 9>, 9> pSudokuArray;

	SudokuArray mNum;
	pSudokuArray mRow;
	pSudokuArray mCol;
	pSudokuArray mCell;

	string mStrSolved;
};

#endif /* _SUDOKU_H_ */
