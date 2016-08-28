
#ifndef  _AI
#define   _AI
#include "Board.h"
#include <ctime>
class AI:public Board {
private:
  // ѡ�����ͷ�ֵ
  int MeVal[6] = { 0, 4, 10, 10, 24, 24 };
  int YouVal[6] = { 0, 2, 5, 5, 12, 12 };
  // �������ͷ�ֵ
  int Tval[8] = { 0, 1, 6, 9, 48, 72, 400, 600 };

public:
  int total = 0;                // ���������
  int hashCount = 0;
  int BestVal = 0;              // ��ѵ��ֵ
  int MaxDepth = 0;             // ʵ�������������
  int SearchDepth = 14;         // �����������
  int time_left = 10000000;
  int timeout_turn = 10000000;
  int timeout_match = 10000000;
  int ThinkTime = 0;
  bool stopThink = false;
  bool getHashMove = false;
  Pos hashMove;
  Pos curBest;
  clock_t start;
  Pos BestMove;

  Pos gobang();
  Pos TurnBest();
  void sort(Point * a, int n);
  void TurnMove(Pos next);
  void RecordHash(int depth, int val, int hashf);
  int ProbeHash(int depth, int alpha, int beta);
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