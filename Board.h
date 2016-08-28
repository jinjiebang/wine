
#ifndef  _BOARD
#define  _BOARD
typedef unsigned long long U64;
const int win = 7;              // ���
const int flex4 = 6;            // ����
const int block4 = 5;           // ����
const int flex3 = 4;            // ����
const int block3 = 3;           // ����
const int flex2 = 2;            // ���
const int block2 = 1;           // �߶�
const int Ntype = 8;            // �������
const int MaxSize = 20;         // ���ߴ�
const int hashSize = 1 << 16;

// hash��־
const int hash_exact = 0;
const int hash_alpha = 1;
const int hash_beta = 2;
const int unknown = -20000;

// ����
const int Outside = 3;
const int Empty = 2;
const int Black = 1;
const int White = 0;
// �������
const int dx[4] = { 1, 0, 1, 1 };
const int dy[4] = { 0, 1, 1, -1 };

// ���
struct Pos {
  int x;
  int y;
};

// �ŷ�
struct Point {
  Pos p;
  int val;
};

// ����״̬
struct Cell {
  int piece;
  int pattern[2][4];
};

struct Hashe {
  U64 key;
  int depth;
  int hashf;
  int val;
  Pos best;
};

class Board {
public:
  int step = 0;                 // ��¼��ֲ���
  int size = 15;                // ���̵�ǰ�ߴ�
  int b_start, b_end;           // ���̱��������
  U64 zobristKey = 0;
  U64 zobrist[2][MaxSize + 4][MaxSize + 4];
  Hashe hashTable[hashSize];
  int typeTable[10][6][6][3];   // �����ͱ�
  int patternTable[65536][2];   // �������ͱ�
  Cell cell[MaxSize + 8][MaxSize + 8]; // ���̽ṹ����¼������ɫ������
  Pos remMove[MaxSize * MaxSize]; // ��¼ÿ��������
  bool IsCand[MaxSize + 8][MaxSize + 8]; // ��¼ÿ��λ���Ƿ�����ŷ�������������ӣ�
  bool IsLose[25];              // ��¼��ڵ�ıذܵ�

    Board();
   ~Board();
  void InitType();
  void InitPattern();
  void InitZobrist();
  void SetSize(int _size);
  void MakeMove(Pos next);
  void DelMove();
  void Undo();
  void ReStart();
  void UpdateRound(int n);
  void UpdateType(int x, int y);
  bool IsType(Pos p, int role, int type);
  U64 Rand64();
  int GetKey(int x, int y, int i);
  int LineType(int role, int key);
  int ShortLine(int role, int *line);
  int GetType(int len, int len2, int count, int block);

  /* ����Ϊ��������� */
  int color(int step) {
    return step & 1;
  } bool CheckXy(int x, int y) {
    return cell[x][y].piece != Outside;
  }

  void TypeCount(int x, int y, int role, int *type) {
    ++type[cell[x][y].pattern[role][0]];
    ++type[cell[x][y].pattern[role][1]];
    ++type[cell[x][y].pattern[role][2]];
    ++type[cell[x][y].pattern[role][3]];
  }

  bool CheckWin() {
    int role = color(step);
    Cell *c = &cell[remMove[step].x][remMove[step].y];

    return c->pattern[role][0] == win
      || c->pattern[role][1] == win
      || c->pattern[role][2] == win || c->pattern[role][3] == win;
  }

};
#endif