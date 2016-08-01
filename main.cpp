#include<iostream>
#include<cstdlib>
#include<vector>
#include<sstream>
#include<string>
#include "AI.h"
//#include "Test.h"
using namespace std;

int gomocup() {
  AI wine;
  string command;
  Pos input, best;
  char dot;
  int size;
  while (1) {
    cin >> command;
    for (size_t i = 0; i < command.size(); i++) {
      char &c = command[i];
      if (c >= 'a' && c <= 'z') {
        c += 'A' - 'a';
      }
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
    } else if (command == "TAKEBACK") {
      wine.DelMove();
      cout << "OK" << endl;
    } else if (command == "BEGIN") {
      best = wine.gobang();
      wine.MakeMove(best);
      cout << best.x << "," << best.y << endl;
    } else if (command == "TURN") {
      cin >> input.x >> dot >> input.y;
      if (!wine.CheckXy(input.x, input.y)
          || wine.cell[input.x][input.y].piece) {
        cout << "ERROR" << endl;
      } else {
        wine.MakeMove(input);
        best = wine.gobang();
        wine.MakeMove(best);
        cout << "MESSAGE";
        cout << " level=" << wine.MaxDepth;
        cout<< " val=" << wine.BestVal;
        cout<< " NPS=" << wine.total / (wine.ThinkTime + 1) << endl;
        cout << best.x << "," << best.y << endl;
      }
    }
    else if (command == "BOARD") {
      int c;
      Pos m;
      stringstream ss;
      wine.ReStart();

      cin >> command;
      while (command != "DONE") {
        ss.clear();
        ss << command;
        ss >> m.x >> dot >> m.y >> dot >> c;
        if (!wine.CheckXy(m.x, m.y) || (c != 1 && c != 2)) {
          cout << "ERROR" << endl;
        } else {
          wine.MakeMove(m);
        }
        cin >> command;
      }
      if (c == 2) {
        best = wine.gobang();
        wine.MakeMove(best);
        cout << best.x << "," << best.y << endl;
      }
    } else if (command == "INFO") {
      int value;
      string key;
      cin >> key;
      if (key == "timeout_turn") {
        cin >> value;
        if (value != 0)
          wine.timeout_turn = value;

      } else if (key == "timeout_match") {
        cin >> value;
        if (value != 0)
          wine.timeout_match = value;

      } else if (key == "time_left") {
        cin >> value;
        if (value != 0)
          wine.time_left = value;

      } else if (key == "max_memory") {
        cin >> value;
        // TODO
      } else if (key == "game_type") {
        cin >> value;
        // TODO
      } else if (key == "rule") {
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

