#pragma once

#include <math.h>
#include <time.h>
#include <iostream>

#include "main.h"
#include "pattern.h"

#define VCF_MAX_DEPTH 16
#define VCT_MAX_DEPTH 25
#define VC2_MAX_DEPTH 36

using namespace std;

struct HashItem {
	ull key;
	int value;
	int move;
	int who;
	float depth;
	int hashf;
};

class Hash {
public:
	Hash();
	ull zobrist;
	ull* zobrists;
	HashItem* table;
	ull rand64();
	void move(int m, int who);
	void undo(int m, int who);
	int read(float depth, int alpha, int beta, int who, int& move);
	void write(float depth, int value, int who, int move, int hashf);
};

class Board {
public:
	Board();
	Hash* hash;
	Pattern* pattern;
	int distance[1024][1024];
	int distanceNear[100];
	int Range1[8], Range2[24], Range3[32], Range4[32], Range5[81];
	int lineX[1024][4];
	int lineY[1024][4];
	int board[1024];
	int neighbor[1024];
	int fourLine[1024][4];
	int oneLine[1024][4][4];
	int fourRank[1024][4];
	int oneRank[1024][4][4];
	int shape[4][100];
	int line[1000];
	int linen;
	int lineVCF[1000];
	int lineVCT[1000];
	int lineVC2[1000];
	int maym[1024];
	int mayn;
	int size;
	int who;
	int opp;
	int ply;
	int maxply;
	ull moven;
	int vcfNode;
	int vctNode;
	int vc2Node;
	int vc1Node;
	int vcfNodeMax;
	int vctNodeMax;
	int vc2NodeMax;
	long vcfTimeStart;
	long vctTimeStart;
	long vc2TimeStart;
	long vctTimeMax;
	long vc2TimeMax;
	int lastVCTMove;
	int vc1Eval[1000];
	int vc1Four[1000];
	int vc1BestMove;
	int vc1RootWho;
	int time_turn;
	int time_match;
	int time_left;
	int time_move;
	long time_max;
	int terminater;
	int seldepth;
	void init(int _size);
	void move(int m);
	void move(int x, int y) { move((x + 6) * 32 + (y + 6)); }
	void undo();
	void nullMove();
	void nullUndo();
	int getRank(int m, int w);
	int getRank0(int m);
	void generate(int l[], int v[], int& n);
	void generate(int l[], int v[], int& n, int move);
	int blackLineType(int m, int i);
	int isForbidden4(int m, int w);
	int isForbidden3(int m, int w);
	int isForbidden2(int m, int w);
	int isForbidden(int m, int w);
	int isForbidden(int m);
	int isWinner(int m, int w);
	int isWinner(int m);
	int isMove(int m);
	int isMove(int m, int w);
	int isMove(int x, int y, int w) { return isMove((x + 6) * 32 + (y + 6), who); }
	int findMove(int w, FourShape s);
	int evaluate();
	void quickSort(int t[], int v[], int l, int r);
	void slowSort(int t[], int v[], int n);
	void sort(int l[], int v[], int n);
	void slowSortI(int t[], int v[], int i, int n);
	int isExpand() { if (shape[opp][ShapeA] > 0) return 1; return 0; }
	int isExtend() { if (shape[opp][ShapeA] + shape[opp][ShapeB] + shape[opp][ShapeC] > 0) return 1; return 0; }
	long getTime() { return clock() * 1000 / CLOCKS_PER_SEC; }
	int findVCFMove();
	int findVCTMove();
	int findVC2Move();
	void vcfStart(int n) { vcfNodeMax = n; vcfTimeStart = getTime(); vcfNode = 0; ply = 0; maxply = 0; }
	void vctStart(int n, long t) { vctTimeMax = t; vctNodeMax = n; vctTimeStart = getTime(); vctNode = 0; ply = 0; maxply = 0; }
	void vc2Start(int n, long t) { vc2TimeMax = t; vc2NodeMax = n; vc2TimeStart = getTime(); vc2Node = 0; ply = 0; maxply = 0; }
	int vcfSearch(int searcher, int depth, int lastMove, int* winMove);
	int vcfSearch(int* winMove);
	int vcfSearch();
	int vctSearch(int searcher, int depth, int maxDepth, int lastMove, int* winMove);
	int vctSearch(int* winMove, int maxDepth, int maxNode, int maxTime);
	int vctSearch(int maxDepth, int maxNode, int maxTime);
	int vc2Search(int searcher, int depth, int maxDepth, int free, int lastMove, int* winMove);
	int vc2Search(int* winMove, int maxDepth, int maxNode, int maxTime);
	int vc2Search(int maxDepth, int maxNode, int maxTime);
	int vc1Search(int depth, float maxDepth, int alpha, int beta);
	void timeControl() { time_move = time_left / 12; if (time_move > time_turn) time_move = time_turn; time_move -= 500; if (time_match == time_left) time_move -= 2000; if (time_move < 10) time_move = 10; time_max = getTime() + time_move; terminater = 0; }
	int search(int& m);
	void search(int& x, int& y);

	void show() {
		int i, j;
		for (i = 6; i < 21; i++) {
			info;
			for (j = 6; j < 21; j++) {
				cout << board[j * 32 + i];
			}
			cout << endl;
		}
		info << "---" << endl;
	}
};