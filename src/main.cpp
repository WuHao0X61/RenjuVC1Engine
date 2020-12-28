#include <iostream>
#include <sstream>
#include <string>

#include "main.h"
#include "board.h"

using namespace std;

void toupper(string &str) {
	for (size_t i = 0; i < str.size(); i++) {
		char &c = str[i];
		if (c >= 'a' && c <= 'z') {
			c += 'A' - 'a';
		}
	}
}

void gomocup() {
	Board *board = new Board;
	string cmd;
	char dot;
	size_t size;
	for (;;) {
		cin >> cmd;
		toupper(cmd);
		if (cmd == "START") {
			cin >> size;
			if (size == 15) answer(OK);
			else answer(SIZE IS NOT 15!);
		}
		else if (cmd == "RESTART") {
			board->init(15);
			answer(OK);
		}
		else if (cmd == "TAKEBACK") {
			board->undo();
			answer(OK);
		}
		else if (cmd == "BEGIN") {
			int x, y;
			board->search(x, y);
			board->move(x, y);
			cout << x << "," << y << endl;
		}
		else if (cmd == "TURN") {
			int x, y;
			cin >> x >> dot >> y;
			board->move(x, y);
			board->search(x, y);
			board->move(x, y);
			cout << x << "," << y << endl;
		}
		else if (cmd == "BOARD") {
			int x, y, c;
			stringstream ss;
			board->init(15);
			cin >> cmd;
			toupper(cmd);
			while (cmd != "DONE")
			{
				ss.clear();
				ss << cmd;
				ss >> x >> dot >> y >> dot >> c;
				board->move(x, y);
				cin >> cmd;
				toupper(cmd);
			}
			if (c == 2) {
				board->search(x, y);
				board->move(x, y);
				cout << x << "," << y << endl;
			}
		}
		else if (cmd == "INFO") {
			int value;
			string key;
			cin >> key;
			toupper(key);
			if (key == "TIMEOUT_TURN") {
				cin >> value;
				if (value != 0) board->time_turn = value;
			}
			else if (key == "TIMEOUT_MATCH") {
				cin >> value;
				if (value != 0) board->time_match = value;
			}
			else if (key == "TIME_LEFT") {
				cin >> value;
				if (value != 0) board->time_left = value;
			}
			else if (key == "MAX_MEMORY") {
				cin >> value;
			}
			else if (key == "GAME_TYPE") {
				cin >> value;
			}
			else if (key == "RULE") {
				cin >> value;
			}
			else if (key == "FOLDER") {
				string t;
				cin >> t;
			}
		}
		else if (cmd == "END") {
			break;
		}
	}
	delete board;
}

void test() {
	int x = 7, y = 7;
	Board *board = new Board;
	board->move(x, y);
	board->search(x, y);
}

int main() {
#if 0
	test();
#endif
	gomocup();
	return 0;
}