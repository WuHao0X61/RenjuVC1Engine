#include <math.h>
#include <iostream>

#include "main.h"
#include "board.h"
#include "pattern.h"

using namespace std;

Hash::Hash() {
	srand((unsigned int)time(NULL));
	zobrist = rand64();
	zobrists = new ull[4096];
	for (int i = 0; i < 4096; i++) zobrists[i] = rand64();
	table = new HashItem[hashsize];
}

ull Hash::rand64() {
	return (((ull)rand())<<0)|(((ull)rand())<<15)|(((ull)rand())<<30)|(((ull)rand())<<45)|(((ull)rand())<<60);
}

void Hash::move(int m, int who) {
	zobrist ^= zobrists[(who<<10)|m];
}

void Hash::undo(int m, int who) {
	zobrist ^= zobrists[(who<<10)|m];
}

int Hash::read(float depth, int alpha, int beta, int who, int& move) {
	HashItem* i = &table[zobrist&hashmask];
	if (i->who != who || i->key != zobrist) return unknown;
	move = i->move;
	if (i->depth >= depth) {
		if (i->hashf == Alpha && i->value <= alpha) return alpha;
		if (i->hashf == Beta && i->value >= beta) return beta;
		if (i->hashf == Pv && i->value <= alpha) return alpha;
		if (i->hashf == Pv && i->value >= beta) return beta;
		if (i->hashf == Pv) return i->value;
	}
	return unknown;
}

void Hash::write(float depth, int value, int who, int move, int hashf) {
	HashItem* i = &table[zobrist&hashmask];
	if (i->who == who && i->key == zobrist && i->depth > depth) return;
	if (value == 10000 || value == -10000) return;
	i->key = zobrist;
	i->depth = depth;
	i->value = value;
	i->who = who;
	i->move = move;
	i->hashf = hashf;
}

Board::Board() {
	hash = new Hash;
	pattern = new Pattern;
	init(15);
	time_turn = 10000;
	time_match = 100000000;
	time_left = 100000000;
}

void Board::init(int _size) {
	size = _size;
	int i, j;
	for (i = 0; i < 1024; i++) {
		for (j = 0; j < 1024; j++) {
			distance[i][j] = (i % 32 - j % 32) * (i % 32 - j % 32) + (i / 32 - j / 32) * (i / 32 - j / 32);
		}
	}
	for (i = 0; i < 100; i++) {
		distanceNear[i] = 0;
	}
	distanceNear[1] = 1;
	distanceNear[2] = 1;
	distanceNear[4] = 1;
	distanceNear[8] = 1;
	distanceNear[9] = 1;
	distanceNear[18] = 1;
	distanceNear[16] = 1;
	distanceNear[32] = 1;
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 32; j++) {
			board[i * 32 + j] = 3;
		}
	}
	for (i = 6; i < 6 + size; i++) {
		for (j = 6; j < 6 + size; j++) {
			board[i * 32 + j] = 0;
		}
	}
	int range[8] = { -1,-31,-32,-33,1,31,32,33 };
	int horse[8] = { -65,-63,-34,-30,30,34,63,65 };
	int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
	for (i = 0; i < 8; i++) {
		Range1[n1++] = range[i];
	}
	for (i = 1; i <= 2; i++) {
		for (j = 0; j < 8; j++) {
			Range2[n2++] = i * range[j];
		}
	}
	for (i = 0; i < 8; i++) {
		Range2[n2++] = horse[i];
	}
	for (i = 1; i <= 3; i++) {
		for (j = 0; j < 8; j++) {
			Range3[n3++] = i * range[j];
		}
	}
	for (i = 0; i < 8; i++) {
		Range3[n3++] = horse[i];
	}
	for (i = 1; i <= 4; i++) {
		for (j = 0; j < 8; j++) {
			Range4[n4++] = i * range[j];
		}
	}
	int n5 = 0, v5[81];
	for (i = -4; i <= 4; i++) {
		for (j = -4; j <= 4; j++) {
			Range5[n5] = i * 32 + j;
			v5[n5] = -(i * i + j * j);
			n5++;
		}
	}
	sort(Range5, v5, n5);
	int a, b, k;
	for (a = 6; a < 6 + size; a++) {
		for (b = 6; b < 6 + size; b++) {
			k = a * 32 + b;
			if (board[k] == Empty) {
				for (j = 0; j < 4; j++) {
					lineX[k][j] = pattern->patternLineFive[board[k + Range1[j] * -5]][board[k + Range1[j] * -4]][board[k + Range1[j] * -3]][board[k + Range1[j] * -2]][board[k + Range1[j] * -1]];
					lineY[k][j] = pattern->patternLineFive[board[k + Range1[j] * +1]][board[k + Range1[j] * +2]][board[k + Range1[j] * +3]][board[k + Range1[j] * +4]][board[k + Range1[j] * +5]];
				}
			}
		}
	}
	for (a = 6; a < 6 + size; a++) {
		for (b = 6; b < 6 + size; b++) {
			k = a * 32 + b;
			if (board[k] == Empty) {
				for (j = 0; j < 4; j++) {
					oneRank[k][1][j] = pattern->patternOneRank1[lineX[k][j]][lineY[k][j]];
					oneRank[k][2][j] = pattern->patternOneRank2[lineX[k][j]][lineY[k][j]];
					oneLine[k][1][j] = pattern->patternOneLine1[lineX[k][j]][lineY[k][j]];
					oneLine[k][2][j] = pattern->patternOneLine2[lineX[k][j]][lineY[k][j]];
				}
				fourLine[k][1] = pattern->patternFourLine1[oneLine[k][1][0]][oneLine[k][1][1]][oneLine[k][1][2]][oneLine[k][1][3]];
				fourLine[k][2] = pattern->patternFourLine2[oneLine[k][2][0]][oneLine[k][2][1]][oneLine[k][2][2]][oneLine[k][2][3]];
				shape[1][fourLine[k][1]]++;
				shape[2][fourLine[k][2]]++;
			}
		}
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 40; j++) {
			shape[i][j] = 0;
		}
	}
	for (i = 0; i < 1000; i++) {
		line[i] = 0;
		lineVCF[i] = 0;
		lineVCT[i] = 0;
	}
	mayn = 0;
	ply = 0;
	maxply = 0;
	moven = 0;
	linen = 0;
	who = 1;
	opp = 2;
	lastVCTMove = 0;
}

void Board::move(int m) {
	if (board[m]) return;

	ply++;
	if (ply > maxply) maxply = ply;

	moven++;

	int i, j, k;

	shape[1][fourLine[m][1]]--;
	shape[2][fourLine[m][2]]--;

	if (fourLine[m][who] >= ShapeL) {
		lineVCF[linen] = 1;
	}
	else {
		lineVCF[linen] = 0;
	}
	if (fourLine[m][who] >= ShapeM) {
		lineVCT[linen] = 1;
	}
	else {
		lineVCT[linen] = 0;
	}
	if (fourLine[m][who] >= ShapeO) {
		lineVC2[linen] = 1;
	}
	else {
		lineVC2[linen] = 0;
	}

	board[m] = who;
	line[linen++] = m;

	hash->move(m, who);

	for (i = 0; i < 24; i++) {
		if (neighbor[m + Range2[i]]++ == 0) {
			maym[mayn++] = m + Range2[i];
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 1; j < 6; j++) {
			k = m + Range1[i] * j;
			lineX[k][i] = pattern->patternLineFive[board[k + Range1[i] * -5]][board[k + Range1[i] * -4]][board[k + Range1[i] * -3]][board[k + Range1[i] * -2]][board[k + Range1[i] * -1]];
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = -1; j > -6; j--) {
			k = m + Range1[i] * j;
			lineY[k][i] = pattern->patternLineFive[board[k + Range1[i] * +1]][board[k + Range1[i] * +2]][board[k + Range1[i] * +3]][board[k + Range1[i] * +4]][board[k + Range1[i] * +5]];
		}
	}

	for (i = 0; i < 32; i++) {
		k = m + Range4[i];
		if (board[k] == Empty) {
			shape[1][fourLine[k][1]]--;
			shape[2][fourLine[k][2]]--;
			j = i % 4;
			oneRank[k][1][j] = pattern->patternOneRank1[lineX[k][j]][lineY[k][j]];
			oneRank[k][2][j] = pattern->patternOneRank2[lineX[k][j]][lineY[k][j]];
			oneLine[k][1][j] = pattern->patternOneLine1[lineX[k][j]][lineY[k][j]];
			oneLine[k][2][j] = pattern->patternOneLine2[lineX[k][j]][lineY[k][j]];
			fourLine[k][1] = pattern->patternFourLine1[oneLine[k][1][0]][oneLine[k][1][1]][oneLine[k][1][2]][oneLine[k][1][3]];
			fourLine[k][2] = pattern->patternFourLine2[oneLine[k][2][0]][oneLine[k][2][1]][oneLine[k][2][2]][oneLine[k][2][3]];
			shape[1][fourLine[k][1]]++;
			shape[2][fourLine[k][2]]++;
		}
	}

	who = 3 - who;
	opp = 3 - opp;
}

void Board::undo() {
	if (linen <= 0) return;

	ply--;

	int i, j, k;

	int m = line[--linen];

	board[m] = Empty;

	who = 3 - who;
	opp = 3 - opp;

	hash->undo(m, who);

	for (i = 0; i < 24; i++) {
		if (--neighbor[m + Range2[i]] == 0) {
			mayn--;
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 6; j++) {
			k = m + Range1[i] * j;
			lineX[k][i] = pattern->patternLineFive[board[k + Range1[i] * -5]][board[k + Range1[i] * -4]][board[k + Range1[i] * -3]][board[k + Range1[i] * -2]][board[k + Range1[i] * -1]];
		}
	}

	for (i = 0; i < 4; i++) {
		for (j = 0; j > -6; j--) {
			k = m + Range1[i] * j;
			lineY[k][i] = pattern->patternLineFive[board[k + Range1[i] * +1]][board[k + Range1[i] * +2]][board[k + Range1[i] * +3]][board[k + Range1[i] * +4]][board[k + Range1[i] * +5]];
		}
	}

	for (i = 0; i < 32; i++) {
		k = m + Range4[i];
		if (board[k] == Empty) {
			shape[1][fourLine[k][1]]--;
			shape[2][fourLine[k][2]]--;
			j = i % 4;
			oneRank[k][1][j] = pattern->patternOneRank1[lineX[k][j]][lineY[k][j]];
			oneRank[k][2][j] = pattern->patternOneRank2[lineX[k][j]][lineY[k][j]];
			oneLine[k][1][j] = pattern->patternOneLine1[lineX[k][j]][lineY[k][j]];
			oneLine[k][2][j] = pattern->patternOneLine2[lineX[k][j]][lineY[k][j]];
			fourLine[k][1] = pattern->patternFourLine1[oneLine[k][1][0]][oneLine[k][1][1]][oneLine[k][1][2]][oneLine[k][1][3]];
			fourLine[k][2] = pattern->patternFourLine2[oneLine[k][2][0]][oneLine[k][2][1]][oneLine[k][2][2]][oneLine[k][2][3]];
			shape[1][fourLine[k][1]]++;
			shape[2][fourLine[k][2]]++;
		}
	}
	k = m;
	for (j = 0; j < 4; j++) {
		oneRank[k][1][j] = pattern->patternOneRank1[lineX[k][j]][lineY[k][j]];
		oneRank[k][2][j] = pattern->patternOneRank2[lineX[k][j]][lineY[k][j]];
		oneLine[k][1][j] = pattern->patternOneLine1[lineX[k][j]][lineY[k][j]];
		oneLine[k][2][j] = pattern->patternOneLine2[lineX[k][j]][lineY[k][j]];
	}
	fourLine[k][1] = pattern->patternFourLine1[oneLine[k][1][0]][oneLine[k][1][1]][oneLine[k][1][2]][oneLine[k][1][3]];
	fourLine[k][2] = pattern->patternFourLine2[oneLine[k][2][0]][oneLine[k][2][1]][oneLine[k][2][2]][oneLine[k][2][3]];
	shape[1][fourLine[m][1]]++;
	shape[2][fourLine[m][2]]++;
}

void Board::nullMove() {
	ply++;
	if (ply > maxply) maxply = ply;
	moven++;
	line[linen++] = 0;
	who = 3 - who;
	opp = 3 - opp;
}

void Board::nullUndo() {
	ply--;
	linen--;
	who = 3 - who;
	opp = 3 - opp;
}

int Board::getRank(int m, int w) {
	if (w == 0) return getRank0(m);
	int v1, v2;
	v1 = oneRank[m][1][0] + oneRank[m][1][1] + oneRank[m][1][2] + oneRank[m][1][3];
	v2 = oneRank[m][2][0] + oneRank[m][2][1] + oneRank[m][2][2] + oneRank[m][2][3];
	return (w == 1 ? v1 * 2 + v2 : v2 * 2 + v1);
}

int Board::getRank0(int m) {
	int v1, v2;
	v1 = oneRank[m][1][0] + oneRank[m][1][1] + oneRank[m][1][2] + oneRank[m][1][3];
	v2 = oneRank[m][2][0] + oneRank[m][2][1] + oneRank[m][2][2] + oneRank[m][2][3];
	return v1 + v2;
}

void Board::generate(int l[], int v[], int& n) {
	if (linen == 0) {
		n = 1;
		l[0] = 429;
		return;
	}
	if (shape[who][ShapeA] > 0) {
		n = 1;
		l[0] = findMove(who, ShapeA);
		return;
	}
	if (shape[opp][ShapeA] > 0) {
		n = 1;
		l[0] = findMove(opp, ShapeA);
		return;
	}
	n = 0;
	int i, m, o;
	if (shape[who][ShapeB] == 0 && shape[opp][ShapeB] + shape[opp][ShapeC] > 0) {
		o = 0;
	}
	else {
		o = who;
	}
	for (i = mayn - 1; i >= 0; i--) {
		m = maym[i];
		if (isMove(m) && (fourLine[m][who] > ShapeO || fourLine[m][opp] > ShapeO)) {
			l[n] = m;
			v[n] = getRank(m, o);
			n++;
		}
	}
	return;
}

void Board::generate(int l[], int v[], int& n, int move) {
	if (linen == 0) {
		n = 1;
		l[0] = 429;
		return;
	}
	if (shape[who][ShapeA] > 0) {
		n = 1;
		l[0] = findMove(who, ShapeA);
		return;
	}
	if (shape[opp][ShapeA] > 0) {
		n = 1;
		l[0] = findMove(opp, ShapeA);
		return;
	}
	n = 0;
	int i, m, o;
	if (shape[who][ShapeB] == 0 && shape[opp][ShapeB] + shape[opp][ShapeC] > 0) {
		o = 0;
	}
	else {
		o = who;
	}
	int hashMove = 0;
	if (isMove(move)) {
		hashMove = move;
		l[n] = move;
		v[n] = winMax;
		n++;
	}
	for (i = mayn - 1; i >= 0; i--) {
		m = maym[i];
		if (isMove(m) && m != hashMove && (fourLine[m][who] > ShapeO || fourLine[m][opp] > ShapeO)) {
			l[n] = m;
			v[n] = getRank(m, o);
			n++;
		}
	}
	return;
}

int Board::blackLineType(int m, int i) {
	return pattern->patternOneLine1[lineX[m][i]][lineY[m][i]];
}

int Board::isForbidden4(int m, int w) {
	if (w != 1) return 0;
	if (fourLine[m][1] < ShapeX) return 0;
	return 1;
}

int Board::isForbidden3(int m, int w) {
	if (w != 1) return 0;
	if (fourLine[m][1] < ShapeX) return 0;
	if (fourLine[m][1] == ShapeX) return 1;
	if (fourLine[m][1] == ShapeY) return 1;
	int i, j, k, l = 0;
	int x[4] = { 0,0,0,0 };
	for (i = 0; i < 4; i++) {
		if (oneLine[m][1][i] == Opened3) x[i] = 1;
	}
	w = who;
	who = 1;
	opp = 2;
	move(m);
	for (i = 0; i < 4; i++) {
		if (x[i] == 0) continue;
		for (j = -3; j <= 3; j++) {
			k = m + Range1[i] * j;
			if (blackLineType(k, i) == Opened4) {
				if (isForbidden4(k, 1) == 0) {
					l++;
					break;
				}
			}
		}
	}
	undo();
	who = w;
	opp = 3 - who;
	if (l >= 2) return 1;
	return 0;
}

int Board::isForbidden2(int m, int w) {
	if (w != 1) return 0;
	if (fourLine[m][1] < ShapeX) return 0;
	if (fourLine[m][1] == ShapeX) return 1;
	if (fourLine[m][1] == ShapeY) return 1;
	int i, j, k, l = 0;
	int x[4] = { 0,0,0,0 };
	for (i = 0; i < 4; i++) {
		if (oneLine[m][1][i] == Opened3) x[i] = 1;
	}
	w = who;
	who = 1;
	opp = 2;
	move(m);
	for (i = 0; i < 4; i++) {
		if (x[i] == 0) continue;
		for (j = -3; j <= 3; j++) {
			k = m + Range1[i] * j;
			if (blackLineType(k, i) == Opened4) {
				if (isForbidden3(k, 1) == 0) {
					l++;
					break;
				}
			}
		}
	}
	undo();
	who = w;
	opp = 3 - who;
	if (l >= 2) return 1;
	return 0;
}

int Board::isForbidden(int m, int w) {
	if (w != 1) return 0;
	if (fourLine[m][1] < ShapeX) return 0;
	if (fourLine[m][1] == ShapeX) return 1;
	if (fourLine[m][1] == ShapeY) return 1;
	int i, j, k, l = 0;
	int x[4] = { 0,0,0,0 };
	for (i = 0; i < 4; i++) {
		if (oneLine[m][1][i] == Opened3) x[i] = 1;
	}
	w = who;
	who = 1;
	opp = 2;
	move(m);
	for (i = 0; i < 4; i++) {
		if (x[i] == 0) continue;
		for (j = -3; j <= 3; j++) {
			k = m + Range1[i] * j;
			if (blackLineType(k, i) == Opened4) {
				if (isForbidden2(k, 1) == 0) {
					l++;
					break;
				}
			}
		}
	}
	undo();
	who = w;
	opp = 3 - who;
	if (l >= 2) return 1;
	return 0;
}

int Board::isForbidden(int m) {
	return isForbidden(m, who);
}

int Board::isWinner(int m, int w) {
	if (w != 1 && w != 2) return 0;
	if (!isMove(m)) return 0;
	if (isForbidden(m, w)) return 3 - w;
	if (fourLine[m][w] == ShapeA) return w;
	return 0;
}

int Board::isWinner(int m) {
	return isWinner(m, who);
}

int Board::isMove(int m) {
	if (linen == 0 && board[m] == Empty) return 1;
	if (neighbor[m] > 0 && board[m] == Empty) {
		return 1;
	}
	return 0;
}

int Board::isMove(int m, int w) {
	if (w != 1 && w != 2) return 0;
	if (linen == 0 && board[m] == Empty) return 1;
	if (neighbor[m] > 0 && board[m] == Empty) {
		if (w == 1 && isForbidden(m, w)) return 0;
		return 1;
	}
	return 0;
}

int Board::findMove(int w, FourShape s) {
	for (int i = 0; i < mayn; i++) {
		int m = maym[i];
		if (isMove(m) && fourLine[m][w] == s) {
			return m;
		}
	}
	return -1;
}

int Board::evaluate() {
	int i;
	int eval[4] = { 0, 0, 0, 0 };
	static int v[15] = { 0, 1, 2, 6, 10, 14, 18, 22, 26, 20, 30, 50, 100, 200, 0 };
	for (i = 0; i < 15; i++) {
		eval[who] += shape[who][i] * v[i];
		eval[opp] += shape[opp][i] * v[i];
	}
	eval[1] /= 2;
	eval[2] /= 2;
	return (int)(eval[who] * 1.0 - eval[opp] * 1.0 + 0);
}

void Board::quickSort(int t[], int v[], int l, int r) {
	int i = l, j = r;
	int baset, basev;
	baset = t[l];
	basev = v[l];
	while (i < j) {
		while (i < j && basev > v[j]) {
			j--;
		}
		if (i < j) {
			t[i] = t[j];
			v[i] = v[j];
			i++;
		}
		while (i < j && basev < v[i]) {
			i++;
		}
		if (i < j) {
			t[j] = t[i];
			v[j] = v[i];
			j--;
		}
	}
	t[i] = baset;
	v[i] = basev;
	if (l < i) {
		quickSort(t, v, l, i - 1);
	}
	if (r > i) {
		quickSort(t, v, i + 1, r);
	}
}

void Board::slowSort(int t[], int v[], int n) {
	int temp;
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (v[j] > v[i]) {
				temp = t[i];
				t[i] = t[j];
				t[j] = temp;
				temp = v[i];
				v[i] = v[j];
				v[j] = temp;
			}
		}
	}
}

void Board::sort(int t[], int v[], int n) {
	if (n > 1) {
#if 0
		quickSort(t, v, 0, n - 1);
#else
		slowSort(t, v, n);
#endif
	}
}

void Board::slowSortI(int t[], int v[], int i, int n) {
	int temp;
	for (int j = i + 1; j < n; j++) {
		if (v[j] > v[i]) {
			temp = t[i];
			t[i] = t[j];
			t[j] = temp;
			temp = v[i];
			v[i] = v[j];
			v[j] = temp;
		}
	}
}

int Board::findVCFMove() {
	if (linen < 2) return -1;
	for (int i = linen - 2; i >= 0; i -= 2) {
		if (lineVCF[i]) {
			return line[i];
		}
	}
	return -1;
}

int Board::findVCTMove() {
	if (linen < 2) return -1;
	for (int i = linen - 2; i >= 0; i -= 2) {
		if (lineVCT[i]) {
			return line[i];
		}
	}
	return -1;
}

int Board::findVC2Move() {
	if (linen < 2) return -1;
	for (int i = linen - 2; i >= 0; i -= 2) {
		if (lineVC2[i]) {
			return line[i];
		}
	}
	return -1;
}

int Board::vcfSearch(int searcher, int depth, int lastMove, int* winMove) {
	if (vcfNode++ >= vcfNodeMax) return 0;
	int q, k, m;
	int winMove0;
	if (shape[who][ShapeA] >= 1) {
		*winMove = findMove(who, ShapeA);
		return ply + 1;
	}
	if (shape[opp][ShapeA] >= 2) {
		return -(ply + 2);
	}
	if (shape[opp][ShapeA] == 1) {
		m = findMove(opp, ShapeA);
		if (isMove(m, who)) {
			move(m);
			q = -vcfSearch(searcher, depth + 1, lastMove, &winMove0);
			undo();
			*winMove = m;
			return q;
		}
		else {
			return -(ply + 2);
		}
	}
	if (shape[who][ShapeB] >= 1) {
		m = findMove(who, ShapeB);
		if (isMove(m, who)) {
			*winMove = m;
			return ply + 3;
		}
	}
	if (depth >= VCF_MAX_DEPTH) return 0;
	int l[225], v[225], n = 0;
	if (who == searcher) {
		int qMax = -10000;
		for (k = 1; k < 81; k++) {
			m = lastMove + Range5[k];
			if (isMove(m) && (fourLine[m][who] >= ShapeF && fourLine[m][who] < ShapeX)) {
				if (!isMove(m, who)) continue;
				l[n] = m;
				v[n] = getRank(m, who);
				n++;
			}
		}
		sort(l, v, n);
		for (k = 0; k < n; k++) {
			if (k >= 6) break;
			m = l[k];
			move(m);
			q = -vcfSearch(searcher, depth + 1, m, &winMove0);
			undo();
			if (q > 0) {
				*winMove = m;
				return q;
			}
			if (q > qMax) qMax = q;
		}
		return qMax;
	}
	return 0;
}

int Board::vcfSearch(int* winMove) {
	int lastMove, result;
	lastMove = findVCFMove();
	if (lastMove != -1) {
		vcfStart(1000000);
		result = vcfSearch(who, 0, lastMove, winMove);
		return result;
	}
	return 0;
}

int Board::vcfSearch() {
	int winMove;
	int lastMove, result;
	lastMove = findVCFMove();
	if (lastMove != -1) {
		vcfStart(1000000);
		result = vcfSearch(who, 0, lastMove, &winMove);
		return result;
	}
	return 0;
}

int Board::vctSearch(int searcher, int depth, int maxDepth, int lastMove, int* winMove) {
	if (vctNode++ >= vctNodeMax) return 0;
	if ((getTime() - vctTimeStart) >= vctTimeMax) return 0;
	int q, k, m;
	int winMove0;
	if (shape[who][ShapeA] >= 1) {
		*winMove = findMove(who, ShapeA);
		return ply + 1;
	}
	if (shape[opp][ShapeA] >= 2) {
		return -(ply + 2);
	}
	if (shape[opp][ShapeA] == 1) {
		m = findMove(opp, ShapeA);
		if (isMove(m, who)) {
			move(m);
			q = -vctSearch(searcher, depth + 1, maxDepth, lastMove, &winMove0);
			undo();
			*winMove = m;
			return q;
		}
		else {
			return -(ply + 2);
		}
	}
	if (shape[who][ShapeB] >= 1) {
		m = findMove(who, ShapeB);
		if (isMove(m, who)) {
			*winMove = m;
			return ply + 3;
		}
	}
	if (depth >= maxDepth) return 0;
	if (depth >= VCT_MAX_DEPTH) return 0;
	int l[225], v[225], n = 0;
	if (who != searcher) {
		int qMin = 0;
		for (k = mayn - 1; k >= 0; k--) {
			m = maym[k];
			if (isMove(m) && (fourLine[m][who] > ShapeO || fourLine[m][opp] > ShapeO)) {
				l[n] = m;
				v[n] = getRank0(m);
				n++;
			}
		}
		sort(l, v, n);
		int t = 0;
		for (k = 0; k < n; k++) {
			if (k >= 25) break;
			m = l[k];
			if (fourLine[m][who] < ShapeF && fourLine[m][opp] < ShapeF && t++ >= 5) continue;
			if (!isMove(m, who)) continue;
			move(m);
			q = -vctSearch(searcher, depth + 1, maxDepth, lastMove, &winMove0);
			undo();
			if (q >= 0) return q;
			if (q < qMin) qMin = q;
		}
		return qMin;
	}
	if (who == searcher) {
		int qMax = -10000;
		for (k = 0; k < 32; k++) {
			m = lastMove + Range4[k];
			if (isMove(m) && (fourLine[m][who] >= ShapeJ)) {
				if (!isMove(m, who)) continue;
				l[n] = m;
				v[n] = getRank(m, who);
				n++;
			}
		}
		sort(l, v, n);
		for (k = 0; k < n; k++) {
			if (k >= 4) break;
			m = l[k];
			move(m);
			q = -vctSearch(searcher, depth + 1, maxDepth, m, &winMove0);
			undo();
			if (q > 0) {
				*winMove = m;
				return q;
			}
			if (q > qMax) qMax = q;
		}
		return qMax;
	}
	return 0;
}

int Board::vctSearch(int* winMove, int maxDepth, int maxNode, int maxTime) {
	vctStart(maxNode, maxTime);
	long timeStart = vctTimeStart;
	int lastMove = 0, depth = 0, result = 0;
	lastMove = findVCTMove();
	if (lastMove != -1) {
		for (depth = 1; depth <= VCT_MAX_DEPTH; depth += 2) {
			if (depth >= maxDepth) break;
			result = vctSearch(who, 0, depth, lastMove, winMove);
			if (result > 0 || vctNode >= maxNode || (getTime() - vctTimeStart) * 3 >= maxTime) {
				if (result > 0) lastVCTMove = *winMove;
				break;
			}
		}
	}
	if (lastVCTMove != 0 && result <= 0) {
		lastMove = lastVCTMove;
		if (lastMove != -1 && isMove(lastMove)) {
			vctStart(maxNode, maxTime);
			for (depth = 1; depth <= VCT_MAX_DEPTH; depth += 2) {
				if (depth >= maxDepth) break;
				result = vctSearch(who, 0, depth, lastMove, winMove);
				if (result > 0 || vctNode >= maxNode || (getTime() - vctTimeStart) * 3 >= maxTime) {
					if (result > 0) lastVCTMove = *winMove;
					break;
				}
			}
		}
	}
	if (result <= 0) result = 0;
	info << "VCT Search depth " << depth << " nodes " << vctNode << " time " << getTime() - timeStart << " result " << result << endl;
	return 0;
}

int Board::vctSearch(int maxDepth, int maxNode, int maxTime) {
	vctStart(maxNode, maxTime);
	int winMove;
	int lastMove = 0, depth = 0, result = 0;
	lastMove = findVCTMove();
	if (lastMove != -1) {
		for (depth = 1; depth <= VCT_MAX_DEPTH; depth += 2) {
			if (depth >= maxDepth) break;
			result = vctSearch(who, 0, depth, lastMove, &winMove);
			if (result > 0 || vctNode >= maxNode || (getTime() - vctTimeStart) * 3 >= maxTime) {
				break;
			}
		}
		if (result > 0) return result;
	}
	return 0;
}

int Board::vc2Search(int searcher, int depth, int maxDepth, int free, int lastMove, int* winMove) {
	if (vc2Node++ >= vc2NodeMax) return 0;
	if ((getTime() - vc2TimeStart) >= vc2TimeMax) return 0;
	if (depth == 0 && isMove(lastMove, who)) {
		int winMove0;
		move(lastMove);
		int q = -vc2Search(searcher, depth + 1, maxDepth, free, lastMove, &winMove0);
		undo();
		*winMove = lastMove;
		return q;
	}
	if (free < 0) return 0;
	int q, k, m;
	int winMove0;
	if (shape[who][ShapeA] >= 1) {
		*winMove = findMove(who, ShapeA);
		return ply + 1;
	}
	if (shape[opp][ShapeA] >= 2) {
		return -(ply + 2);
	}
	if (shape[opp][ShapeA] == 1) {
		m = findMove(opp, ShapeA);
		if (isMove(m, who)) {
			move(m);
			q = -vc2Search(searcher, depth + 1, maxDepth, free, lastMove, &winMove0);
			undo();
			*winMove = m;
			return q;
		}
		else {
			return -(ply + 2);
		}
	}
	if (shape[who][ShapeB] >= 1) {
		m = findMove(who, ShapeB);
		if (isMove(m, who)) {
			*winMove = m;
			return ply + 3;
		}
	}
	if (depth >= maxDepth) return 0;
	if (depth >= VC2_MAX_DEPTH) return 0;
	int l[225], v[225], n = 0;
	if (who != searcher) {
		int qMin = 0;
		for (k = mayn - 1; k >= 0; k--) {
			m = maym[k];
			if (isMove(m) && (fourLine[m][who] > ShapeO || fourLine[m][opp] > ShapeO)) {
				l[n] = m;
				v[n] = getRank0(m);
				n++;
			}
		}
		sort(l, v, n);
		int t = 0;
		for (k = 0; k < n; k++) {
			if (k >= 25) break;
			m = l[k];
			if (fourLine[m][who] < ShapeF && fourLine[m][opp] < ShapeF && t++ >= 7) continue;
			if (!isMove(m, who)) continue;
			move(m);
			q = -vc2Search(searcher, depth + 1, maxDepth, free, lastMove, &winMove0);
			undo();
			if (q >= 0) return q;
			if (q < qMin) qMin = q;
		}
		return qMin;
	}
	if (who == searcher) {
		int qMax = -10000;
		if (lastMove > 0) {
			n = 0;
			for (k = 0; k < 32; k++) {
				m = lastMove + Range4[k];
				if (isMove(m) && (fourLine[m][who] >= ShapeJ)) {
					if (!isMove(m, who)) continue;
					l[n] = m;
					v[n] = getRank(m, who);
					n++;
				}
			}
			sort(l, v, n);
			for (k = 0; k < n; k++) {
				if (k >= 4) break;
				m = l[k];
				move(m);
				q = -vc2Search(searcher, depth + 1, maxDepth, free, m, &winMove0);
				undo();
				if (q > 0) {
					*winMove = m;
					return q;
				}
				if (q > qMax) qMax = q;
			}
		}
		if (free > 0) {
			n = 0;
			for (k = mayn - 1; k >= 0; k--) {
				m = maym[k];
				if (isMove(m) && (fourLine[m][who] > ShapeO || fourLine[m][opp] > ShapeO)) {
					l[n] = m;
					v[n] = getRank(m, who);
					n++;
				}
			}
			sort(l, v, n);
			for (k = 0; k < n; k++) {
				if (k >= 10) break;
				if (depth > 0 && k >= 6) break;
				m = l[k];
				if (!isMove(m, who)) continue;
				int free0 = free - 1;
				int d = distance[m][lastMove];
				if (d < 100) if (distanceNear[d] > 0) free0++;
				if (free0 == free && fourLine[m][who] >= ShapeJ) continue;
				move(m);
				q = -vc2Search(searcher, depth + 1, maxDepth, free0, m, &winMove0);
				undo();
				if (q > 0) {
					*winMove = m;
					return q;
				}
				if (q > qMax) qMax = q;
			}
		}
		return qMax;
	}
	return 0;
}

int Board::vc2Search(int* winMove, int maxDepth, int maxNode, int maxTime) {
	vc2Start(maxNode, maxTime);
	int lastMove = 0, depth = 0, result = 0;
	lastMove = findVC2Move();
	for (depth = 1; depth <= VC2_MAX_DEPTH; depth += 2) {
		if (depth >= maxDepth) break;
		result = vc2Search(who, 0, depth, 1 + depth / 10, lastMove, winMove);
		if (result > 0 || vc2Node >= maxNode || (getTime() - vc2TimeStart) * 2 >= maxTime) {
			break;
		}
	}
	if (result < 0) result = 0;
	info << "VC2 Search depth " << depth << " nodes " << vc2Node << " time " << getTime() - vc2TimeStart << " result " << result << endl;
	return result;
}

int Board::vc2Search(int maxDepth, int maxNode, int maxTime) {
	vc2Start(maxNode, maxTime);
	int winMove;
	int lastMove = 0, depth = 0, result = 0;
	lastMove = findVC2Move();
	for (depth = 1; depth <= VC2_MAX_DEPTH; depth += 2) {
		if (depth >= maxDepth) break;
		result = vc2Search(who, 0, depth, 1 + depth / 10, lastMove, &winMove);
		if (result > 0 || vc2Node >= maxNode || (getTime() - vc2TimeStart) * 2 >= maxTime) {
			break;
		}
	}
	if (result < 0) result = 0;
	return result;
}

int Board::vc1Search(int depth, float maxDepth, int alpha, int beta) {
	vc1Node++;
	if (getTime() >= time_max) {
		terminater = 1;
		return 10000;
	}
	int eval = evaluate();
	vc1Eval[depth] = eval;
	if (depth >= 1)eval = (vc1Eval[depth] - vc1Eval[depth - 1]) / 2;
	if (depth >= maxDepth && (depth & 1) == 1) return eval;
	int q, m;
	int hashMove = 0;
	q = hash->read(maxDepth - (float)depth, alpha, beta, vc1RootWho, hashMove);
	if (q != unknown) {
		if (depth > 0) return q;
	}
	int hashf = Alpha;
	if (shape[who][ShapeA] >= 1) {
		return 10000;
	}
	if (shape[opp][ShapeA] >= 2) {
		return -10000;
	}
	int i, value;
	if (depth >= 1 && vc1Four[depth - 2]) {
		value = vcfSearch();
		if (depth + maxply > seldepth) seldepth = depth + maxply;
		if (value > 0 && depth > 0) return 10000;
	}
	if (shape[opp][ShapeA] == 1) {
		m = findMove(opp, ShapeA);
		if (isMove(m, who)) {
			if (fourLine[m][who] >= ShapeF && fourLine[m][who] < ShapeX) vc1Four[depth] = 1;
			else vc1Four[depth] = 0;
			move(m);
			q = -vc1Search(depth + 1, maxDepth, -beta, -alpha);
			undo();
			if (q >= beta) q = beta;
			if (q <= alpha) q = alpha;
			return q;
		}
		else {
			return -10000;
		}
	}
	if (shape[who][ShapeB] >= 1) {
		m = findMove(who, ShapeB);
		if (isMove(m, who)) {
			return 10000;
		}
	}
	if (depth >= (int)(maxDepth * 0.4) && maxDepth - depth < 10) {
		float d = maxDepth - depth;
		if (d < 0) d = 0;
		if (eval - 20 - 20 * d >= beta) return beta;
		if (eval + 20 + 20 * d <= alpha) return alpha;
	}
	int l[225], v[225], n;
	generate(l, v, n, hashMove);
	sort(l, v, n);
	int count = 16;
	if (depth >= 2) count -= depth;
	if (count <= 4) count = 4;
	if (shape[opp][ShapeA] + shape[opp][ShapeB] + shape[opp][ShapeC]) count = 25;
	int bestValue = -10000;
	int bestMove = l[0];
	for (i = 0; i < n; i++) {
		if (i >= 25) break;
		if (i >= count && bestValue > -10000) break;
		m = l[i];
		if (!isMove(m, who)) continue;
		float newMaxDepth = maxDepth;
		if (fourLine[m][who] >= ShapeJ) newMaxDepth += 0.35f;
		else newMaxDepth -= 0.35f;
		if (i >= 4) newMaxDepth -= 0.35f;
		if (fourLine[m][who] >= ShapeF && fourLine[m][who] < ShapeX) vc1Four[depth] = 1;
		else vc1Four[depth] = 0;
		move(m);
		value = -vc1Search(depth + 1, newMaxDepth, -beta, -alpha);
		undo();
		if (depth == 0 && value > bestValue) vc1BestMove = m;
		if (terminater) return 10000;
		if (value > bestValue) {
			bestValue = value;
			bestMove = m;
			if (value > alpha) {
				alpha = value;
				hashf = Pv;
				if (value >= beta) {
					hash->write(maxDepth - (float)depth, beta, vc1RootWho, m, Beta);
					return beta;
				}
			}
		}
	}
	if (depth == 0) vc1BestMove = bestMove;
	if (terminater == 0){
		hash->write(maxDepth - (float)depth, alpha, vc1RootWho, bestMove, hashf);
	}
	return alpha;
}

int Board::search(int& m) {
	if (shape[who][ShapeA] > 0) {
		m = findMove(who, ShapeA);
		return 5;
	}
	if (shape[opp][ShapeA] > 0) {
		m = findMove(opp, ShapeA);
		return 0;
	}
	if (vcfSearch(&m) > 0) {
		return 4;
	}
	if (time_move >= 60 && vctSearch(&m, 25, 1000000, time_move / 60) > 0) {
		return 3;
	}
#if 0
	if (time_move >= 30 && vc2Search(&m, 25, 1000000, time_move / 30) > 0) {
		return 2;
	}
#endif
	moven = 0;
	vc1BestMove = 0;
	vc1RootWho = who;
	long time0 = getTime();
	for (int d = 1; d < 50; d++) {
		seldepth = d;
		long time1 = getTime();
		int v = vc1Search(0, (float)d, -10000, 10000);
		long time2 = getTime();
		if (terminater) break;
		m = vc1BestMove;
		int x = m / 32 - 6;
		int y = m % 32 - 6;
		info << "VC1 Search depth " << d << " seldepth " << seldepth <<  " move [" << x << "," << y << "] nodes " << moven << " time " << time2 - time0 << " nps " << moven * 1000 / ((time2 - time0 > 0) ? (time2 - time0) : 1) << " value " << v << endl;
		if (time2 - time0 >= time_move * 2 / 5) break;
	}
	return 0;
}

void Board::search(int& x, int& y) {
	//show();
	if (linen == 0) {
		x = 7;
		y = 7;
		return;
	}
	timeControl();
	int m, v;
	v = search(m);
	x = m / 32 - 6;
	y = m % 32 - 6;
	if (v == 6);
	else if (v == 5) info << "WIN [" << x << "," << y << "]" << endl;
	else if (v == 4) info << "VCF [" << x << "," << y << "]" << endl;
	else if (v == 3) info << "VCT [" << x << "," << y << "]" << endl;
	else if (v == 2) info << "VC2 [" << x << "," << y << "]" << endl;
	else if (v == 1) info << "VC1 [" << x << "," << y << "]" << endl;
	else info << "MOVE [" << x << "," << y << "]" << endl;
}