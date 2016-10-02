
#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  // 选点棋型分值
  int MeVal[6] = { 0, 4, 10, 10, 24, 30 };
  int YouVal[6] = { 0, 2, 5, 5, 12, 15 };
  // 局势棋型分值
  int Cval[8] = { 0, 3, 18, 27, 144, 216, 1200, 1800 };
  int Hval[8] = { 0, 2, 12, 18, 96, 144, 800, 1200 };
public:
  int total = 0;                // 搜索局面数
  int BestVal = 0;              // 最佳点分值
  int MaxDepth = 0;             // 实际搜索的最大深度
  int SearchDepth = 14;         // 搜索深度上限
  int time_left = 10000000;
  int timeout_turn = 500000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  bool stopThink = false;
  clock_t start;
  Pos BestMove;

  Pos gobang();
  Pos TurnBest();
  void sort(Point * a, int n);
  void TurnMove(Pos next);
  int GetTime();
  int StopTime();
  int ScoreMove(int x, int y);
  int minimax(int depth, int alpha, int beta);
  int AlphaBeta(int depth, int alpha, int beta);
  int CutCand(Pos * move, Point * cand, int Csize);
  int GetMove(Pos * move, int MaxMove);
  int evaluate();
  bool Same(Pos a, Pos b);
};
#endif
