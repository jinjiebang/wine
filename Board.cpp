#include "Board.h"
#include <cstring>
#include <iostream>
using namespace std;
Board::Board() {
  InitType();
  memset(cell, 0, sizeof(cell));
  memset(IsCand, 0, sizeof(IsCand));
  memset(remMove, 0, sizeof(remMove));
}
Board::~Board() {

}
// 下子
void Board::MakeMove(Pos next) {
  int x = next.x;
  int y = next.y;

  ++step;
  cell[x][y].piece = color(step);
  remMove[step] = next;
  UpdateRound(2);
  UpdateType(x, y);
}

// 删子
void Board::DelMove() {
  int x = remMove[step].x;
  int y = remMove[step].y;

  --step;
  cell[x][y].piece = 0;
  UpdateType(x, y);
}

// 悔棋
void Board::Undo() {
  if (step >= 2) {
    DelMove();
    DelMove();
  }
}

// 重新开始
void Board::ReStart() {
  while (step) {
    DelMove();
  }
}

  // 检查坐标
bool Board::CheckXy(int x, int y) {
  return (x >= 0 && x < size && y >= 0 && y < size);
}

// 棋子颜色
int Board::color(int step) {
  return (step % 2) ? 1 : 2;
}

// 检查胜利
bool Board::CheckWin() {
  int role = color(step);
  Pos p = remMove[step];
  Cell *c = &cell[p.x][p.y];

  return (c->pattern[role - 1][0] == win
          || c->pattern[role - 1][1] == win
          || c->pattern[role - 1][2] == win 
          || c->pattern[role - 1][3] == win);
}
//判断棋型
bool Board::IsType(Pos p, int role, int type) {
  Cell *c = &cell[p.x][p.y];
  return (c->pattern[role - 1][0] == type
          || c->pattern[role - 1][1] == type
          || c->pattern[role - 1][2] == type
          || c->pattern[role - 1][3] == type);
}

// 更新点(x,y)周围位置的棋型
void Board::UpdateType(int x, int y) {
  int a, b;
  Cell *c;
  for (int i = 0; i < 4; ++i) {
    a = x + dx[i];
    b = y + dy[i];
    for (int j = 0; j < 4 && CheckXy(a, b); a += dx[i], b += dy[i], ++j) {
      c = &cell[a][b];
      c->pattern[0][i] = TypeLine(1, a, b, dx[i], dy[i]);
      c->pattern[1][i] = TypeLine(2, a, b, dx[i], dy[i]);
    }
    a = x - dx[i];
    b = y - dy[i];
    for (int k = 0; k < 4 && CheckXy(a, b); a -= dx[i], b -= dy[i], ++k) {
      c = &cell[a][b];
      c->pattern[0][i] = TypeLine(1, a, b, dx[i], dy[i]);
      c->pattern[1][i] = TypeLine(2, a, b, dx[i], dy[i]);
    }
  }
}

// 更新合理着法
void Board::UpdateRound(int n) {
  memset(IsCand, false, sizeof(IsCand));
  int x, y;
  for (int k = 1; k <= step; ++k) {
    x = remMove[k].x + n;
    y = remMove[k].y + n;
    for (int i = x - 2 * n; i <= x; ++i) {
      for (int j = y - 2 * n; j <= y; ++j) {
        if (!CheckXy(i, j))
          continue;
        IsCand[i][j] = true;
      }
    }
  }
}

// 棋型判断
int Board::TypeLine(int role, int x, int y, int i, int j) {
  int kong = 0, block = 0;
  int len = 1, len2 = 1, count = 1;
  int a, b, k;
  Cell *c;
  a = x + i;
  b = y + j;
  for (k = 0; k < 4; a += i, b += j, ++k) {
    if (!CheckXy(a, b)) {
      if (len2 == kong + count)
        ++block;
      break;
    }
    c = &cell[a][b];
    if (c->piece == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (c->piece == 0) {
      ++len;
      ++kong;
    } else {
      if (len2 == kong + count)
        ++block;
      break;
    }
  }
  // 计算中间空格
  kong = len2 - count;
  a = x - i;
  b = y - j;
  for (k = 0; k < 4; a -= i, b -= j, ++k) {
    if (!CheckXy(a, b)) {
      if (len2 == kong + count)
        ++block;
      break;
    }
    c = &cell[a][b];
    if (c->piece == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (c->piece == 0) {
      ++len;
      ++kong;
    } else {
      if (len2 == kong + count)
        ++block;
      break;
    }
  }
  return typeTable[len][len2][count][block];
}

// 棋型计数
void Board::TypeCount(int x, int y, int role, int *type) {
  int d[4];
  Cell *c = &cell[x][y];
  // 四个方向
  d[0] = c->pattern[role - 1][0];
  d[1] = c->pattern[role - 1][1];
  d[2] = c->pattern[role - 1][2];
  d[3] = c->pattern[role - 1][3];
  // 记录棋型
  ++type[d[0]];
  ++type[d[1]];
  ++type[d[2]];
  ++type[d[3]];

}
int Board::GetType(int len, int len2, int count, int block) {
  if (len >= 5 && count > 1) {
    if (count == 5)
      return win;
    if (len > 5 && len2 < 5 && block == 0) {
      switch (count) {
      case 2:
        return flex2;
      case 3:
        return flex3;
      case 4:
        return flex4;
      }
    } else {
      switch (count) {
      case 2:
        return block2;
      case 3:
        return block3;
      case 4:
        return block4;
      }
    }
  }
  return 0;
}
void Board::InitType() {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 6; ++j) {
      for (int k = 0; k < 6; ++k) {
        for (int l = 0; l < 3; ++l) {
          typeTable[i][j][k][l] = GetType(i, j, k, l);
        }
      }
    }
  }
}