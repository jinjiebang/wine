
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

// 棋子
const int Outside = 3;
const int Empty = 2;
const int Black = 1;
const int White = 0;
// 方向向量
const int dx[4] = { 1, 0, 1, 1 };
const int dy[4] = { 0, 1, 1, -1 };

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
  int step = 0;                                     // 记录棋局步数
  int size = 15;                                    // 棋盘当前尺寸
  int b_start, b_end;                               // 棋盘遍历的索引
  int typeTable[10][6][6][3];                       // 初级棋型表
  int patternTable[65536][2];                       // 完整棋型表
  Cell cell[MaxSize + 8][MaxSize + 8];              // 棋盘结构，记录棋子颜色和棋型
  Pos remMove[MaxSize * MaxSize];                   // 记录每步棋的坐标
  bool IsCand[MaxSize + 8][MaxSize + 8];            // 记录每个位置是否合理着法（两格内有棋子）
  bool IsLose[51];                                  // 记录根节点的必败点

    Board();
   ~Board();
  void InitType();
  void InitPattern();
  void SetSize(int _size);
  void MakeMove(Pos next);
  void DelMove();
  void Undo();
  void ReStart();
  void UpdateRound(int n);
  void UpdateType(int x, int y);
  bool IsType(Pos p, int role, int type);
  int GetKey(int x, int y, int i);
  int LineType(int role, int key);
  int ShortLine(int role, int *line);
  int GetType(int len, int len2, int count, int block);

  /* 以下为可内联函数 */
  int color(int step) {
    return step & 1;
  } 
  bool CheckXy(int x, int y) {
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
      || c->pattern[role][2] == win 
      || c->pattern[role][3] == win;
  }

};
#endif
