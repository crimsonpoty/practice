
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

protected:
	bool FinalInspection();
//	bool Inspection(int Row, int Col);
	void InsertInHorAndRemoveSub(int Row, int Col, int Target);
	void InsertSub(int Row, int Col, int Target = 0);

private:
	void Print();
	void Sync(string Mode);
	void Sync(string Mode, int Row, int Col);
	void Prepare(string Mode);
	void Solve_Normal();
	void Solve_Hard();
	void RearrangeSub(int Row, int Col);
	int FillRemainOne(vector<int> & v);
	bool ValidCheck(vector<int> & vRef);
	bool IsFillSub(int Row, int Col, int Target);

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
