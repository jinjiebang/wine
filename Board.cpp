#include "Board.h"
#include <cstring>
#include <iostream>
  using namespace std;

Board::Board() {
  InitType();
  memset(cell, 0, sizeof(cell));
  memset(IsLose, 0, sizeof(IsLose));
  memset(IsCand, 0, sizeof(IsCand));
  memset(remMove, 0, sizeof(remMove));
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      cell[i][j].piece = Empty;
    }
  }
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
  cell[x][y].piece = Empty;
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

  // 检查胜利
bool Board::CheckWin() {
  int role = color(step);
  Pos p = remMove[step];
  Cell *c = &cell[p.x][p.y];

  return (c->pattern[role][0] == win
          || c->pattern[role][1] == win
          || c->pattern[role][2] == win 
          || c->pattern[role][3] == win);
}

// 判断角色role在点p能否成棋型type
bool Board::IsType(Pos p, int role, int type) {
  Cell *c = &cell[p.x][p.y];
  return (c->pattern[role][0] == type
          || c->pattern[role][1] == type
          || c->pattern[role][2] == type 
          || c->pattern[role][3] == type);
}

// 更新点(x,y)周围位置的棋型
void Board::UpdateType(int x, int y) {
  int a, b;
  int p1, p2;
  Cell *c;
  for (int i = 0; i < 4; ++i) {
    a = x + dx[i];
    b = y + dy[i];
    for (int j = 0; j < 4 && CheckXy(a, b); a += dx[i], b += dy[i], ++j) {
      c = &cell[a][b];
      // 更新白棋棋型
      p1 = TypeLine(0, a, b, dx[i], dy[i]);
      p2 = TypeLine(0, a, b, -dx[i], -dy[i]);
      c->pattern[0][i] = (p1 > p2) ? p1: p2;
      // 更新黑棋棋型
      p1 = TypeLine(1, a, b, dx[i], dy[i]);
      p2 = TypeLine(1 , a, b, -dx[i], -dy[i]);
      c->pattern[1][i] = (p1 > p2) ? p1: p2;
    }
    a = x - dx[i];
    b = y - dy[i];
    for (int k = 0; k < 4 && CheckXy(a, b); a -= dx[i], b -= dy[i], ++k) {
      c = &cell[a][b];
      // 更新白棋棋型
      p1 = TypeLine(0, a, b, dx[i], dy[i]);
      p2 = TypeLine(0, a, b, -dx[i], -dy[i]);
      c->pattern[0][i] = (p1 > p2) ? p1: p2;
      // 更新黑棋棋型
      p1 = TypeLine(1, a, b, dx[i], dy[i]);
      p2 = TypeLine(1 , a, b, -dx[i], -dy[i]);
      c->pattern[1][i] = (p1 > p2) ? p1: p2;
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
    for (int i = x - (n << 1); i <= x; ++i) {
      for (int j = y - (n << 1); j <= y; ++j) {
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
  a = x + i;
  b = y + j;
  for (k = 0; k < 4; a += i, b += j, ++k) {
    if (!CheckXy(a, b)) {
      if (len2 == kong + count)
        ++block;
      break;
    }
    if (cell[a][b].piece == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (cell[a][b].piece == Empty) {
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
    if (cell[a][b].piece == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (cell[a][b].piece == Empty) {
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
  d[0] = c->pattern[role][0];
  d[1] = c->pattern[role][1];
  d[2] = c->pattern[role][2];
  d[3] = c->pattern[role][3];
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
