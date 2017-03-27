
#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  // 局面评价分值
  int Cval[8] = { 0, 3, 18, 27, 144, 216, 1200, 1800 };
  int Hval[8] = { 0, 2, 12, 18, 96, 144, 800, 1200 };

public:
  int total = 0;                // 搜索局面数
  int hashCount = 0;            // hash表命中次数
  int MaxDepth = 0;             // 实际搜索深度
  int time_left = 10000000;
  int timeout_turn = 30000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  int bestIndex;
  Point bestPoint;
  bool stopThink = false;
  clock_t start;

  Pos gobang();
  Pos TurnBest();
  void sort(Point * a, int n);
  void TurnMove(Pos next);
  void RecordHash(int depth, int val, int hashf);
  int ProbeHash(int depth, int alpha, int beta);
  int GetTime();
  int StopTime();
  int ScoreMove(Cell * c);
  Point minimax(int depth, int alpha, int beta);
  int AlphaBeta(int depth, int alpha, int beta);
  int CutCand(Pos * move, Point * cand, int Csize);
  int GetMove(Pos * move);
  int evaluate();
  bool Same(Pos a, Pos b);

};
#endif
