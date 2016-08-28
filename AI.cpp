
#include "AI.h"
#include<ctime>
#include<cstring>
#include<cstdlib>
#include<iostream>
  using namespace std;

int AI::GetTime() {
  return (double)(clock() - start) / CLOCKS_PER_SEC * 1000;
}

int AI::StopTime() {
  return (timeout_turn < time_left / 7) ? timeout_turn : time_left / 7;
}

int AI::ProbeHash(int depth, int alpha, int beta) {
  Hashe *phashe = &hashTable[zobristKey % hashSize];
  if (phashe->key == zobristKey) {
    if (phashe->depth >= depth) {
      if (phashe->hashf == hash_exact) {
        return phashe->val;
      }
      if ((phashe->hashf == hash_alpha) && (phashe->val <= alpha)) {
        return alpha;
      }
      if ((phashe->hashf == hash_beta) && (phashe->val >= beta)) {
        return beta;
      }
    }
    if (phashe->best.x != -1) {
      getHashMove = true;
      hashMove = phashe->best;
    }
  }
  return unknown;
}

void AI::RecordHash(int depth, int val, int hashf) {
  Hashe *phashe = &hashTable[zobristKey % hashSize];
  phashe->key = zobristKey;
  phashe->best = curBest;
  phashe->val = val;
  phashe->hashf = hashf;
  phashe->depth = depth;
}
// ��������
void AI::TurnMove(Pos next) {
  next.x += 4, next.y += 4;
  MakeMove(next);
}

// ������ѵ�
Pos AI::TurnBest() {
  Pos best = gobang();
  best.x -= 4, best.y -= 4;
  return best;
}

// ������ѵ�
Pos AI::gobang() {
  start = clock();
  total = 0;
  hashCount = 0;
  stopThink = false;

  // ��һ�������ĵ�
  if (step == 0) {
    BestMove.x = size / 2 + 4;
    BestMove.y = size / 2 + 4;
    return BestMove;
  }
  // �ڶ��������
  if (step == 1 || step == 2) {
    int rx, ry;
    int d = step * 2 + 1;
    srand(time(NULL));
    do {
      rx = rand() % d + remMove[1].x - step;
      ry = rand() % d + remMove[1].y - step;
    } while (!CheckXy(rx, ry) || cell[rx][ry].piece != Empty);
    BestMove.x = rx;
    BestMove.y = ry;
    return BestMove;
  }
  // ����������
  memset(IsLose, false, sizeof(IsLose));
  for (int i = 2; i <= SearchDepth; i += 2) {
    if (i > 4 && GetTime() * 12 >= StopTime()) {
      break;
    }
    MaxDepth = i;
    BestVal = minimax(i, -10001, 10000);
    if (BestVal == 10000) {
      break;
    }
  }

  ThinkTime = GetTime();
  cout << (double)hashCount / (total + 1) * 100 << endl;

  return BestMove;
}

inline bool AI::Same(Pos a, Pos b) {
  return a.x == b.x && a.y == b.y;
}

// max����
int AI::minimax(int depth, int alpha, int beta) {
  UpdateRound(2);
  Pos move[28];
  int val;
  int count = GetMove(move, 27);

  if (count == 1) {
    BestMove = move[1];
    return BestVal;
  }

  move[0] = (depth > 2) ? BestMove : move[1];

  // ���������߷�
  for (int i = 0; i <= count; i++) {
    if (i > 0 && Same(move[0], move[i])) {
      continue;
    }

    if (i > 0 && IsLose[i]) {
      continue;
    }

    MakeMove(move[i]);
    do {
      if (i > 0 && alpha + 1 < beta) {
        val = -AlphaBeta(depth - 1, -alpha - 1, -alpha);
        if (val <= alpha || val >= beta) {
          break;
        }
      }
      val = -AlphaBeta(depth - 1, -beta, -alpha);
    } while (0);
    DelMove();

    if (stopThink) {
      break;
    }

    if (val == -10000) {
      IsLose[i] = true;
    }

    if (val >= beta) {
      BestMove = move[i];
      return val;
    }
    if (val > alpha) {
      alpha = val;
      BestMove = move[i];
    }
  }
  return alpha;
}

// alpha-beta����
int AI::AlphaBeta(int depth, int alpha, int beta) {
  total++;
  getHashMove = false;
  curBest.x = -1;

  static int cnt = 1000;
  if (--cnt <= 0) {
    cnt = 1000;
    if (GetTime() + 500 >= StopTime()) {
      stopThink = true;
    }
  }
  // �Է����һ�����
  if (CheckWin()) {
    return -10000;
  }


  Pos move[28];
  int val;
  int hashf = hash_alpha;
  if ((val = ProbeHash(depth, alpha, beta)) != unknown) {
    hashCount++;
    return val;
  }
  // Ҷ�ڵ�
  if (depth == 0) {
    val = evaluate();
    RecordHash(depth, val, hash_exact);
    return val;
  }

  int count = GetMove(move, 27);
  int move_start = 1;
  if (getHashMove) {
    move_start = 0;
    move[0] = hashMove;
  }
  // ���������߷�
  for (int i = move_start; i <= count; i++) {

    MakeMove(move[i]);
    do {
      if (i > 1 && alpha + 1 < beta) {
        val = -AlphaBeta(depth - 1, -alpha - 1, -alpha);
        if (val <= alpha || val >= beta) {
          break;
        }
      }
      val = -AlphaBeta(depth - 1, -beta, -alpha);
    } while (0);
    DelMove();

    if (stopThink) {
      break;
    }

    if (val >= beta) {
      curBest = move[i];
      RecordHash(depth, beta, hash_beta);
      return val;
    }
    if (val > alpha) {
      curBest = move[i];
      hashf = hash_exact;
      alpha = val;
    }
  }
  RecordHash(depth, alpha, hashf);
  return alpha;
}

// ��ȫ��֦
int AI::CutCand(Pos * move, Point * cand, int Csize) {
  int me = color(step + 1);
  int you = !me;
  int Msize = 0;
  // ���ӷ��ܳ�������
  // �Է��ܳ���
  if (cand[1].val >= 2400) {
    move[1] = cand[1].p;
    Msize = 1;
  }
  // �Է��ܳɻ���
  else if (cand[1].val == 1200) {
    move[1] = cand[1].p;
    Msize = 1;
    if (cand[2].val == 1200) {
      move[2] = cand[2].p;
      Msize = 2;
    }
    for (int i = Msize + 1; i <= Csize; ++i) {
      if (IsType(cand[i].p, me, block4)
          || IsType(cand[i].p, you, block4)) {
        ++Msize;
        move[Msize] = cand[i].p;
      }
    }
  }
  return Msize;
}

// ��������ŷ��������ظ���
int AI::GetMove(Pos * move, int branch) {
  Point cand[200];
  int Csize = 0, Msize = 0;
  int val;
  for (int i = b_start; i < b_end; i++) {
    for (int j = b_start; j < b_end; j++) {
      if (IsCand[i][j] && cell[i][j].piece == Empty) {
        val = ScoreMove(i, j);
        if (val > 0) {
          ++Csize;
          cand[Csize].p.x = i;
          cand[Csize].p.y = j;
          cand[Csize].val = val;
        }
      }
    }
  }
  // �ŷ�����
  sort(cand, Csize);
  Csize = (Csize < branch) ? Csize : branch;
  // ���ͼ�֦
  Msize = CutCand(move, cand, Csize);

  // ���û�м�֦
  if (Msize == 0) {
    Msize = Csize;
    for (int k = 1; k <= Msize; ++k) {
      move[k] = cand[k].p;
    }
  }

  return Msize;
}

// ����
void AI::sort(Point * a, int n) {
  int i, j;
  Point key;
  for (i = 2; i <= n; i++) {
    key = a[i];
    for (j = i; j > 1 && a[j - 1].val < key.val; j--) {
      a[j] = a[j - 1];
    }
    a[j] = key;
  }
}

// ���͹�ֵ����
int AI::evaluate() {
  int Ctype[Ntype] = { 0 };
  int Htype[Ntype] = { 0 };
  int me = color(step + 1);
  int you = !me;

  for (int i = b_start; i < b_end; ++i) {
    for (int j = b_start; j < b_end; ++j) {
      if (IsCand[i][j] && cell[i][j].piece == Empty) {
        TypeCount(i, j, me, Ctype);
        TypeCount(i, j, you, Htype);
      }
    }
  }

  if (Ctype[win] > 0) {
    return 10000;
  }
  if (Htype[win] > 1) {
    return -10000;
  }
  if (Ctype[flex4] > 0 && Htype[win] == 0) {
    return 10000;
  }

  int Cscore = 0;
  int Hscore = 0;
  for (int i = 1; i < Ntype; ++i) {
    Cscore += Ctype[i] * Tval[i];
    Hscore += Htype[i] * Tval[i];
  }

  return Cscore * 3 - Hscore * 2;
}
// �ŷ����
int AI::ScoreMove(int x, int y) {
  int score = 0;
  int MeType[Ntype] = { 0 };
  int YouType[Ntype] = { 0 };
  int me = color(step + 1);

  TypeCount(x, y, me, MeType);
  TypeCount(x, y, !me, YouType);

  if (MeType[win] > 0) {
    return 10000;
  }
  if (YouType[win] > 0) {
    return 5000;
  }
  if (MeType[flex4] > 0 || MeType[block4] > 1) {
    return 2400;
  }
  if (MeType[block4] > 0 && MeType[flex3] > 0) {
    return 2000;
  }
  if (YouType[flex4] > 0 || YouType[block4] > 1) {
    return 1200;
  }
  if (YouType[block4] > 0 && YouType[flex3] > 0) {
    return 1000;
  }
  if (MeType[flex3] > 1) {
    return 400;
  }
  if (YouType[flex3] > 1) {
    return 200;
  }

  for (int i = 1; i <= block4; i++) {
    score += MeVal[i] * MeType[i];
    score += YouVal[i] * YouType[i];
  }

  return score;
}