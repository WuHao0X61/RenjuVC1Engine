#include <iostream>

#include "main.h"
#include "pattern.h"

using namespace std;

Pattern::Pattern() {
	init();
}

int whoCount(int l[], int i, int j, int w) {
	int k, n = 0;
	for (k = i; k < j; k++) {
		if (l[k] == w) n++;
	}
	return n;
}
int oppCount(int l[], int i, int j, int w) {
	int k, n = 0;
	for (k = i; k < j; k++) {
		if (l[k] != w && l[k] != 0) n++;
	}
	return n;
}
int emptyCount(int l[], int i, int j) {
	int k, n = 0;
	for (k = i; k < j; k++) {
		if (l[k] == 0) n++;
	}
	return n;
}

int getOneLine(int l[], int w) {
	int i;
	if (w == 1) {
		for (i = 0; i <= 5; i++) {
			if (whoCount(l, i, i + 5, w) == 5) {
				return Long;
			}
		}
	}
	for (i = 1; i <= 5; i++) {
		if (whoCount(l, i, i + 4, w) == 4) {
			return Opened5;
		}
	}
	for (i = 1; i <= 4; i++) {
		if (whoCount(l, i, i + 5, w) == 3 && l[i] == Empty && l[i + 4] == Empty) {
			if (w == 1) {
				if (l[i - 1] != 1 && l[i + 5] != 1) {
					return Opened4;
				}
			}
			else {
				return Opened4;
			}
		}
	}
	int k = 0;
	for (i = 1; i <= 5; i++) {
		if (whoCount(l, i, i + 4, w) == 3 && emptyCount(l, i, i + 4) == 1) {
			if (w == 1) {
				if (l[i - 1] != 1 && l[i + 4] != 1) {
					k++;
				}
			}
			else {
				k++;
			}
		}
	}
	if (k > 0) {
		if (w == 1) {
			if (k >= 2) {
				return Double4;
			}
			if (k == 1) {
				return Closed4;
			}
		}
		else {
			return Closed4;
		}
	}
	for (i = 1; i <= 4; i++) {
		if (whoCount(l, i, i + 5, w) == 2 && emptyCount(l, i, i + 5) == 3 && l[i] == Empty && l[i + 4] == Empty) {
			return Opened3;
		}
	}
	for (i = 1; i <= 5; i++) {
		if (whoCount(l, i, i + 4, w) == 2 && emptyCount(l, i, i + 4) == 2) {
			return Closed3;
		}
	}
	for (i = 1; i <= 4; i++) {
		if (whoCount(l, i, i + 5, w) == 1 && emptyCount(l, i, i + 5) == 4 && l[i] == Empty && l[i + 4] == Empty) {
			return Opened2;
		}
	}
	for (i = 1; i <= 5; i++) {
		if (whoCount(l, i, i + 4, w) == 1 && emptyCount(l, i, i + 4) == 3) {
			return Closed2;
		}
	}
	return None;
}

int getFourLine1(int a, int b, int c, int d) {
	int n[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	n[a]++; n[b]++; n[c]++; n[d]++;
	if (n[Opened5] >= 1) return ShapeA;
	if (n[Long] >= 1) return ShapeX;
	if (n[Opened4] + n[Closed4] >= 2) return ShapeY;
	if (n[Double4] >= 1) return ShapeY;
	if (n[Opened3] >= 2) return ShapeZ;
	if (n[Opened4] >= 1) return ShapeB;
	if (n[Closed4] >= 2) return ShapeB;
	if (n[Closed4] >= 1 && n[Opened3] >= 1) return ShapeC;
	if (n[Closed4] >= 1 && n[Closed3] >= 1) return ShapeD;
	if (n[Closed4] >= 1 && n[Opened2] >= 1) return ShapeD;
	if (n[Closed4] >= 1 && n[Closed2] >= 1) return ShapeE;
	if (n[Closed4] >= 1) return ShapeF;
	if (n[Opened3] >= 2) return ShapeG;
	if (n[Opened3] >= 1 && n[Closed3] >= 1) return ShapeH;
	if (n[Opened3] >= 1 && n[Opened2] >= 1) return ShapeH;
	if (n[Opened3] >= 1 && n[Closed2] >= 1) return ShapeI;
	if (n[Opened3] >= 1) return ShapeJ;
	if (n[Closed3] + n[Opened2] >= 2) return ShapeK;
	if (n[Closed3] >= 1) return ShapeL;
	if (n[Opened2] >= 1) return ShapeM;
	if (n[Closed2] >= 1) return ShapeN;
	return ShapeO;
}

int getFourLine2(int a, int b, int c, int d) {
	int n[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	n[a]++; n[b]++; n[c]++; n[d]++;
	if (n[Opened5] >= 1) return ShapeA;
	if (n[Opened4] >= 1) return ShapeB;
	if (n[Closed4] >= 2) return ShapeB;
	if (n[Closed4] >= 1 && n[Opened3] >= 1) return ShapeC;
	if (n[Closed4] >= 1 && n[Closed3] >= 1) return ShapeD;
	if (n[Closed4] >= 1 && n[Opened2] >= 1) return ShapeD;
	if (n[Closed4] >= 1 && n[Closed2] >= 1) return ShapeE;
	if (n[Closed4] >= 1) return ShapeF;
	if (n[Opened3] >= 2) return ShapeG;
	if (n[Opened3] >= 1 && n[Closed3] >= 1) return ShapeH;
	if (n[Opened3] >= 1 && n[Opened2] >= 1) return ShapeH;
	if (n[Opened3] >= 1 && n[Closed2] >= 1) return ShapeI;
	if (n[Opened3] >= 1) return ShapeJ;
	if (n[Closed3] + n[Opened2] >= 2) return ShapeK;
	if (n[Closed3] >= 1) return ShapeL;
	if (n[Opened2] >= 1) return ShapeM;
	if (n[Closed2] >= 1) return ShapeN;
	return ShapeO;
}

int getFourLine(int a, int b, int c, int d, int e) {
	if (e == 1) return getFourLine1(a, b, c, d);
	if (e == 2) return getFourLine2(a, b, c, d);
	return 0;
}

int getOneRank(int l[], int w) {
	int i, n = 0;
	int v[5] = { 1, 8, 27, 64, 125 };
	for (i = 1; i < 6; i++) {
		if (oppCount(l, i, i + 4, w) == 0) {
			n += v[whoCount(l, i, i + 4, w)];
		}
	}
	return n;
}

void Pattern::init() {
	int a, b, c, d, e, f, g, h, i, j;
	int l[10];

#define D1(a) for (a = 0; a < 4; a++)
#define D2(a) for (a = 0; a < 12; a++)

	D1(a)D1(b)D1(c)D1(d)D1(e) {
		patternLineFive[a][b][c][d][e] = a * 256 + b * 64 + c * 16 + d * 4 + e * 1;
	}

	D1(a)D1(b)D1(c)D1(d)D1(e)D1(f)D1(g)D1(h)D1(i)D1(j) {
		l[0] = a; l[1] = b; l[2] = c; l[3] = d; l[4] = e; l[5] = f; l[6] = g; l[7] = h; l[8] = i; l[9] = j;
		patternOneLine1[patternLineFive[a][b][c][d][e]][patternLineFive[f][g][h][i][j]] = getOneLine(l, 1);
		patternOneLine2[patternLineFive[a][b][c][d][e]][patternLineFive[f][g][h][i][j]] = getOneLine(l, 2);
		patternOneRank1[patternLineFive[a][b][c][d][e]][patternLineFive[f][g][h][i][j]] = getOneRank(l, 1);
		patternOneRank2[patternLineFive[a][b][c][d][e]][patternLineFive[f][g][h][i][j]] = getOneRank(l, 2);
	}

	D2(a)D2(b)D2(c)D2(d) {
		patternFourLine1[a][b][c][d] = getFourLine(a, b, c, d, 1);
		patternFourLine2[a][b][c][d] = getFourLine(a, b, c, d, 2);
	}
}