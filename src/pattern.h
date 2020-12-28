#pragma once

using namespace std;

class Pattern {
public:
	Pattern();
	int patternLineFive[4][4][4][4][4];
	int patternOneLine1[1024][1024];
	int patternOneLine2[1024][1024];
	int patternFourLine1[12][12][12][12];
	int patternFourLine2[12][12][12][12];
	int patternOneRank1[1024][1024];
	int patternOneRank2[1024][1024];
	void init();
};