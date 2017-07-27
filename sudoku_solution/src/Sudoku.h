/**
 * Sudoku.h
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <vector>
#include <string>

using std::vector;
using std::string;

class CSudoku
{
public:
	CSudoku();

	void Input();
	bool Solve();
	bool FinalInspection();
	void Print();
	static int mSolveCount;		//for test

private:
	void Sync(string Mode);
	void Sync(string Mode, int Row, int Col);
	void Prepare(string Mode);
	void Solve_Normal();
	void Solve_Hard();
	void InsertInHorAndRemoveSub(int Row, int Col, int Target);
	void InsertSub(int Row, int Col, int Target = 0);
	void RearrangeSub(int Row, int Col);
	int FillRemainOne(vector<int> & v);
	bool ValidCheck(vector<int> & vRef);
	bool IsFillSub(int Row, int Col, int Target);

private:
	vector<vector<int> > mCell;
	vector<vector<int> > mHorizontal;
	vector<vector<int> > mVertical;
	vector<vector<vector<int> > > mSub;
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

#endif /* SUDOKU_H_ */
