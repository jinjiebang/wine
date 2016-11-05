
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

// 查询置换表
int AI::ProbeHash(int depth, int alpha, int beta) {
  Hashe *phashe = &hashTable[zobristKey % hashSize];
  if (phashe->key == zobristKey) {
    if (phashe->depth >= depth) {
      switch (phashe->hashf) {
      case hash_exact:
        return phashe->val;
      case hash_alpha:
        if (phashe->val <= alpha) return phashe->val;
        break;
      case hash_beta:
        if (phashe->val >= beta) return phashe->val;
        break;
      }
    }
  }
  return unknown;
}

// 写入置换表
void AI::RecordHash(int depth, int val, int hashf) {
  Hashe *phashe = &hashTable[zobristKey % hashSize];
  phashe->key = zobristKey;
  phashe->val = val;
  phashe->hashf = hashf;
  phashe->depth = depth;
}

// 界面落子
void AI::TurnMove(Pos next) {
  next.x += 4, next.y += 4;
  MakeMove(next);
}

// 返回最佳点
Pos AI::TurnBest() {
  Pos best = gobang();
  best.x -= 4, best.y -= 4;
  return best;
}

// 搜索最佳点
Pos AI::gobang() {
  start = clock();
  total = 0;
  BestVal = 0;
  hashCount = 0;
  stopThink = false;

  // 第一步下中心点
  if (step == 0) {
    BestMove.x = size / 2 + 4;
    BestMove.y = size / 2 + 4;
    return BestMove;
  }
  // 第二三步随机
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
  // 迭代加深搜索
  memset(IsLose, false, sizeof(IsLose));
  for (int i = 2; i <= SearchDepth && BestVal != 10000; i += 2) {
    if (i > 8 && GetTime() * 12 >= StopTime()) {
      break;
    }
    MaxDepth = i;
    BestVal = minimax(i, -10001, 10000);
  }

  ThinkTime = GetTime();
  //cout << (double)hashCount / (total + 1) * 100 << endl;

  return BestMove;
}

inline bool AI::Same(Pos a, Pos b) {
  return a.x == b.x && a.y == b.y;
}

// 根节点搜索
int AI::minimax(int depth, int alpha, int beta) {
  Pos move[64];
  int move_count = GetMove(move, 40);

  if (move_count == 1) {
    BestMove = move[1];
    return BestVal;
  }

  move[0] = (depth > 2) ? BestMove : move[1];

  // 遍历可选点
  int val;
  for (int i = 0; i <= move_count; i++) {
    if (i > 0 && Same(move[0], move[i])) continue;
    // 跳过必败点
    if (IsLose[move[i].x][move[i].y]) continue;

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

    if (stopThink) break;

    if (val == -10000) IsLose[move[i].x][move[i].y] = true;

    if (val >= beta) {
      BestMove = move[i];
      return val;
    }
    if (val > alpha) {
      alpha = val;
      BestMove = move[i];
    }
  }
  return alpha == -10001 ? BestVal : alpha;
}

//带pvs的搜索
int AI::AlphaBeta(int depth, int alpha, int beta) {
  total++;

  static int cnt = 1000;
  if (--cnt <= 0) {
    cnt = 1000;
    if (GetTime() + 200 >= StopTime()) {
      stopThink = true;
    }
  }
  //对方已成五
  if (CheckWin()) return -10000;
  // 叶节点
  if (depth == 0) return evaluate();

  int val = ProbeHash(depth, alpha, beta);
  if (val != unknown) {
    hashCount++;
    return val;
  }
  if (MaxDepth <= 6){
    if (IsType(LastMove(), color(step), block4))
      depth += 2;
  }

  Pos move[64];
  int move_count = GetMove(move, 40);
  int hashf = hash_alpha;
  int val_best = -10000;
  for (int i = 1; i <= move_count; i++) {

    MakeMove(move[i]);
    do {
      if (hashf == hash_exact && alpha + 1 < beta) {
        val = -AlphaBeta(depth - 1, -alpha - 1, -alpha);
        if (val <= alpha || val >= beta) {
          break;
        }
      }
      val = -AlphaBeta(depth - 1, -beta, -alpha);
    } while (0);
    DelMove();

    if (stopThink) break;

    if (val > val_best){
      if (val >= beta) {
        RecordHash(depth, val, hash_beta);
        return val;
      }

      val_best = val;
      if (val > alpha) {
        hashf = hash_exact;
        alpha = val;
      }
    }
  }
  if (!stopThink) RecordHash(depth, val_best, hashf);

  return val_best;
}

// 棋型剪枝
int AI::CutCand(Pos * move, Point * cand, int Csize) {
  int you = color(step);
  int me = you ^ 1;
  int Msize = 0;

  if (cand[1].val >= 2400) {
    move[1] = cand[1].p;
    Msize = 1;
  }
  else if (cand[1].val == 1200) {
    move[1] = cand[1].p;
    Msize = 1;
    if (cand[2].val == 1200) {
      move[2] = cand[2].p;
      Msize = 2;
    }
    Cell *p;
    for (int i = Msize + 1; i <= Csize; ++i) {
      p = &cell[cand[i].p.x][cand[i].p.y];
      if (IsType(p, me, block4) || IsType(p, you, block4)) {
        ++Msize;
        move[Msize] = cand[i].p;
      }
    }
  }
  return Msize;
}

// 获取最好的MaxMoves个着法
int AI::GetMove(Pos * move, int MaxMoves) {
  int Csize = 0, Msize = 0;
  int me = color(step + 1);
  int val;
  for (int i = b_start; i < b_end; i++) {
    for (int j = b_start; j < b_end; j++) {
      if (cell[i][j].IsCand && cell[i][j].piece == Empty) {
        val = ScoreMove(&cell[i][j], me);
        if (val > 0) {
          ++Csize;
          cand[Csize].p.x = i;
          cand[Csize].p.y = j;
          cand[Csize].val = val;
        }
      }
    }
  }
  // 排序
  sort(cand, Csize);
  // 剪枝
  Csize = (Csize < MaxMoves) ? Csize : MaxMoves;
  Msize = CutCand(move, cand, Csize);
  // 如果没发生剪枝
  if (Msize == 0) {
    Msize = Csize;
    for (int k = 1; k <= Msize; ++k) {
      move[k] = cand[k].p;
    }
  }

  return Msize;
}

// 插入排序
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

// 局面评价
int AI::evaluate() {
  int Ctype[8] = { 0 }, Htype[8] = { 0 };
  int you = color(step), me = you ^ 1;
  int p_block4;

  for (int i = b_start; i < b_end; ++i) {
    for (int j = b_start; j < b_end; ++j) {
      if (cell[i][j].IsCand && cell[i][j].piece == Empty) {
        p_block4 = Ctype[block4];
        TypeCount(&cell[i][j], me, Ctype);
        TypeCount(&cell[i][j], you, Htype);
        if (Ctype[block4] - p_block4 > 1)
          Ctype[flex4]++;
      }
    }
  }

  if (Ctype[win] > 0) return 10000;
  if (Htype[win] > 1) return -10000;
  if (Ctype[flex4] > 0 && Htype[win] == 0)
    return 10000;

  int Cscore = 0, Hscore = 0;
  for (int i = 1; i < 8; ++i) {
    Cscore += Ctype[i] * Cval[i];
    Hscore += Htype[i] * Hval[i];
  }

  return Cscore - Hscore;
}
// 着法打分
int AI::ScoreMove(Cell *c, int me) {
  int MeType[8] = { 0 };
  int YouType[8] = { 0 };

  TypeCount(c, me, MeType);
  TypeCount(c, me ^ 1, YouType);

  if (MeType[win] > 0)
    return 10000;
  if (YouType[win] > 0)
    return 5000;
  if (MeType[flex4] > 0 || MeType[block4] > 1)
    return 2400;
  if (MeType[block4] > 0 && MeType[flex3] > 0)
    return 2000;
  if (YouType[flex4] > 0 || YouType[block4] > 1)
    return 1200;
  if (YouType[block4] > 0 && YouType[flex3] > 0)
    return 1000;
  if (MeType[flex3] > 1)
    return 400;
  if (YouType[flex3] > 1)
    return 200;

  int score = 0;
  for (int i = 1; i <= block4; i++) {
    score += MeVal[i] * MeType[i];
    score += YouVal[i] * YouType[i];
  }

  return score;
}
