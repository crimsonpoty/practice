
/**
 * Sudoku.h
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#ifndef _SUDOKU_H_
#define _SUDOKU_H_

#define __USE_MINGW_ANSI_STDIO 0	// For solve error "multiple definition of 'vsnprintf'"

#include <vector>
#include <string>

using std::vector;
using std::string;

class CSudoku
{
public:
	CSudoku();

	void Input();
	void Input(string InputStr);
	bool Solve();
	string GetSolvedSudoku();
	static int mSolveCount;		//for test

private:
	void Sync();
	void Sync(int Row, int Col);
	void Prepare();
	void Solve_Normal();
	void Insert(int Row, int Col, int Target);
	void RemoveSub(int Row, int Col, int Target);
	bool Inspection();
	bool ValidCheck(vector<int> & vRef);
	void Print();
	void RearrangeSub(int Row, int Col);

private:
	vector<vector<int> > mCell;
	vector<vector<int> > mHorizontal;
	vector<vector<int> > mVertical;
	vector<vector<vector<int> > > mSub;
	string mStrSolved;
};

struct SNumber
{
	int number;
	int count;
};

struct SSubNumber
{
	int number;
	vector<int> subPosition;
};

#endif /* _SUDOKU_H_ */
