#pragma once

using namespace std;

#define IN_LINUX

#ifdef IN_LINUX
#define ull unsigned long long
#else
#define ull unsigned __int64
#endif

#define unknown 20000
#define winMax 10000
#define winMin 9000

enum Hashf { Alpha = 0, Pv = 1, Beta = 2 };
enum Piece { Empty = 0, White = 1, Black = 2, Block = 3 };
enum OneShape { None = 0, Closed1 = 1, Opened1 = 2, Closed2 = 3, Opened2 = 4, Closed3 = 5, Opened3 = 6, Closed4 = 7, Opened4 = 8, Double4 = 9, Opened5 = 10, Long = 11 };
enum FourShape { ShapeA = 14, ShapeB = 13, ShapeC = 12, ShapeD = 11, ShapeE = 10, ShapeF = 9, ShapeG = 8, ShapeH = 7, ShapeI = 6, ShapeJ = 5, ShapeK = 4, ShapeL = 3, ShapeM = 2, ShapeN = 1, ShapeO = 0, ShapeX = 31, ShapeY = 32, ShapeZ = 33 };

#define info cout << "MESSAGE > " 
#define answer(s) cout << #s << endl

#define hashsize 1048576
#define hashmask 1048575