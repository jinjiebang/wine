#ifndef  _AI
#define   _AI
#include "Board.h"
#include "Board.cpp"
#include<time.h>
class AI:public Board {
private:
  int w1[6] = { 0, 4, 10, 10, 24, 24 };
  int w2[6] = { 0, 2, 5, 5, 12, 12 };
  int w3[6] = { 0, 4, 10, 10, 24, 24 };
  int w4[6] = { 0, 2, 5, 5, 12, 12 };
  int Tval[6] = { 0, 1, 6 , 6, 36, 36 };

public:
  int total = 0;
  int remStep = 0;
  int BestVal = 0;
  int MaxDepth = 0;
  int SearchDepth = 10;
  clock_t start;
  int ThinkTime = 0;
  int timeout_turn=30000;
  int timeout_match=180000;
  int time_left=10000000;
  Pos BestMove;

  Pos gobang();
  void sort(Point * a, int n);
  void AllType(int role, int *type);
  int ScoreMove(int x, int y, int wNum);
  int ScoreChess(int *type);
  int minimax(int depth, int alpha, int beta);
  int AlphaBeta(int depth, int alpha, int beta);
  int CutCand(Pos * moves, Point * cand, int Csize);
  int GetMove(Pos * moves, int branch);
  int evaluate();
  int evaluate2();
};
#endif
