
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
#include <functional> 	// std::less
#include <iostream>
#include <iterator>
#include <numeric>      // std::accumulate

#include "MyDebug.h"


using namespace std;


///> TODO: 알고리즘 수정

int CSudoku::mSolveCount = 0;

CSudoku::CSudoku()
{
	vector<vector<int> > initVector(9, vector<int>(9, 0));
	mHorizontal = initVector;
	mVertical = initVector;
	mCell = initVector;

	mSub.assign(9, vector<vector<int> >(9));

	mStrSolved = "";
}


void CSudoku::Input()
{
	char _input[256];
	for(int i = 0; i < 9; i++) {
		// 라인별로 입력받기
		memset(_input, 0x00, sizeof(_input));
		cin.getline(_input, sizeof(_input));

		// 공백을 기준으로 문자열 분리
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

	Sync();
}


void CSudoku::Input(string InputStr)
{
	string _input(InputStr);

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			mHorizontal[i][j] = _input[i*9 + j] - 48;
		}
	}

	Sync();
}


string CSudoku::GetSolvedSudoku()
{
	if(Inspection()) {
		for(auto& x: mHorizontal) {
			for(auto& y: x) {
				string tmpStr = to_string(y);
				mStrSolved.append(tmpStr);
			}
		}
	}

	DBG_INFO("%s", mStrSolved.c_str())

	return mStrSolved;
}


void CSudoku::Sync()
{
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			// 행 요소를 열 요소에 대입
			mVertical[j][i] = mHorizontal[i][j];

			// 행 요소를 셀 요소에 대입
			int x = (i / 3) * 3 + (j / 3);
			int y = (i % 3) * 3 + (j % 3);
			mCell[x][y] = mHorizontal[i][j];
		}
	}

	///> 후보군 정리
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			RearrangeSub(i, j);
		}
	}
}


void CSudoku::Sync(int Row, int Col)
{
	mVertical[Col][Row] = mHorizontal[Row][Col];

	int x = (Row / 3) * 3 + (Col / 3);
	int y = (Row % 3) * 3 + (Col % 3);
	mCell[x][y] = mHorizontal[Row][Col];

	///> 후보군 정리
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			RearrangeSub(i, j);
		}
	}
}


void CSudoku::Prepare()
{
	/**
	 * mSub는 행렬처럼 아래와 같은 형태로 사용
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

	///> 각 셀에 후보항목 채우기
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(0 == mHorizontal[i][j]) {	///> 채워야할 값만 검사
				for(int k = 1; k <= 9; k++) {
					int cell = (i / 3) * 3 + (j / 3);
					if(find(mCell[cell].begin(), mCell[cell].end(), k)				== mCell[cell].end()
						&& find(mHorizontal[i].begin(), mHorizontal[i].end(), k)	== mHorizontal[i].end()
						&& find(mVertical[j].begin(), mVertical[j].end(), k)		== mVertical[j].end()) {

						mSub[i][j].push_back(k);
					}
				}
			}
		}
	}
}


bool CSudoku::Solve()
{
	///> Normal 단계 해결
	Prepare();

	Solve_Normal();		// for test

	if(Inspection()) {
		Print();
		return true;
	}
	else {
		DBG_WRN("Not Solved in Normal / Count: %d", mSolveCount)
	}

	return false;
}


void CSudoku::Solve_Normal()
{
	int count = 0;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(1 == mSub[i][j].size()) {
				int value = mSub[i][j][0];

				// 후보군이 1개일 때, 숫자 대입
				Insert(i, j, value);
				count++;
			}
		}
	}

	mSolveCount++;

	///> 해결될 때까지 재귀
	if(count) Solve_Normal();
}


void CSudoku::Insert(int Row, int Col, int Target)
{
	mHorizontal[Row][Col] = Target;
	mSub[Row][Col].clear();
	Sync(Row, Col);

	DBG_TRACE("[Hor]: Target(%d) Row(%d) Col(%d)", Target, Row, Col)
	DBG_TRACE_D2(mHorizontal)

	RemoveSub(Row, Col, Target);
}


void CSudoku::RemoveSub(int Row, int Col, int Target)
{
	///> 채워진 값을 후보군에서 없애는 부분
	int cell = (Row / 3) * 3 + (Col / 3);
	int x = (cell / 3) * 3;
	int y = (cell % 3) * 3;

	///> 셀
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			auto it = find(mSub[x + i][y + j].begin(), mSub[x + i][y + j].end(), Target);
			if(it != mSub[x + i][y + j].end()) {
				mSub[x + i][y + j].erase(it);
				DBG_TRACE("[RemCel]: %d is erased - mSub[%d][%d]", Target, x + i, y + i)
				DBG_TRACE_D1(mSub[x + i][y + j])
			}
		}
	}

	for(int i = 0; i < 9; i++) {
		///> 가로
		auto it = find(mSub[Row][i].begin(), mSub[Row][i].end(), Target);
		if(it != mSub[Row][i].end()) {
			mSub[Row][i].erase(it);
			DBG_TRACE("[RemVer]: %d is erased - mSub[%d][%d]", Target, Row, i)
			DBG_TRACE_D1(mSub[Row][i])
		}

		///> 세로
		it = find(mSub[i][Col].begin(), mSub[i][Col].end(), Target);
		if(it != mSub[i][Col].end()) {
			mSub[i][Col].erase(it);
			DBG_TRACE("[RemVer]: %d is erased - mSub[%d][%d]", Target, i, Col)
			DBG_TRACE_D1(mSub[i][Col])
		}
	}
}


bool CSudoku::Inspection()
{
	for(int i = 0; i < 9; i++) {
		if(!ValidCheck(mHorizontal[i])) {
			DBG_WRN("ValidCheck(Horizontal)[%d] is false", i)
			DBG_TRACE_D1(mHorizontal[i])
			return false;
		}

		if(!ValidCheck(mVertical[i])) {
			DBG_WRN("ValidCheck(Vertical)[%d] is false", i)
			DBG_TRACE_D1(mVertical[i])
			return false;
		}

		if(!ValidCheck(mCell[i])) {
			DBG_WRN("ValidCheck(Cell)[%d] is false", i)
			DBG_TRACE_D1(mCell[i])
			return false;
		}

		for(int j = 0; j < 9; j++) {
			if(!mSub[i][j].empty()) {
				DBG_WRN("ValidCheck(Sub)[%d][%d] is false", i, j)
				DBG_TRACE_D1(mSub[i][j])
				return false;
			}
		}
	}

	return true;
}


bool CSudoku::ValidCheck(vector<int> & vRef)
{
	///> 중복 원소 제거
	vector<int> v(vRef);
	std::sort(v.begin(), v.end(), std::less<int>());
	auto pos = std::unique(v.begin(), v.end());
	v.erase(pos, v.end());

	///> 유효성 검사
	if(9 != v.size() || 45 != accumulate(v.begin(), v.end(), 0)) {
		return false;
	}

	return true;
}


void CSudoku::Print()
{
	cout << endl << "Output: " << endl;

	int count = 0;
	for(auto & i: mHorizontal) {
		for(auto & j: i) {
			cout << j << " ";
			if(0 == ++count % 3) cout << " ";
		}
		if(0 == count % (3 * 9)) cout << endl;
		cout << endl;
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
