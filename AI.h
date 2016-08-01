#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  int MeVal[6] = { 0, 4, 10, 10, 24, 24 };
  int YouVal[6] = { 0, 2, 5, 5, 12, 12 };
  int Tval[6] = { 0, 1, 6, 6, 36, 36 };

public:
  int total = 0;
  int BestVal = 0;
  int _BestVal = 0;
  int MaxDepth = 0;
  int SearchDepth = 12;
  int time_left = 10000000;
  int timeout_turn = 1000000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  bool stopThink = false;
  clock_t start;
  Pos BestMove;
  Pos _BestMove;

  Pos gobang();
  void sort(Point * a, int n);
  void AllType(int role, int *type);
  int GetTime();
  int StopTime();
  int ScoreMove(int x, int y);
  int minimax(int depth, int alpha, int beta);
  int AlphaBeta(int depth, int alpha, int beta);
  int CutCand(Pos * moves, Point * cand, int Csize,int branch);
  int GetMove(Pos * moves, int branch);
  int evaluate();
  int evaluate2();
};
#endif
