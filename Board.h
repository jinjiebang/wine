#ifndef  _BOARD
#define  _BOARD
const int win = 7;              // 连五
const int flex4 = 6;            // 活四
const int block4 = 5;           // 冲四
const int flex3 = 4;            // 活三
const int block3 = 3;           // 眠三
const int flex2 = 2;            // 活二
const int block2 = 1;           // 眠二
const int Ntype = 8;            // 棋型数量
const int MaxSize = 20;         // 最大尺寸

const int Empty=2;
const int Black=1;
const int White=0;


// 坐标
struct Pos {
  int x;
  int y;
};

// 着法
struct Point {
  Pos p;
  int val;
};

// 棋子状态
struct Cell {
  int piece;
  int pattern[2][4];
};

class Board {
public:
  
  int step = 0;
  int size = 15;
  const int dx[4] = { 1, 0, 1, 1 };
  const int dy[4] = { 0, 1, 1, -1 };
  int typeTable[10][6][6][3];
  Cell cell[MaxSize][MaxSize];
  Pos remMove[MaxSize * MaxSize];
  bool IsCand[MaxSize][MaxSize];
  bool IsLose[MaxSize][MaxSize];

  Board();
  ~Board();
  void InitType();
  void MakeMove(Pos next);
  void DelMove();
  void Undo();
  void ReStart();
  void UpdateType(int x, int y);
  void UpdateRound(int n);
  void TypeCount(int x, int y, int role, int *type);
  bool CheckWin();
  bool IsType(Pos p, int role, int type);
  bool CheckXy(int x, int y) {
    return (x >= 0 && x < size && y >= 0 && y < size);
  }
  int color(int step) {
    return step%2;
  }
  int TypeLine(int role, int x, int y, int i, int j);
  int GetType(int len, int len2, int count, int block);
};
#endif