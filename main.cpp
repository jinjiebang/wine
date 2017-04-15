
#include<iostream>
#include<cstdlib>
#include<sstream>
#include<string>
#include "AI.h"
  using namespace std;

AI wine;

void toupper(string &str){
    for (size_t i = 0; i < str.size(); i++) {
      char &c = str[i];
      if (c >= 'a' && c <= 'z') {
        c += 'A' - 'a';
      }
    }
}

int gomocup() {
  string command;
  Pos input, best;
  char dot;
  int size = 15;
  while (1) {
    cin >> command;
    toupper(command);

    if (command == "START") {
      cin >> size;
      if (size > MaxSize || size <= 5) {
        cout << "ERROR" << endl;
      } else {
        wine.SetSize(size);
        cout << "OK" << endl;
      }
    } else if (command == "RESTART") {
      wine.ReStart();
      cout << "OK" << endl;
    } else if (command == "TAKEBACK") {
      wine.DelMove();
      cout << "OK" << endl;
    } else if (command == "BEGIN") {
      best = wine.TurnBest();
      wine.TurnMove(best);
      cout << best.x << "," << best.y << endl;
    } else if (command == "TURN") {
      cin >> input.x >> dot >> input.y;
      if (input.x < 0 || input.x >= size || input.y < 0 || input.y >= size
          || wine.cell[input.x + 4][input.y + 4].piece != Empty) {
        cout << "ERROR" << endl;
      } else {
        wine.TurnMove(input);
        best = wine.TurnBest();
        wine.TurnMove(best);
        cout << best.x << "," << best.y << endl;
      }
    } else if (command == "BOARD") {
      int c;
      Pos m;
      stringstream ss;
      wine.ReStart();

      cin >> command;
      toupper(command);
      while (command != "DONE") {
        ss.clear();
        ss << command;
        ss >> m.x >> dot >> m.y >> dot >> c;
        if (m.x < 0 || m.x >= size || m.y < 0 || m.y >= size
          || wine.cell[m.x + 4][m.y + 4].piece != Empty) {
          cout << "ERROR" << endl;
        } else {
          wine.TurnMove(m);
        }
        cin >> command;
        toupper(command);
      }
      best = wine.TurnBest();
      wine.TurnMove(best);
      cout << best.x << "," << best.y << endl;
    } else if (command == "INFO") {
      int value;
      string key;
      cin >> key;
      toupper(key);

      if (key == "TIMEOUT_TURN") {
        cin >> value;
        if (value != 0)
          wine.timeout_turn = value;

      } else if (key == "TIMEOUT_MATCH") {
        cin >> value;
        if (value != 0)
          wine.timeout_match = value;

      } else if (key == "TIMEOUT_LEFT") {
        cin >> value;
        if (value != 0)
          wine.time_left = value;

      } else if (key == "MAX_MEMORY") {
        cin >> value;
        // TODO
      } else if (key == "GAME_TYPE") {
        cin >> value;
        // TODO
      } else if (key == "RULE") {
        cin >> value;
        // TODO
      } else if (key == "FOLDER") {
        string t;
        cin >> t;
      }
    } else if (command == "END") {
      exit(0);

    }
  }
}
int main() {
  gomocup();
}
