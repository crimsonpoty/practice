
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


///> TODO: [8][0] 에러나는 부분 디버깅
///> TODO: 알고리즘 수정
///> TODO: 여러번 테스트 후, TRACE 필요 없는 부분 제거

int CSudoku::mSolveCount = 0;

CSudoku::CSudoku()
{
	vector<vector<int> > initVector (9, vector<int>(9, 0));
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

	Sync(string("Horizontal"));
}

void CSudoku::Input(string InputStr)
{
	string _input(InputStr);

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			mHorizontal[i][j] = _input[i*9 + j] - 48;
		}
	}

	Sync(string("Horizontal"));
}

void CSudoku::Sync(string Mode)
{
	if(0 == Mode.compare("Horizontal")) {
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
	}
	else if(0 == Mode.compare("Vertical")) {
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				// 열 요소를 행 요소에 대입
				mHorizontal[j][i] = mVertical[i][j];

				// 열 요소를 셀 요소에 대입
				int x = (j / 3) * 3 + (i / 3);
				int y = (j % 3) * 3 + (i % 3);
				mCell[x][y] = mVertical[i][j];
			}
		}
	}
	else if(0 == Mode.compare("Cell")) {
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				// 셀 요소를 행 요소에 대입
				int x = (i / 3) * 3 + (j / 3);
				int y = (i % 3) * 3 + (j % 3);
				mHorizontal[x][y] = mCell[i][j];

				// 셀 요소를 열 요소에 대입
				mVertical[y][x] = mCell[i][j];
			}
		}
	}

	///> 후보군 정리
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

	///> 후보군 정리
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			RearrangeSub(i, j);
		}
	}
}

void CSudoku::Prepare(string Mode)
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

	if(0 == Mode.compare("normal")) {
		///> 각 셀에 후보항목 채우기
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
	else if(0 == Mode.compare("hard")) {
		///> 후보항목 초기화
		for(int i = 0; i < 9; i++) {
			for(int j = 0; j < 9; j++) {
				mSub[i][j].clear();
			}
		}
	}
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

bool CSudoku::IsFillSub(int Row, int Col, int Target)
{
	bool check = false;

	///> 채워야할 값만 검사
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

	DBG_TRACE("[Hor]: Target(%d) Row(%d) Col(%d)", Target, Row, Col)
	DBG_TRACE_D2(mHorizontal)

	///> 채워진 값을 후보군에서 없애는 부분
	int cell = (Row / 3) * 3 + (Col / 3);
	int x = (cell / 3) * 3;
	int y = (cell % 3) * 3;

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			///> 셀
			auto it = find(mSub[x + i][y + j].begin(), mSub[x + i][y + j].end(), Target);
			if(it != mSub[x + i][y + j].end()) {
				mSub[x + i][y + j].erase(it);

				DBG_TRACE("[RemCel]: %d is erased - mSub[%d][%d]", Target, x + i, y + i)
				DBG_TRACE_D1(mSub[x + i][y + j])
			}
		}
	}

	for(int k = 0; k < 9; k++) {
		///> 가로
		auto it = find(mSub[Row][k].begin(), mSub[Row][k].end(), Target);
		if(it != mSub[Row][k].end()) {
			mSub[Row][k].erase(it);
			DBG_TRACE("[RemVer]: %d is erased - mSub[%d][%d]", Target, Row, k)
			DBG_TRACE_D1(mSub[Row][k])
		}

		///> 세로
		it = find(mSub[k][Col].begin(), mSub[k][Col].end(), Target);
		if(it != mSub[k][Col].end()) {
			mSub[k][Col].erase(it);
			DBG_TRACE("[RemVer]: %d is erased - mSub[%d][%d]", Target, k, Col)
			DBG_TRACE_D1(mSub[k][Col])
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
	///> Normal 단계 해결
	Prepare(string("normal"));

	Solve_Normal();		// for test

	if(FinalInspection()) {
		Print();
		return true;
	}
	else {
		DBG_INFO("Not Solved in Normal / Count: %d", mSolveCount)
	}

	mSolveCount = 0;
	Prepare(string("hard"));
//	Solve_Hard();

	if(FinalInspection()) {
		Print();
		return true;
	}
	else {
		DBG_INFO("Not Solved in Hard / Count: %d", mSolveCount)
	}

	///> TODO: 아직 Extreme은 해결하지 못함
	///> 고민: Hard도 못푸는 상황에서 어떻게 현재 상태를 기억하면서 찍을 것인가?
	///> 	   다른 풀이법을 적용할 것인가?

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
				InsertInHorAndRemoveSub(i, j, value);
				count++;
			}
		}
	}

	mSolveCount++;

	///> 해결될 때까지 재귀
	if(count) Solve_Normal();
}


void CSudoku::Solve_Hard()
{
	int count = 0;

	///> 행에서 1개 남은 부분 조사해서 숫자 채우기
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
	DBG_TRACE(">>>>> mSub[8][0]:")
	DBG_TRACE_D1(mSub[8][0])

	///> 열에서 1개 남은 부분 조사해서 숫자 채우기
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
	DBG_TRACE(">>>>> mSub[8][0]:")
	DBG_TRACE_D1(mSub[8][0])

	///> 셀에서 1개 남은 부분 조사해서 숫자 채우기
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
	DBG_TRACE(">>>>> mSub[8][0]:")
	DBG_TRACE_D1(mSub[8][0])

	///> 숫자 세기 & 정렬
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
	DBG_TRACE(">>>>> mSub[8][0]:")
	DBG_TRACE_D1(mSub[8][0])

	std::sort(vNum.begin(), vNum.end(), [] (const SNumber & l, const SNumber & r) { return l.count > r.count; });


	/*
	 * 1. 현재 셀과 가로, 세로 셀에서 x가 있는지 확인
	 * 2. x가 있다면 몇 번째 행 또는 열인지 파악(행,열을 따로 검사)
	 * 3. 후보군으로 소거
	 *  1) 하나의 셀
	 *   (1) 가로 인접 판단 -> 위치 계산 -> 반영
	 *   (2) 세로 인접 판단 -> 위치 계산 -> 반영
	 *  2) 두 개의 셀
	 *   (1) 가로 인접 판단 -> 위치 계산 -> 반영
	 *   (2) 세로 인접 판단 -> 위치 계산 -> 반영
	 * 4. 재귀
	 *
	 * 상세 사항은 UML 참조
	 *
	 * */

	///> 단일 숫자만으로 셀 돌며 숫자 채우기, 후보(2개) 채우기 -> 소거법으로 진행
	bool aSub[9];

	for(auto it = vNum.begin(); it != vNum.end(); it++) {
		///> 다 채운 숫자는 제외
		if((*it).count < 9) {
			for(int i = 0; i < 9; i++) {
				memset(aSub, true, sizeof(aSub));
				///> 해당 셀에 찾으려는 숫자가 있다면 건너뛰기
				auto itB = find(mCell[i].begin(), mCell[i].end(), (*it).number);
				if(itB != mCell[i].end()) continue;

				///> 해당 셀에 숫자가 채워있는 부분 소거
				for(int j = 0; j < 9; j++) {
					if(0 != mCell[i][j]) {
						aSub[j] = false;
					}
				}
				DBG_TRACE("[Fil] aSub: ")
				DBG_TRACE_D1(aSub)

				/**> 가로 부분 검사 후 소거 */
				int x2_1 = -1;
				int x2_2 = -1;

				for(int j = 0; j < 3; j++) {

					///> 검사부분: 셀 012 - 가로 012 / 셀 345 - 가로 345 / 셀 678 - 가로 678
					int row = (i / 3) * 3 + j;
					DBG_TRACE("[Hor] num(%d) count(%d) i(%d) j(%d) row(%d)", (*it).number, (*it).count, i, j, row)

					auto itH = find(mHorizontal[row].begin(), mHorizontal[row].end(), (*it).number);
					if(itH != mHorizontal[row].end()) {

						///> 소거부분: 가로 0 - 후보 012, 가로 1 - 후보 345, 가로 2 - 후보 678
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
						}
						DBG_TRACE("[Hor - Sub] aSub: ")
						DBG_TRACE_D1(aSub)
					}

					/**> 후보군을 이용한 검사 */

					///> 후보군 찾기
					int subCount = 0;
					int x1 = 0;
					int x2 = 0;
					int x3 = 0;

					for(int k = 0; k < 9; k++) {

						///> 같은 셀 건너뛰기
						if((i % 3) == (k / 3)) continue;

						if(!mSub[row][k].empty()) {
							auto itS = find(mSub[row][k].begin(), mSub[row][k].end(), (*it).number);

							if(itS != mSub[row][k].end()) {
								subCount++;

								if(1 == subCount) x1 = k;
								else if(2 == subCount) {
									x2 = k;

									///> 2개 셀 후보군 처리 관련 부분
									if(x2_1 < 0)	x2_1 = j;
									else 			x2_2 = j;
								}
								else if(3 == subCount) x3 = k;
							}
						}
					}

					///> 후보군이 한 라인에 2개 이상이더라도 소거법이 적용 안 되는 경우 제외
					if(2 == subCount
							&& x1 != x2
							&& (x1 / 3) == (x2 / 3)) {

						///> 후보군있다면 해당 가로부분 소거
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
							DBG_TRACE("[Hor - Sub2] aSub: ")
							DBG_TRACE_D1(aSub)
						}
					}

					if(3 == subCount
							&& x1 != x2 && x1 != x3 && x2 != x3
							&& (x1 / 3) == (x2 / 3) && (x1 / 3) == (x3 / 3)&& (x2 / 3) == (x3 / 3)) {

						///> 후보군있다면 해당 가로부분 소거
						for(int k = 0; k < 3; k++) {
							int col = j * 3 + k;
							aSub[col] = false;
							DBG_TRACE("[Hor - Sub3] aSub: ")
							DBG_TRACE_D1(aSub)
						}
					}
				}

				///> 2개 셀의 후보군으로 가로 부분이 소거되는 경우
				if(x2_1 >= 0 && x2_2 >= 0) {
					for(int k = 0; k < 3; k++) {
						aSub[x2_1 * 3 + k] = false;
						aSub[x2_2 * 3 + k] = false;
						DBG_TRACE("[Hor - Sub22] aSub: ")
						DBG_TRACE_D1(aSub)
					}
				}


				/**> 세로 부분 검사 후 소거 */
				int y2_1 = -1;
				int y2_2 = -1;

				for(int j = 0; j < 3; j++) {

					///> 검사부분: 셀 036 - 세로 012 / 셀 147 - 세로 345 / 셀 258 - 세로 678
					int col = (i % 3) * 3 + j;
					DBG_TRACE("[Ver] num(%d) count(%d) i(%d) j(%d) col(%d)", (*it).number, (*it).count, i, j, col)

					auto itV = find(mVertical[col].begin(), mVertical[col].end(), (*it).number);
					if(itV != mVertical[col].end()) {

						///> 소거부분: 세로 0 - 후보 036, 세로 1 - 후보 147, 세로 2 - 후보 258
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
						DBG_TRACE("[Ver] aSub: ")
						DBG_TRACE_D1(aSub)
					}

					/**> 후보군을 이용한 검사 */

					///> 후보군 찾기
					int subCount = 0;
					int y1 = 0;
					int y2 = 0;
					int y3 = 0;

					for(int k = 0; k < 9; k++) {

						///> 같은 셀 건너뛰기
						if((i / 3) == (k / 3)) continue;

						if(!mSub[k][col].empty()) {
							auto itS = find(mSub[k][col].begin(), mSub[k][col].end(), (*it).number);

							if(itS != mSub[k][col].end()) {
								subCount++;

								if(1 == subCount) y1 = k;
								else if(2 == subCount) {
									y2 = k;

									///> 2개 셀 후보군 처리 관련 부분
									if(y2_1 < 0)	y2_1 = j;
									else 			y2_2 = j;
								}
								else if(3 == subCount) y3 = k;
							}
						}

					}

					///> 후보군이 한 라인에 2개 이상이더라도 소거법이 적용 안 되는 경우 제외
					if(2 == subCount
							&& y1 != y2
							&& (y1 / 3) == (y2 / 3)) {

						///> 후보군있다면 해당 세로부분 소거
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
						DBG_TRACE("[Ver - Sub2] aSub: ")
						DBG_TRACE_D1(aSub)
					}

					if(3 == subCount
							&& y1 != y2 && y1 != y3 && y2 != y3
							&& (y1 / 3) == (y2 / 3) && (y1 / 3) == (y3 / 3)&& (y2 / 3) == (y3 / 3)) {

						///> 후보군있다면 해당 세로부분 소거
						for(int k = 0; k < 9; k += 3) {
							aSub[j + k] = false;
						}
						DBG_TRACE("[Ver - Sub3] aSub: ")
						DBG_TRACE_D1(aSub)
					}
				}

				///> 2개 셀의 후보군으로 세로 부분이 소거되는 경우
				if(y2_1 >= 0 && y2_2 >= 0) {
					for(int k = 0; k < 9; k += 3) {
						aSub[y2_1 + k] = false;
						aSub[y2_2 + k] = false;
					}
					DBG_TRACE("[Ver - Sub22] aSub: ")
					DBG_TRACE_D1(aSub)
				}

				/**> 1개 셀의 후보군의 동일한 숫자쌍이 있는 경우 */
				SSubNumber _s;
				vector<SSubNumber> vSubNum;

				for(int i = 1; i <= 9; i++) {
					_s.number = i;
					vSubNum.push_back(_s);
				}

				for(int j = 0; j < 9; j++) {
					///> 좌표변환: 셀기반 -> 행기반
					int x = (i / 3) * 3 + (j / 3);
					int y = (i % 3) * 3 + (j % 3);

					for(int k = 1; k <= 9; k++) {
						///> 조사 중인 숫자가 찾으려는 후보군 숫자쌍과 같을 때는 건너뜀
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

				///> 비교를 위해 후보군 위치를 내림차순으로 정렬
				for_each(vSubNum.begin(), vSubNum.end(), [] (SSubNumber & s) {
					if(s.subPosition.size() >= 2) {
						std::sort(s.subPosition.begin(), s.subPosition.end(), std::less<int>());
					}
				});

				///> 중복되는 값이 있다면 해당 후보군 소거
				for(int j = 0; j < 9; j++) {
					for(int k = j+1; k < 9; k++) {
						if(vSubNum[j].subPosition.size() >= 2
								&& vSubNum[j].subPosition == vSubNum[k].subPosition) {
							aSub[j] = false;
							aSub[k] = false;
							DBG_TRACE("[Dup] aSub: ")
							DBG_TRACE_D1(aSub)
							DBG_TRACE("[Dup] j(%d) k(%d) subPosition: ", j, k)
							DBG_TRACE_D1(vSubNum[j].subPosition)
						}
					}
				}


				///> 후보군 세기
				int subCount = std::count(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true);

				///> 셀 채우기
				if(1 == subCount) {
					/**> i(셀 기준)와 aSub의 true위치로 좌표를 계산하여 mHorizontal에 대입 */

					///> aSub으로 임시 좌표 계산
					int ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;

					///> 임시 좌표로 행열 좌표 계산하고 채우기
					int x = (i / 3) * 3 + (ty / 3);
					int y = ty - (x % 3) * 3 + (i % 3) * 3;
					InsertInHorAndRemoveSub(x, y, (*it).number);
					count++;
					DBG_TRACE(">>>>> mSub[8][0]:")
					DBG_TRACE_D1(mSub[8][0])
				}

				///> 후보군 채우기
				else if(2 == subCount) {
					DBG_TRACE("[2 == subCount] aSub: ")
					DBG_TRACE_D1(aSub)

					/**> 1. i(셀 기준)와 aSub의 true위치로 좌표를 계산하여 mSub에 대입 */

					///> aSub으로 첫번째 임시 좌표 계산
					int ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;
					aSub[ty] = false;

					///> 임시 열좌표로 행열 좌표 계산
					int x = (i / 3) * 3 + (ty / 3);
					int y = ty - (x % 3) * 3 + (i % 3) * 3;

					///> mSub 채우기
					InsertSub(x, y, (*it).number);
					DBG_TRACE("mSub[%d][%d]: ", x, y)
					DBG_TRACE_D1(mSub[x][y])

					count++;
					DBG_TRACE(">>>>> mSub[8][0]:")
					DBG_TRACE_D1(mSub[8][0])

					/**> 2. i(셀 기준)와 aSub의 true위치로 좌표를 계산하여 mSub에 대입 */

					///> aSub으로 두번째 임시 좌표 계산
					ty = find(aSub, aSub + (sizeof(aSub)/sizeof(bool)), true) - aSub;
					aSub[ty] = false;

					///> 임시 열좌표로 행열 좌표 계산
					x = (i / 3) * 3 + (ty / 3);
					y = ty - (x % 3) * 3 + (i % 3) * 3;

					///> mSub 채우기
					InsertSub(x, y, (*it).number);
					DBG_TRACE("mSub[%d][%d]: ", x, y)
					DBG_TRACE_D1(mSub[x][y])

					count++;
					DBG_TRACE(">>>>> mSub[8][0]:")
					DBG_TRACE_D1(mSub[8][0])
				}
			}
		}
	}

	mSolveCount++;

	if(7 <= mSolveCount && 8 >= mSolveCount) {
		DBG_TRACE("mSolveCount(%d), mSub[8][0] ", mSolveCount)
		DBG_TRACE_D1(mSub[8][0])
		DBG_TRACE_D2(mHorizontal)
	}
	DBG_TRACE("[DEBUG] ->> CSudoku::Solve_Hard: mSolveCount - %d count - %d", mSolveCount, count)
	///> 해결될 때까지 재귀
	if(count) Solve_Hard();
}

int CSudoku::FillRemainOne(vector<int> & v)
{
	if(1 != std::count(v.begin(), v.end(), 0))
		return 0;

	///> 셀, 행, 열에서 1개 남은 부분 조사해서 숫자 채우기
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
			DBG_TRACE("ValidCheck(Cell)[%d] is false", i)
			return false;
		}
		if(!ValidCheck(mHorizontal[i])) {
			DBG_TRACE("ValidCheck(Horizontal)[%d] is false", i)
			return false;
		}
		if(!ValidCheck(mVertical[i])) {
			DBG_TRACE("ValidCheck(Vertical)[%d] is false", i)
			return false;
		}

		for(int j = 0; j < 9; j++) {
			if(!mSub[i][j].empty()) {
				DBG_TRACE("ValidCheck(Sub)[%d][%d] is false", i, j)
				DBG_TRACE_D1(mSub[i][j])
				return false;
			}
		}
	}

	return true;
}

/*bool CSudoku::Inspection(int Row, int Col)
{
	///> 행 -> 셀 좌표변환
	int x = (Row / 3) * 3 + (Col / 3);
	int y = (Row % 3) * 3 + (Col % 3);
	if(0 == mHorizontal[Row][Col] || 0 == mVertical[Col][Row] || 0 == mCell[x][y]) {
		DBG_WRN("Cell value is wrong, mHorizontal[%d][%d]: %d, mVertical[%d][%d]: %d, mCell[%d][%d]: %d", Row, Col, mHorizontal[Row][Col], Col, Row, mVertical[Col][Row], x, y, mCell[x][y])
		return false;
	}

	///> 셀 검사
	int cell = (Row / 3) * 3 + (Col / 3);
	x = (cell / 3) * 3;
	y = (cell % 3) * 3;

	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			///> 셀
			auto it = find(mCell[x + i][y + j].begin(), mCell[x + i][y + j].end(), Target);
			if(it != mCell[x + i][y + j].end()) {
				DBG_WRN("duplicated in Cell, mCell[%d][%d]: %d", x, y, mCell[x][y])
				return false;
			}
		}
	}

	for(int k = 0; k < 9; k++) {
		///> 가로 검사
		auto it = find(mHorizontal[Row][k].begin(), mHorizontal[Row][k].end(), Target);
		if(it != mHorizontal[Row][k].end()) {
			DBG_WRN("duplicated in Horizontal, mHorizontal[%d][%d]: %d", Row, Col, mHorizontal[x][y])
			return false;
		}

		///> 세로 검사
		it = find(mVertical[k][Col].begin(), mVertical[k][Col].end(), Target);
		if(it != mVertical[k][Col].end()) {
			DBG_WRN("duplicated in Vertical, mVertical[%d][%d]: %d", Col, Row, mVertical[x][y])
			return false;
		}
	}

	return true;
}*/

void CSudoku::Print()
{
#ifndef DEBUG
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
	DBG_INFO("Horizontal:")
	DBG_INFO_D2(mHorizontal)

	DBG_INFO("Vertical:")
	DBG_INFO_D2(mVertical)

	DBG_INFO("Rectangle:")
	DBG_INFO_D2(mCell)

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


string CSudoku::GetSolvedSudoku()
{
	if(FinalInspection()) {
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
