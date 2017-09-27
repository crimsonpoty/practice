
/**
 * Sudoku.cpp
 *
 *  Created on: 2017. 3. 24.
 *      Author: crimson_88@naver.com
 */

#include "Sudoku.h"

#include <string.h>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <iostream>

#include "MyDebug.h"


using namespace std;


CSudoku::CSudoku()
{
	///> ������ ���� �ʱ�ȭ
	for(auto & i: mNum) {
		for(auto &j: i) {
			j = 0;
		}
	}

	///> ������ ���� �ʱ�ȭ
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			mRow[i][j] = &mNum[i][j];
			mCol[i][j] = &mNum[j][i];

			int x = (i / 3) * 3 + (j / 3);
			int y = (i % 3) * 3 + (j % 3);
			mCell[x][y] = &mNum[i][j];
		}
	}

	mStrSolved = "";
}


void CSudoku::Input()
{
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
			mNum[i][j] = atoi(tokens[j].c_str());
		}
	}
}


void CSudoku::Input(const string & InputStr)
{
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			mNum[i][j] = InputStr[i*9 + j] - 48;
		}
	}
}


string CSudoku::GetSolvedSudoku()
{
	if(Inspection()) {
		for(auto & x: mNum) {
			for(auto & y: x) {
				string tmpStr = to_string(y);
				mStrSolved.append(tmpStr);
			}
		}
	}
	else {
		DBG_WRN("Can not solved")
	}

	DBG_INFO("%s", mStrSolved.c_str())

	return mStrSolved;
}


bool CSudoku::Solve()
{
	SSudoku _s = findMinSubCount();	// ���� ����� ���� ���� ĭ Ž��

	if (_s.row > 9) {	// ���ذ�
		return false;
	}

	if (_s.row < 0) {	// �ذ� �Ϸ�
		return true;
	}

	for (int i = 1; i <= 9; i++) {
		if (!_s.num[i - 1]) {	// �Ұ����� ���� �ǳʶٱ�
			continue;
		}

		mNum[_s.row][_s.col] = i;
		DBG_TRACE("Insert(%d) in Row(%d) Col(%d)", i, _s.row, _s.col)

		if(Solve()) {	// ���
			return true;	// �ذ� �Ϸ�
		}
	}

	// ���ذ��̹Ƿ� ����ͷ� �ʱ�ȭ ��, �ٽ� Ǯ�� �õ�
	mNum[_s.row][_s.col] = 0;

	return false;
}


SSudoku CSudoku::findMinSubCount()
{
	SSudoku minSub = {-1, -1};
	int minCount = 10;

	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			if(mNum[i][j] > 0) {	// ä���� �ִ� ���� �ǳʶٱ�
				continue;
			}

			SSudoku temp = findPossibleSub(i, j);
			int subCount = sizeNums(temp);

			if(0 == subCount) {
				return SSudoku {10, 10};	// C++11
			}
			else if(1 == subCount) {	// 1���� ������ �ּҰ��̹Ƿ� �ٷ� ����
				return temp;
			}
			else if(minCount > subCount) {
				minCount = subCount;
				minSub = temp;
			}
		}
	}

	return minSub;
}


SSudoku CSudoku::findPossibleSub(int Row, int Col)
{
	SSudoku _s = {Row, Col, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	int cell = (Row / 3) * 3 + (Col / 3);    // �࿭ ��ǥ�� �� ��ȣ ���ϴ� ����

	for (int i = 0; i < 9; i++) {

		if (*mRow[Row][i] > 0) {	// �� Ž��
			_s.num[*mRow[Row][i] - 1] = false;
		}
		if (*mCol[Col][i] > 0) {	// �� Ž��
			_s.num[*mCol[Col][i] - 1] = false;
		}
		if (*mCell[cell][i] > 0) {	// �� Ž��
			_s.num[*mCell[cell][i] - 1] = false;
		}
	}

	return _s;
}


int CSudoku::sizeNums(SSudoku s)
{
	bool *n = s.num;
	int size = 0;
	for(int i = 0; i < 9; i++)
		size += n[i];

	return size;
}

bool CSudoku::Inspection()
{
	///> Ǯ�� �� �����˻�
	for(int i = 0; i < 9; i++) {
		int sumRow = 0;
		int sumCol = 0;
		int sumCell = 0;

		for(int j = 0; j < 9; j++) {
			sumRow += *mRow[i][j];
			sumCol += *mCol[i][j];
			sumCell += *mCell[i][j];
		}

		if(45 != sumRow) {
			DBG_WRN("Row[%d] is false", i)
			DBG_TRACE_D1(mRow[i])
			return false;
		}

		if(45 != sumCol) {
			DBG_WRN("Col[%d] is false", i)
			DBG_TRACE_D1(mCol[i])
			return false;
		}

		if(45 != sumCell) {
			DBG_WRN("Cell[%d] is false", i)
			DBG_TRACE_D1(mCell[i])
			return false;
		}
	}

	return true;
}


void CSudoku::Print()
{
	cout << "Output: " << endl;

	int count = 0;
	for(auto & i: mNum) {
		for(auto & j: i) {
			cout << j << " ";
			if(0 == ++count % 3) cout << " ";
		}
		if(0 == count % (3 * 9)) cout << endl;
		cout << endl;
	}
}

