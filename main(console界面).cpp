#include<iostream>
#include<cstdlib>
#include<sstream>
#include<string>
#include "AI.h"
using namespace std;
// 获取棋子坐标
void GetXy(Pos & p) {
  char x;
  int y;
  cin >> x >> y;
  p.x = x - 65;
  p.y = 15 - y;
}
// 显示棋子坐标
void ShowMove(Pos p) {
  char x = p.x + 65;
  int y = 15 - p.y;
  cout << x << y << " ";
}
// 显示最后一子棋型
void ShowShape(AI * ai) {
  int type[8] = { 0 };
  int role = ai->color(ai->step);
  Pos p = ai->remMove[ai->step];

  ai->TypeCount(p.x, p.y, role, type);
  for (int i = 1; i < 8; ++i) {
    if (type[i]) {
      cout << type[i] << "个";
      switch (i) {
      case block2:
        cout << "眠二";
        break;
      case flex2:
        cout << "活二";
        break;
      case block3:
        cout << "眠三";
        break;
      case flex3:
        cout << "活三";
        break;
      case block4:
        cout << "眠四";
        break;
      case flex4:
        cout << "活四";
        break;
      case win:
        cout << "连五";
        break;
      }
      cout << endl;
    }
  }
}
// 画棋盘
void DrawChess(AI * ai) {
  int size = ai->size;
  cout << "思考时间:" << ai->ThinkTime << endl;
  cout << "局势:" << ai->BestVal << endl;
  cout << "局面:" << ai->total << endl;
  for (int i = 0; i < size; i++) {
    if (size - i < 10)
      cout << " ";
    cout << size - i;
    for (int j = 0; j < size; j++) {
      switch (ai->cell[j][i].piece) {
      case 2:
        ai->IsLose[j][i] ? cout << " L" : cout << "+ ";
        break;
      case 1:
        cout << "● ";
        break;
      case 0:
        cout << "○ ";
        break;
      case 3:
        cout << "□ ";
        break;
      }
    }
    cout << endl;
  }
  cout << "  ";
  for (int i = 0; i < size; i++) {
    char c = 65 + i;
    cout << c << " ";
  }
  cout << endl;
  ShowShape(ai);
}

int gomocup() {
  AI wine;
  wine.SearchDepth = 8;
   string command;
  Pos input, best;
  char dot;
  int size;
  while (1) {
    DrawChess(&wine);
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
    if (command == "P") {
      GetXy(input);
      if (!wine.CheckXy(input.x, input.y)
          || wine.cell[input.x][input.y].piece != Empty) {
        cout << "ERROR" << endl;
      } else {
        wine.MakeMove(input);
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
      // cin >> input.x >> dot >> input.y;
      GetXy(input);
      if (!wine.CheckXy(input.x, input.y)
          || wine.cell[input.x][input.y].piece != Empty) {
        cout << "ERROR" << endl;
      } else {
        wine.MakeMove(input);
        best = wine.gobang();
        wine.MakeMove(best);
        cout << "MESSAGE";
        cout << " level=" << wine.MaxDepth;
        cout << " val=" << wine.BestVal;
        cout << " NPS=" << wine.total / (wine.ThinkTime + 1) << endl;
        cout << best.x << "," << best.y << endl;
      }
    } else if (command == "BOARD") {
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
  cout << "以 turn H8 这样的命令下棋\n";
  cout << "takeback 命令用于悔棋\n";
  cout << "restart 命令用于重新开始\n";
  cout << "begin 命令可让AI下第一步或计算最佳点\n";
  gomocup();
}