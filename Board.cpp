
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


// �������̳ߴ�ͱ߽�
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

// ����
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

// ɾ��
void Board::DelMove() {
  int x = remMove[step].x;
  int y = remMove[step].y;

  zobristKey ^= zobrist[step & 1][x][y];
  --step;
  cell[x][y].piece = Empty;
  UpdateType(x, y);
}

// ����
void Board::Undo() {
  if (step >= 2) {
    DelMove();
    DelMove();
  }
}

// ���¿�ʼ
void Board::ReStart() {
  zobristKey = 0;
  memset(hashTable, 0, sizeof(hashTable));
  while (step) {
    DelMove();
  }
}

// �жϽ�ɫrole�ڵ�p�ܷ������type
bool Board::IsType(Pos p, int role, int type) {
  Cell *c = &cell[p.x][p.y];
  return c->pattern[role][0] == type
    || c->pattern[role][1] == type
    || c->pattern[role][2] == type || c->pattern[role][3] == type;
}

// ���µ�(x,y)��Χλ�õ�����
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

// ���º����ŷ�
void Board::UpdateRound(int n) {
  memset(IsCand, false, sizeof(IsCand));
  int x, y;
  int Rx, Ry;

  for (int k = 1; k <= step; ++k) {
    x = remMove[k].x;
    y = remMove[k].y;
    // �߽�����
    Rx = x + n;
    Ry = y + n;
    // ����n�����ڵĿյ�Ϊ�����ŷ�
    for (int i = x - n; i <= Rx; ++i) {
      for (int j = y - n; j <= Ry; ++j) {
        IsCand[i][j] = true;
      }
    }
  }
}

// ��ȡ����key
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

// �ж�key�����ͣ�����������ͱ�
int Board::LineType(int role, int key) {
  int line_left[8], line_right[8];
  for (int i = 0; i < 8; i++) {
    line_left[i] = key & 3;
    line_right[7 - i] = key & 3;
    key >>= 2;
  }
  // ˫���жϣ�ȡ��������
  int p1 = ShortLine(role, line_left);
  int p2 = ShortLine(role, line_right);
  return p1 > p2 ? p1 : p2;
}

// �жϵ����������
int Board::ShortLine(int role, int *line) {
  int kong = 0, block = 0;
  int len = 1, len2 = 1, count = 1;
  int k;

  for (k = 4; k <= 7; k++) {
    if (line[k] == role) {
      if (kong + count > 4) {
        break;
      }
      ++count;
      ++len;
      len2 = kong + count;
    } else if (line[k] == Empty) {
      ++len;
      ++kong;
    } else {
      if (len2 == kong + count) {
        ++block;
      }
      break;
    }
  }
  // �����м�ո�
  kong = len2 - count;
  for (k = 3; k >= 0; k--) {
    if (line[k] == role) {
      if (kong + count > 4) {
        break;
      }
      ++count;
      ++len;
      len2 = kong + count;
    } else if (line[k] == Empty) {
      ++len;
      ++kong;
    } else {
      if (len2 == kong + count) {
        ++block;
      }
      break;
    }
  }
  return typeTable[len][len2][count][block];
}

// ��ɳ����ͱ���Ϣ
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

// �����ͱ�ĳ�ʼ��
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

// �������ͱ�ĳ�ʼ��
void Board::InitPattern() {
  for (int key = 0; key < 65536; key++) {
    patternTable[key][0] = LineType(0, key);
    patternTable[key][1] = LineType(1, key);
  }
}