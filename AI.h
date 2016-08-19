#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  // 选点棋型分值
  int MeVal[6] = { 0, 4, 10, 10, 24, 24 };
  int YouVal[6] = { 0, 2, 5, 5, 12, 12 };
  // 局势棋型分值
  int Tval[6] = { 0, 1, 6, 6, 36, 40 };
  
public:
  int total = 0;                  //搜索局面数
  int BestVal = 0;                //最佳点分值
  int MaxDepth = 0;               //实际搜索的最大深度
  int SearchDepth = 14;           //搜索深度上限
  int time_left = 10000000;
  int timeout_turn = 10000000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  bool stopThink = false;
  clock_t start;
  Pos BestMove;

  Pos gobang();
  void sort(Point * a, int n);
  void AllType(int role, int *type);
  int GetTime();
  int StopTime();
  int ScoreMove(int x, int y);
  int minimax(int depth, int alpha, int beta);
  int AlphaBeta(int depth, int alpha, int beta);
  int CutCand(Pos * move, Point * cand, int Csize);
  int GetMove(Pos * move, int MaxMove);
  int evaluate();
  int evaluate2();
  bool Same(Pos a, Pos b);
};
#endif
