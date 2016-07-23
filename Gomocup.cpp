#include<iostream>
#include<stdlib.h>
#include<vector>
#include<sstream>
#include<string>
#include "AI.h"
#include "AI.cpp"
//#include "Test.cpp"
using namespace std;

int gomocup() {
  AI wine;
  string command;
  Pos input, best;
  vector < int >cl;
  vector < Pos > ps;
  int size;
  char dot;
  bool last_is_board = false;
  while (1) {
    //DrawChess(&wine);//测试代码
    if (last_is_board && cl.back() == 2) {
      command = "BEGIN";
    } else {
      cin >> command;
      for (int i = 0; i < command.size(); i++) {
        char c = command[i];
        if (c >= 'a' && c <= 'z') {
          command[i] += 'A' - 'a';
        }
      }
    }
    if (last_is_board) {
      for (int i = 0; i < ps.size(); i++) {
        wine.MakeMove(ps[i]);
      }
      ps.clear();
      cl.clear();
      last_is_board = false;
    }
    if (command == "START") {
      cin >> size;
      if (size > 20 || size <= 5) {
        cout << "ERROR" << endl;
      } else {
        wine.size = size;
        cout << "OK" << endl;
      }
    }
    if (command == "RESTART") {
      wine.ReStart();
      cout << "OK" << endl;
      // command = "BEGIN";
    } else if (command == "TAKEBACK") {
      wine.DelMove();
      // TODO
    } else if (command == "BEGIN") {
      best = wine.gobang();
      wine.MakeMove(best);
      cout << (int)best.x << "," << (int)best.y << endl;
    } else if (command == "TURN") {
      cin >> input.x >> dot >> input.y;
      //GetXy(input);//调试代码
      if (!wine.CheckXy(input.x, input.y)) {
        cout << "ERROR" << endl;
      } else {
        wine.MakeMove(input);
        best = wine.gobang();
        wine.MakeMove(best);
        cout << (int)best.x << "," << (int)best.y << endl;
      }
    }
    else if (command == "BOARD") {
      cl.clear();
      ps.clear();
      wine.ReStart();
      last_is_board = true;
      stringstream ss;
      cin >> command;
      while (command != "DONE") {
        ss.clear();
        ss << command;
        Pos m;
        int c;
        ss >> m.x >> dot >> m.y >> dot >> c;
        if (!wine.CheckXy(m.x, m.y) || (c != 1 && c != 2)) {
          cout << "ERROR" << endl;
        } else {
          ps.push_back(m);
          cl.push_back(c);
        }
        cin >> command;
      }

    } else if (command == "INFO") {
      int SEARCH_TIME;
      string key;
      cin >> key;
      if (key == "timeout_turn") {
        int value;
        cin >> value;
        if (value >= 100) {
          SEARCH_TIME = value - 170;
        } else {
          SEARCH_TIME = 100;
        }

      } else if (key == "timeout_match") {
        cin >> wine.timeout_match;
      } else if (key == "max_memory") {
        int value;
        cin >> value;
        // TODO
      } else if (key == "time_left") {
        cin >> wine.time_left;
      } else if (key == "game_type") {
        int value;
        cin >> value;
        // TODO
      } else if (key == "rule") {
        int value;
        cin >> value;
        // TODO
      } else if (key == "folder") {
        string t;
        cin >> t;
      }
    } else if (command == "END") {
    	exit(1);

    }

  }

}
int main() {
  gomocup();
}
