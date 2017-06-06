
#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  // 局面评价分值
  int whoVal[8] = { 0, 3, 18, 27, 144, 216, 1200, 1800 };
  int oppVal[8] = { 0, 2, 12, 18, 96, 144, 800, 1200 };
  // 不区分下子方的评价分值
  int eval[8] = { 0, 2, 8, 10, 32, 40, 128, 140};

public:
  int total = 0;                // 搜索局面数
  int hashCount = 0;            // hash表命中次数
  int searchDepth = 0;          // 实际搜索深度
  int time_left = 10000000;
  int timeout_turn = 5000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  Point bestPoint;
  Line bestLine;
  clock_t start;
  bool stopThink = false;
  int Npoint=0;

  Pos gobang();
  Pos TurnBest();
  void sort(Point * a, int n);
  void TurnMove(Pos next);
  void RecordHash(int depth, int val, int hashf);
  void RecordPVS(Pos best);
  Pos MoveNext(MoveList &moveList);
  int ProbeHash(int depth, int alpha, int beta);
  int GetTime();
  int StopTime();
  int ScoreMove(Cell * c);
  Point minimax(int depth, int alpha, int beta, Line *pline);
  int AlphaBeta(int depth, int alpha, int beta, Line *pline);
  int CutCand(Pos * move, Point * cand, int Csize);
  int GetMove(Pos * move);
  int evaluate();

};
#endif
