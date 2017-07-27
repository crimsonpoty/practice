/**
 * Sudoku.cpp
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#include "Sudoku.h"

#include <string.h>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <functional> 	// less
#include <iostream>
#include <iterator>
#include <numeric>      // std::accumulate

//#define DEBUG_MODE

using namespace std;

///> TODO: ĸ��ȭ�� �� �ְ� �ڵ� ����

int CSudoku::mSolveCount = 0;

CSudoku::CSudoku()
{
	vector<vector<int> > initVector (9, vector<int>(9, 0));
	mHorizontal = initVector;
	mVertical = initVector;
	mCell = initVector;

	mSub.assign(9, vector<vector<int> >(9));
}

void CSudoku::Input()
{
#ifndef DEBUG_MODE
	char _input[256];
	for(int i = 0; i < 9; i++) {
		// ���κ��� �Է¹ޱ�
		memset(_input, 0x00, sizeof(_input));
		cin.getline(_input, sizeof(_input));

		// ������ �������� ���ڿ� �и�
		stringstream ss(_input);
		string buffer;
		vector<string> tokens;

		while(ss >> buffer) {
			tokens.push_back(buffer);
		}

		for(int j = 0; j < 9; j++) {
			mHorizontal[i][j] = atoi(tokens[j].c_str());
		}
	}
#else
//	string _input("530841620900502800000069001000200010752100986140790053000083042401627395003005060");	// normal - 16
	string _input("080000001000840300200010067020057009017000080090060075630000700070586000100009600");	// hard - 315
//	string _input("000060040380000920100090506460050090090000000700180004608905702510007408207300000");

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			mHorizontal[i][j] = _input[i*9 + j] - 48;
		}
	}
#endif

	Sync(string("Horizontal"));
}

void CSudoku::Sync(string Mode)
{
	if(0 == Mode.compare("Horizontal")) {
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				// �� ��Ҹ� �� ��ҿ� ����
				mVertical[j][i] = mHorizontal[i][j];

				// �� ��Ҹ� �� ��ҿ� ����
				int x = (i / 3) * 3 + (j / 3);
				int y = (i % 3) * 3 + (j % 3);
				mCell[x][y] = mHorizontal[i][j];
			}
		}
	}
	else if(0 == Mode.compare("Vertical")) {
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				// �� ��Ҹ� �� ��ҿ� ����
				mHorizontal[j][i] = mVertical[i][j];

				// �� ��Ҹ� �� ��ҿ� ����
				int x = (j / 3) * 3 + (i / 3);
				int y = (j % 3) * 3 + (i % 3);
				mCell[x][y] = mVertical[i][j];
			}
		}
	}
	else if(0 == Mode.compare("Cell")) {
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				// �� ��Ҹ� �� ��ҿ� ����
				int x = (i / 3) * 3 + (j / 3);
				int y = (i % 3) * 3 + (j % 3);
				mHorizontal[x][y] = mCell[i][j];

				// �� ��Ҹ� �� ��ҿ� ����
				mVertical[y][x] = mCell[i][j];
			}
		}
	}

	///> �ĺ��� ����
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			RearrangeSub(i, j);
		}
	}
}

void CSudoku::Sync(string Mode, int Row, int Col)
{
	if(0 == Mode.compare("Horizontal")) {
		mVertical[Col][Row] = mHorizontal[Row][Col];

		int x = (Row / 3) * 3 + (Col / 3);
		int y = (Row % 3) * 3 + (Col % 3);
		mCell[x][y] = mHorizontal[Row][Col];
	}
	else if(0 == Mode.compare("Vertical")) {
		mHorizontal[Col][Row] = mVertical[Row][Col];

		int x = (Col / 3) * 3 + (Row / 3);
		int y = (Col % 3) * 3 + (Row % 3);
		mCell[x][y] = mVertical[Row][Col];
	}
	else if(0 == Mode.compare("Cell")) {
		int x = (Row / 3) * 3 + (Col / 3);
		int y = (Row % 3) * 3 + (Col % 3);
		mHorizontal[x][y] = mCell[Row][Col];
		mVertical[y][x] = mCell[Row][Col];
	}

	///> �ĺ��� ����
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			RearrangeSub(i, j);
		}
	}
}

void CSudoku::Prepare(string Mode)
{
	/**
	 * mSub�� ���ó�� �Ʒ��� ���� ���·� ���
	 * 0,0 0,1 0,2 | 0,3 0,4 0,5 | 0,6 0,7 0,8
	 * 1,0 1,1 1,2 | 1,3 1,4 1,5 | 1,6 1,7 1,8
	 * 2,0 2,1 2,2 | 2,3 2,4 2,5 | 2,6 2,7 2,8
	 *
	 * 3,0 3,1 3,2 | 3,3 3,4 3,5 | 3,6 3,7 3,8
	 * 4,0 4,1 4,2 | 4,3 4,4 4,5 | 4,6 4,7 4,8
	 * 5,0 5,1 5,2 | 5,3 5,4 5,5 | 5,6 5,7 5,8
	 *
	 * 6,0 6,1 6,2 | 6,3 6,4 6,5 | 6,6 6,7 6,8
	 * 7,0 7,1 7,2 | 7,3 7,4 7,5 | 7,6 7,7 7,8
	 * 8,0 8,1 8,2 | 8,3 8,4 8,5 | 8,6 8,7 8,8
	 */

	if(0 == Mode.compare("normal")) {
		///> �� ���� �ĺ��׸� ä���
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				for(int k = 1; k <= 9; k++) {
					if(IsFillSub(i, j, k)) {
						mSub[i][j].push_back(k);
					}
				}
			}
		}
	}

	if(0 == Mode.compare("hard")) {
		///> �ĺ��׸� �ʱ�ȭ
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				mSub[i][j].clear();
			}
		}
	}
}

bool CSudoku::ValidCheck(vector<int> & vRef)
{
	///> �ߺ� ���� ����
	vector<int> v(vRef);
	std::sort(v.begin(), v.end(), std::less<int>());
	auto pos = std::unique(v.begin(), v.end());
	v.erase(pos, v.end());

	///> ��ȿ�� �˻�
	if(9 != v.size() || 45 != accumulate(v.begin(), v.end(), 0)) {
		return false;
	}

	return true;
}

bool CSudoku::IsFillSub(int Row, int Col, int Target)
{
	bool check = false;

	///> ä������ ���� �˻�
	if(0 == mHorizontal[Row][Col])  {
		int x = (Row / 3) * 3;
		int y = Col / 3;

		if(find(mCell[x + y].begin(), mCell[x + y].end(), Target)		== mCell[x + y].end()
				&& find(mHorizontal[Row].begin(), mHorizontal[Row].end(), Target)	== mHorizontal[Row].end()
				&& find(mVertical[Col].begin(), mVertical[Col].end(), Target)		== mVertical[Col].end()) {
			check = true;
		}
	}

	return check;
}

void CSudoku::InsertInHorAndRemoveSub(int Row, int Col, int Target)
{
	mHorizontal[Row][Col] = Target;
	mSub[Row][Col].clear();
	Sync(string("Horizontal"), Row, Col);

#ifdef DEBUG_MODE
	cout << "[InsertInHorAndRemoveSub(Hor)]: " << endl; for(int i = 0; i < 9; i++) { for(int j = 0; j < 9; j++) { cout << mHorizontal[i][j] << " "; } cout <<endl; } cout <<endl; // for test
#endif

	///> ä���� ���� �ĺ������� ���ִ� �κ�
	int cell = (Row / 3) * 3 + (Col / 3);
	int x = (cell / 3) * 3;
	int y = (cell % 3) * 3;

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			///> ��
			auto it = find(mSub[x + i][y + j].begin(), mSub[x + i][y + j].end(), Target);
			if(it != mSub[x + i][y + j].end()) {
				mSub[x + i][y + j].erase(it);
#ifdef DEBUG_MODE
				cout << "[InsertInHorAndRemoveSub(RemCel)]: " << Target << " is erased - mSub[" << x + i << "][" << y + j << "]: "; for(auto& a: mSub[x + i][y + j]) { cout << a << " "; } cout << endl; // for test
#endif
			}
		}
	}

	for(int k = 0; k < 9; k++) {
		///> ����
		auto it = find(mSub[Row][k].begin(), mSub[Row][k].end(), Target);
		if(it != mSub[Row][k].end()) {
			mSub[Row][k].erase(it);
#ifdef DEBUG_MODE
			cout << "[InsertInHorAndRemoveSub(RemHor)]: " << Target << " is erased - mSub[" << Row << "][" << k << "]: "; for(auto& a: mSub[Row][k]) { cout << a << " "; } cout << endl; // for test
#endif
		}

		///> ����
		it = find(mSub[k][Col].begin(), mSub[k][Col].end(), Target);
		if(it != mSub[k][Col].end()) {
			mSub[k][Col].erase(it);
#ifdef DEBUG_MODE
			cout << "[InsertInHorAndRemoveSub(RemVer)]: " << Target << " is erased - mSub[" << k << "][" << Col << "]: "; for(auto& a: mSub[k][Col]) { cout << a << " "; } cout << endl; // for test
#endif
		}
	}
}

void CSudoku::InsertSub(int Row, int Col, int Target)
{
	if(Target) {
		auto it = find(mSub[Row][Col].begin(), mSub[Row][Col].end(), Target);
		if(it != mSub[Row][Col].end()) {
			return;
		}
		mSub[Row][Col].push_back(Target);
		RearrangeSub(Row, Col);
	}
}

void CSudoku::RearrangeSub(int Row, int Col)
{
	if(mSub[Row][Col].size() > 1) {
		std::sort(mSub[Row][Col].begin(), mSub[Row][Col].end(), std::less<int>());
		auto pos = std::unique(mSub[Row][Col].begin(), mSub[Row][Col].end());
		mSub[Row][Col].erase(pos, mSub[Row][Col].end());
	}
}


bool CSudoku::Solve()
{
	///> Normal �ܰ� �ذ�
	Prepare(string("normal"));

	Solve_Normal();		// for test

	if(FinalInspection()) {
		cout << "Solved in Normal / Count: " << mSolveCount << endl;
		Print();
		return true;
	}
	else {
		cout << "Not Solved in Normal / Count: " << mSolveCount << endl;
	}

	mSolveCount = 0;
	Prepare(string("hard"));
	Solve_Hard();

	if(FinalInspection()) {
		cout << "Solved in Hard / Count: " << mSolveCount << endl;
		Print();
		return true;
	}
	else {
		cout << "Not Solved in Hard / Count: " << mSolveCount << endl;
	}

	///> TODO: ���� Extreme�� �ذ����� ����
	///> ���: Hard�� ��Ǫ�� ��Ȳ���� ��� ���� ���¸� ����ϸ鼭 ���� ���ΰ�?
	///> 	   �ٸ� Ǯ�̹��� ������ ���ΰ�?

	return false;
}


void CSudoku::Solve_Normal()
{
	int count = 0;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(1 == mSub[i][j].size()) {
				int value = mSub[i][j][0];

				// �ĺ����� 1���� ��, ���� ����
				InsertInHorAndRemoveSub(i, j, value);
				count++;
			}
		}
	}

	mSolveCount++;

	///> �ذ�� ������ ���
	if(count) Solve_Normal();
}


void CSudoku::Solve_Hard()
{
	int count = 0;

	///> �࿡�� 1�� ���� �κ� �����ؼ� ���� ä���
	for(int i = 0; i < 9; i++) {
		if(1 == std::count(mHorizontal[i].begin(), mHorizontal[i].end(), 0)) {
			int col = FillRemainOne(mHorizontal[i]);
			if(col) {
				mSub[i][col].clear();
				count++;
			}
		}
	}
	Sync(string("Horizontal"));

	///> ������ 1�� ���� �κ� �����ؼ� ���� ä���
	for(int i = 0; i < 9; i++) {
		if(1 == std::count(mVertical[i].begin(), mVertical[i].end(), 0)) {
			int row = FillRemainOne(mVertical[i]);
			if(row) {
				mSub[row][i].clear();
				count++;
			}
		}
	}
	Sync(string("Vertical"));

	///> ������ 1�� ���� �κ� �����ؼ� ���� ä���
	for(int i = 0; i < 9; i++) {
		if(1 == std::count(mCell[i].begin(), mCell[i].end(), 0)) {
			int ty = FillRemainOne(mCell[i]);
			if(ty) {
				int x = (i / 3) * 3 + (ty / 3);
				int y = ty - (x % 3) * 3 + (i % 3) * 3;
				mSub[x][y].clear();
				count++;
			}
		}
	}
	Sync(string("Cell"));

	///> ���� ���� & ����
	SNumber _s = {0, 0};
	vector<SNumber> vNum;

	for(int i = 1; i <= 9; i++) {
		_s.number = i;
		vNum.push_back(_s);
	}

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(0 != mHorizontal[i][j]) {
				const int num = mHorizontal[i][j];
				auto it = find_if(vNum.begin(), vNum.end(), [num] (const SNumber & s) { return num == s.number; });
				(*it).count++;
			}
		}
	}

	std::sort(vNum.begin(), vNum.end(), [] (const SNumber & l, const SNumber & r) { return l.count > r.count; });
//	for test
#ifdef DEBUG_MODE
  	for_each(vNum.begin(), vNum.end(), [] (const SNumber s) {
			cout << "number: " << s.number << " / count: " << s.count <<endl;
	});
#endif

	/*
	 * 1. ���� ���� ����, ���� ������ x�� �ִ��� Ȯ��
	 * 2. x�� �ִٸ� �� ��° �� �Ǵ� ������ �ľ�(��,���� ���� �˻�)
	 * 3. �ĺ������� �Ұ�
	 *  1) �ϳ��� ��
	 *   (1) ���� ���� �Ǵ� -> ��ġ ��� -> �ݿ�
	 *   (2) ���� ���� �Ǵ� -> ��ġ ��� -> �ݿ�
	 *  2) �� ���� ��
	 *   (1) ���� ���� �Ǵ� -> ��ġ ��� -> �ݿ�
	 *   (2) ���� ���� �Ǵ� -> ��ġ ��� -> �ݿ�
	 * 4. ���
	 *
	 * �� ������ UML ����
	 *
	 * */

	///> ���� ���ڸ����� �� ���� ���� ä���, �ĺ�(2��) ä��� -> �ҰŹ����� ����
	bool aSub[9];

	for(auto it = vNum.begin(); it != vNum.end(); it++) {
		///> �� ä�� ���ڴ� ����
		if((*it).count < 9) {
			for(int i = 0; i < 9; i++) {
				memset(aSub, true, sizeof(aSub));
				///> �ش� ���� ã������ ���ڰ� �ִٸ� �ǳʶٱ�
				auto itB = find(mCell[i].begin(), mCell[i].end(), (*it).number);
				if(itB != mCell[i].end()) continue;

				///> �ش� ���� ���ڰ� ä���ִ� �κ� �Ұ�
				for(int j = 0; j < 9; j++) {
					if(0 != mCell[i][j]) {
						aSub[j] = false;
					}
				}
#ifdef DEBUG_MODE
				cout << "[Fil] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif

				/**> ���� �κ� �˻� �� �Ұ� */
				int x2_1 = -1;
				int x2_2 = -1;

				for(int j = 0; j < 3; j++) {

					///> �˻�κ�: �� 012 - ���� 012 / �� 345 - ���� 345 / �� 678 - ���� 678
					int row = (i / 3) * 3 + j;
#ifdef DEBUG_MODE
					cout << "[DEBUG - Hor] >> num(" << (*it).number << ") count(" << (*it).count << ") i(" << i << ") j(" << j << ") row(" << row << ")" <<endl;	// for test
#endif
					auto itH = find(mHorizontal[row].begin(), mHorizontal[row].end(), (*it).number);
					if(itH != mHorizontal[row].end()) {

						///> �Ұźκ�: ���� 0 - �ĺ� 012, ���� 1 - �ĺ� 345, ���� 2 - �ĺ� 678
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
						}
#ifdef DEBUG_MODE
						cout << "[Hor] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					}

					/**> �ĺ����� �̿��� �˻� */

					///> �ĺ��� ã��
					int subCount = 0;
					int x1 = 0;
					int x2 = 0;
					int x3 = 0;

					for(int k = 0; k < 9; k++) {

						///> ���� �� �ǳʶٱ�
						if((i % 3) == (k / 3)) continue;

						if(!mSub[row][k].empty()) {
							auto itS = find(mSub[row][k].begin(), mSub[row][k].end(), (*it).number);

							if(itS != mSub[row][k].end()) {
								subCount++;

								if(1 == subCount) x1 = k;
								else if(2 == subCount) {
									x2 = k;

									///> 2�� �� �ĺ��� ó�� ���� �κ�
									if(x2_1 < 0)	x2_1 = j;
									else 			x2_2 = j;
								}
								else if(3 == subCount) x3 = k;
							}
						}
					}

					///> �ĺ����� �� ���ο� 2�� �̻��̴��� �ҰŹ��� ���� �� �Ǵ� ��� ����
					if(2 == subCount
							&& x1 != x2
							&& (x1 / 3) == (x2 / 3)) {

						///> �ĺ����ִٸ� �ش� ���κκ� �Ұ�
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
#ifdef DEBUG_MODE
							cout << "[Hor - Sub2] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
						}
					}

					if(3 == subCount
							&& x1 != x2 && x1 != x3 && x2 != x3
							&& (x1 / 3) == (x2 / 3) && (x1 / 3) == (x3 / 3)&& (x2 / 3) == (x3 / 3)) {

						///> �ĺ����ִٸ� �ش� ���κκ� �Ұ�
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
#ifdef DEBUG_MODE
							cout << "[Hor - Sub3] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
						}
					}
				}

				///> 2�� ���� �ĺ������� ���� �κ��� �ҰŵǴ� ���
				if(x2_1 >= 0 && x2_2 >= 0) {
					for(int k = 0; k < 3; k++) {
						aSub[x2_1 * 3 + k] = false;
						aSub[x2_2 * 3 + k] = false;
#ifdef DEBUG_MODE
						cout << "[Hor - Sub22] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					}
				}


				/**> ���� �κ� �˻� �� �Ұ� */
				int y2_1 = -1;
				int y2_2 = -1;

				for(int j = 0; j < 3; j++) {

					///> �˻�κ�: �� 036 - ���� 012 / �� 147 - ���� 345 / �� 258 - ���� 678
					int col = (i % 3) * 3 + j;
#ifdef DEBUG_MODE
					cout << "[DEBUG - Ver] >> num(" << (*it).number << ") count(" << (*it).count << ") i(" << i << ") j(" << j << ") col(" << col << ")" <<endl;	// for test
#endif
					auto itV = find(mVertical[col].begin(), mVertical[col].end(), (*it).number);
					if(itV != mVertical[col].end()) {

						///> �Ұźκ�: ���� 0 - �ĺ� 036, ���� 1 - �ĺ� 147, ���� 2 - �ĺ� 258
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
#ifdef DEBUG_MODE
						cout << "[Ver] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					}

					/**> �ĺ����� �̿��� �˻� */

					///> �ĺ��� ã��
					int subCount = 0;
					int y1 = 0;
					int y2 = 0;
					int y3 = 0;

					for(int k = 0; k < 9; k++) {

						///> ���� �� �ǳʶٱ�
						if((i / 3) == (k / 3)) continue;

						if(!mSub[k][col].empty()) {
							auto itS = find(mSub[k][col].begin(), mSub[k][col].end(), (*it).number);

							if(itS != mSub[k][col].end()) {
								subCount++;

								if(1 == subCount) y1 = k;
								else if(2 == subCount) {
									y2 = k;

									///> 2�� �� �ĺ��� ó�� ���� �κ�
									if(y2_1 < 0)	y2_1 = j;
									else 			y2_2 = j;
								}
								else if(3 == subCount) y3 = k;
							}
						}

					}

					///> �ĺ����� �� ���ο� 2�� �̻��̴��� �ҰŹ��� ���� �� �Ǵ� ��� ����
					if(2 == subCount
							&& y1 != y2
							&& (y1 / 3) == (y2 / 3)) {

						///> �ĺ����ִٸ� �ش� ���κκ� �Ұ�
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
#ifdef DEBUG_MODE
						cout << "[Ver - Sub2] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					}

					if(3 == subCount
							&& y1 != y2 && y1 != y3 && y2 != y3
							&& (y1 / 3) == (y2 / 3) && (y1 / 3) == (y3 / 3)&& (y2 / 3) == (y3 / 3)) {

						///> �ĺ����ִٸ� �ش� ���κκ� �Ұ�
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
#ifdef DEBUG_MODE
						cout << "[Ver - Sub3] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					}
				}

				///> 2�� ���� �ĺ������� ���� �κ��� �ҰŵǴ� ���
				if(y2_1 >= 0 && y2_2 >= 0) {
					for(int k = 0; k < 9; k += 3) {
						aSub[y2_1 + k] = false;
						aSub[y2_2 + k] = false;
					}
#ifdef DEBUG_MODE
					cout << "[Ver - Sub22] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
				}

				/**> 1�� ���� �ĺ����� ������ ���ڽ��� �ִ� ��� */
				SSubNumber _s;
				vector<SSubNumber> vSubNum;

				for(int i = 1; i <= 9; i++) {
					_s.number = i;
					vSubNum.push_back(_s);
				}

				for(int j = 0; j < 9; j++) {
					///> ��ǥ��ȯ: ����� -> ����
					int x = (i / 3) * 3 + (j / 3);
					int y = (i % 3) * 3 + (j % 3);

					for(int k = 1; k <= 9; k++) {
						///> ���� ���� ���ڰ� ã������ �ĺ��� ���ڽְ� ���� ���� �ǳʶ�
						if(k == (*it).number) continue;

						if(!mSub[x][y].empty()) {
							auto itS = find(mSub[x][y].begin(), mSub[x][y].end(), k);
							if(itS != mSub[x][y].end()) {
								vSubNum[k - 1].subPosition.push_back(j);
//								auto itV = find_if(vSubNum.begin(), vSubNum.end(), [k] (const SSubNumber s) { return k == s.number; });
//								(*itV).subPosition.push_back(j);
							}
						}
					}
				}

				///> �񱳸� ���� �ĺ��� ��ġ�� ������������ ����
				for_each(vSubNum.begin(), vSubNum.end(), [] (SSubNumber & s) {
					if(s.subPosition.size() >= 2) {
						std::sort(s.subPosition.begin(), s.subPosition.end(), std::less<int>());
					}
				});

				///> �ߺ��Ǵ� ���� �ִٸ� �ش� �ĺ��� �Ұ�
				for(int j = 0; j < 9; j++) {
					for(int k = j+1; k < 9; k++) {
#ifdef DEBUG_MODE
						cout << ">>>>>>>>>>>>>>>>>>> j,k = " << j << "," << k << endl;
#endif
						if(vSubNum[j].subPosition.size() >= 2
								&& vSubNum[j].subPosition == vSubNum[k].subPosition) {
							aSub[j] = false;
							aSub[k] = false;
#ifdef DEBUG_MODE
							cout << "[Dup] aSub: "; for(auto& x: aSub) { cout << x << " "; }; cout << " / j(" << j << ") k(" << k << ") subPosition: "; for(auto& x: vSubNum[j].subPosition) cout << x << " "; cout <<endl; // for test
#endif
						}
					}
				}


				///> �ĺ��� ����
				int subCount = std::count(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true);

				///> �� ä���
				if(1 == subCount) {
					/**> i(�� ����)�� aSub�� true��ġ�� ��ǥ�� ����Ͽ� mHorizontal�� ���� */

					///> aSub���� �ӽ� ��ǥ ���
					int ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;

					///> �ӽ� ��ǥ�� �࿭ ��ǥ ����ϰ� ä���
					int x = (i / 3) * 3 + (ty / 3);
					int y = ty - (x % 3) * 3 + (i % 3) * 3;
					InsertInHorAndRemoveSub(x, y, (*it).number);
					count++;
				}

				///> �ĺ��� ä���
				else if(2 == subCount) {
#ifdef DEBUG_MODE
					cout << "[2 == subCount] aSub: "; for(auto& x: aSub) { cout << x << " "; } cout <<endl; // for test
#endif
					/**> 1. i(�� ����)�� aSub�� true��ġ�� ��ǥ�� ����Ͽ� mSub�� ���� */

					///> aSub���� ù��° �ӽ� ��ǥ ���
					int ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;
					aSub[ty] = false;

					///> �ӽ� ����ǥ�� �࿭ ��ǥ ���
					int x = (i / 3) * 3 + (ty / 3);
					int y = ty - (x % 3) * 3 + (i % 3) * 3;

					///> mSub ä���
					InsertSub(x, y, (*it).number);
#ifdef DEBUG_MODE
					cout << "[mSub][" << x << "][" << y << "]: "; for(auto& a: mSub[x][y]) { cout << a << " "; } cout <<endl; // for test
#endif
					count++;

					/**> 2. i(�� ����)�� aSub�� true��ġ�� ��ǥ�� ����Ͽ� mSub�� ���� */

					///> aSub���� �ι�° �ӽ� ��ǥ ���
					ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;
					aSub[ty] = false;

					///> �ӽ� ����ǥ�� �࿭ ��ǥ ���
					x = (i / 3) * 3 + (ty / 3);
					y = ty - (x % 3) * 3 + (i % 3) * 3;

					///> mSub ä���
					InsertSub(x, y, (*it).number);
#ifdef DEBUG_MODE
					cout << "[mSub][" << x << "][" << y << "]: "; for(auto& a: mSub[x][y]) { cout << a << " "; } cout <<endl; // for test
#endif
					count++;
				}
			}
		}
	}

	mSolveCount++;
#ifdef DEBUG_MODE
	cout << "[DEBUG] ->> CSudoku::Solve_Hard: mSolveCount - " << mSolveCount << " count - " << count << endl;
#endif
	///> �ذ�� ������ ���
	if(count) Solve_Hard();
}

int CSudoku::FillRemainOne(vector<int> & v)
{
	if(1 != std::count(v.begin(), v.end(), 0))
		return 0;

	///> ��, ��, ������ 1�� ���� �κ� �����ؼ� ���� ä���
	for(int i = 1; i <= 9; i++) {
		if(find(v.begin(), v.end(), i) == v.end()) {
			auto it = find(v.begin(), v.end(), 0);
			int pos = std::distance(v.begin(), it);

			v.insert(v.erase(it), i);
			return pos;
		}
	}

	return 0;
}

bool CSudoku::FinalInspection()
{
	for(int i = 0; i < 9; i++) {
		if(!ValidCheck(mCell[i])) {
#ifdef DEBUG_MODE
			cout << "ValidCheck(Cell)-i(" << i << ") is false" << endl;
#endif
			return false;
		}
		if(!ValidCheck(mHorizontal[i])) {
#ifdef DEBUG_MODE
			cout << "ValidCheck(Horizontal)-i(" << i << ") is false" << endl;
#endif
			return false;
		}
		if(!ValidCheck(mVertical[i])) {
#ifdef DEBUG_MODE
			cout << "ValidCheck(Vertical)-i(" << i << ") is false" << endl;
#endif
			return false;
		}

		for(int j = 0; j < 9; j++) {
			if(!mSub[i][j].empty()) {
#ifdef DEBUG_MODE
				cout << "ValidCheck(Sub)-i(" << i << ") j(" << j << ") is false" << endl; for(auto& x: mSub[i][j]) cout << x << " "; cout << endl; // for test
#endif
				return false;
			}
		}
	}

	return true;
}

void CSudoku::Print()
{
#ifndef DEBUG_MODE
	cout << endl << "Output: " << endl;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			cout << mHorizontal[i][j] << " ";
			if(0 == (j + 1) % 3) cout << " ";
		}
		if(0 == (i + 1) % 3) cout << endl;
		cout << endl;
	}
	cout << endl;
#else
	cout << "Rectangle: " << endl;
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			cout << mCell[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << "Horizontal: " << endl;
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			cout << mHorizontal[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	cout << "Vertical: " << endl;
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			cout << mVertical[j][i] << " ";
		}
		cout << endl;
	}
	cout << endl;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(!mSub[i][j].empty()) {
				cout << "mSub[" << i << "][" << j << "]: ";
				for(auto& x: mSub[i][j]) {
					cout << x << " ";
				}
				cout << endl;
			}
		}
	}
#endif
}


