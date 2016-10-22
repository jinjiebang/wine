
#include "Board.h"
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <iostream>
  using namespace std;

Board::Board() {
  InitType();
  InitPattern();
  InitZobrist();
  memset(cell, 0, sizeof(cell));
  memset(IsLose, 0, sizeof(IsLose));
  memset(IsCand, 0, sizeof(IsCand));
  memset(remMove, 0, sizeof(remMove));
  memset(hashTable, 0, sizeof(hashTable));
  SetSize(15);
}

Board::~Board() {

}

U64 Board::Rand64() {
  return rand() ^ ((U64) rand() << 15) ^ ((U64) rand() << 30) ^ ((U64) rand() << 45) ^ ((U64) rand() << 60);
}

void Board::InitZobrist() {
  srand(time(NULL));
  for (int i = 0; i < MaxSize + 4; i++) {
    for (int j = 0; j < MaxSize + 4; j++) {
      zobrist[0][i][j] = Rand64();
      zobrist[1][i][j] = Rand64();
    }
  }
}


// 设置棋盘尺寸和边界
void Board::SetSize(int _size) {
  size = _size;
  b_start = 4, b_end = size + 4;
  for (int i = 0; i < MaxSize + 8; i++) {
    for (int j = 0; j < MaxSize + 8; j++) {
      if (i < 4 || i >= size + 4 || j < 4 || j >= size + 4) {
        cell[i][j].piece = Outside;
      } else {
        cell[i][j].piece = Empty;
      }
    }
  }
}

// 下子
void Board::MakeMove(Pos next) {
  int x = next.x;
  int y = next.y;

  ++step;
  cell[x][y].piece = color(step);
  zobristKey ^= zobrist[step & 1][x][y];
  remMove[step] = next;
  UpdateRound(2);
  UpdateType(x, y);
}

// 删子
void Board::DelMove() {
  int x = remMove[step].x;
  int y = remMove[step].y;

  zobristKey ^= zobrist[step & 1][x][y];
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
  zobristKey = 0;
  memset(hashTable, 0, sizeof(hashTable));
  while (step) {
    DelMove();
  }
}

// 判断角色role在点p是否能成棋型type
bool Board::IsType(Pos p, int role, int type) {
  Cell *c = &cell[p.x][p.y];
  return c->pattern[role][0] == type
    || c->pattern[role][1] == type
    || c->pattern[role][2] == type 
    || c->pattern[role][3] == type;
}

// 更新(x,y)附近棋型
void Board::UpdateType(int x, int y) {
  int a, b;
  int key;

  for (int i = 0; i < 4; ++i) {
    a = x + dx[i];
    b = y + dy[i];
    for (int j = 0; j < 4 && CheckXy(a, b); a += dx[i], b += dy[i], ++j) {
      key = GetKey(a, b, i);
      cell[a][b].pattern[0][i] = patternTable[key][0];
      cell[a][b].pattern[1][i] = patternTable[key][1];
    }
    a = x - dx[i];
    b = y - dy[i];
    for (int k = 0; k < 4 && CheckXy(a, b); a -= dx[i], b -= dy[i], ++k) {
      key = GetKey(a, b, i);
      cell[a][b].pattern[0][i] = patternTable[key][0];
      cell[a][b].pattern[1][i] = patternTable[key][1];
    }
  }
}

// 更新合理着法
void Board::UpdateRound(int n) {
  memset(IsCand, false, sizeof(IsCand));
  int x, y;
  int Rx, Ry;

  for (int k = 1; k <= step; ++k) {
    x = remMove[k].x;
    y = remMove[k].y;
    Rx = x + n;
    Ry = y + n;
    // 设置n格以内有棋子的点为合理着法
    for (int i = x - n; i <= Rx; ++i) {
      for (int j = y - n; j <= Ry; ++j) {
        IsCand[i][j] = true;
      }
    }
  }
}

// 获取i线上的key
int Board::GetKey(int x, int y, int i) {
  int key = 0;
  int a = x - dx[i] * 4;
  int b = y - dy[i] * 4;
  for (int k = 0; k < 9; a += dx[i], b += dy[i], k++) {
    if (k == 4) {
      continue;
    }
    key <<= 2;
    key ^= cell[a][b].piece;
  }
  return key;
}

// 判断key的棋型，用于填充棋型表
int Board::LineType(int role, int key) {
  int line_left[9];
  int line_right[9];
  int i;

  for (i = 0; i < 9; i++) {
    if (i == 4) {
      line_left[i] = role;
      line_right[i] = role;
    } else {
      line_left[i] = key & 3;
      line_right[8 - i] = key & 3;
      key >>= 2;
    }
  }

  // 双向判断，取最大的棋型
  int p1 = ShortLine(line_left);
  int p2 = ShortLine(line_right);

  // 同线双四,双三特判
  if (p1 == block3 && p2 == block3)
    return CheckFlex3(line_left);

  if (p1 == block4 && p2 == block4)
    return CheckFlex4(line_left);

  return p1 > p2 ? p1 : p2;
}

// 同线双三特判
int Board::CheckFlex3(int *line) {
  int role = line[4];
  int type;
  for (int i = 0; i < 9; i++) {
    if (line[i] == Empty) {
      line[i] = role;
      type = CheckFlex4(line);
      line[i] = Empty;
      if (type == flex4)
        return flex3;
    }
  }
  return block3;
}

// 同线双四特判
int Board::CheckFlex4(int *line) {
  int i, j, count;

  int five = 0;
  int role = line[4];
  for (i = 0; i < 9; i++) {
    if (line[i] == Empty) {
      count = 0;
      for (j = i - 1; j >= 0 && line[j] == role; j--)
        count++;
      for (j = i + 1; j <= 8 && line[j] == role; j++)
        count++;
      if (count >= 4)
        five++;
    }
  }
  return five >= 2 ? flex4 : block4;
}

// 判断单个方向的棋型
int Board::ShortLine(int *line) {
  int kong = 0, block = 0;
  int len = 1, len2 = 1, count = 1;
  int k;

  int role = line[4];
  for (k = 5; k <= 8; k++) {
    if (line[k] == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (line[k] == Empty) {
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
  for (k = 3; k >= 0; k--) {
    if (line[k] == role) {
      if (kong + count > 4)
        break;
      ++count;
      ++len;
      len2 = kong + count;
    } else if (line[k] == Empty) {
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
// 生成初级棋型表
int Board::GetType(int len, int len2, int count, int block) {
  if (len >= 5 && count > 1) {
    if (count == 5) {
      return win;
    }
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

// 初始化初级棋型表
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

// 初始化完整棋型表
void Board::InitPattern() {
  for (int key = 0; key < 65536; key++) {
    patternTable[key][0] = LineType(0, key);
    patternTable[key][1] = LineType(1, key);
  }
}
