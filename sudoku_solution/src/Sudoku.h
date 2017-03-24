/*
 * Sudoku.h
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <vector>

using std::vector;

class CSudoku
{
public:
	CSudoku();

	void Input();
	void Prepare();
	int Solve();
	bool FinalInspection();
	void Print();

private:
	bool UniqueCheck(vector<int> & vRef);
	bool isFillSub(int Row, int Col, int Target);

private:
	vector<vector<int> > mBox;
	vector<vector<int> > mHorizontal;
	vector<vector<int> > mVertical;
	vector<vector<vector<int> > > mSub;
};

#endif /* SUDOKU_H_ */
