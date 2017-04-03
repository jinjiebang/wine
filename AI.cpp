
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
  Hashe *phashe = &hashTable[zobristKey & (hashSize - 1)];
  if (phashe->key == zobristKey) {
    if (phashe->depth >= depth) {
      if(phashe->hashf == hash_exact){
        return phashe->val;
      }else if(phashe->hashf == hash_alpha && phashe->val <= alpha){
        return phashe->val;
      }else if(phashe->hashf == hash_beta && phashe->val >= beta){
        return phashe->val;
      }
    }
  }
  return unknown;
}

// 写入置换表
void AI::RecordHash(int depth, int val, int hashf) {
  Hashe *phashe = &hashTable[zobristKey & (hashSize - 1)];
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
  hashCount = 0;

  Pos bestMove;
  // 第一步下中心点
  if (step == 0) {
    bestMove.x = size / 2 + 4;
    bestMove.y = size / 2 + 4;
    return bestMove;
  }
  /* 第二步随机选择第一手棋周围一格内的点, 第三步随机选择第一手棋周围两格内的点 */
  if (step == 1 || step == 2) {
    int rx, ry;
    srand(time(NULL));
    do {
      /*rand() % (step * 2 + 1) - step表示随机选择 -step 到 step 的值*/
      rx = remMove[0].x + rand() % (step * 2 + 1) - step;
      ry = remMove[0].y + rand() % (step * 2 + 1) - step;
    } while (!CheckXy(rx, ry) || cell[rx][ry].piece != Empty);
    bestMove.x = rx;
    bestMove.y = ry;
    return bestMove;
  }
  // 迭代加深搜索
  stopThink = false;
  bestIndex = 0;
  bestPoint.val = 0;
  memset(IsLose, false, sizeof(IsLose));
  for (int i = 2; i <= searchDepth && !stopThink; i += 2) {
    MaxDepth = i;
    bestPoint = minimax(i, -10001, 10000);
    if (GetTime() >= 1000 && i >= 10 && GetTime() * 12 >= StopTime()) stopThink = true;
  }

  ThinkTime = GetTime();
  bestMove = bestPoint.p;
  //cout << (double)hashCount / (total + 1) * 100 << endl;

  return bestMove;
}

inline bool AI::Same(Pos a, Pos b) {
  return a.x == b.x && a.y == b.y;
}

// 根节点搜索
Point AI::minimax(int depth, int alpha, int beta) {
  Pos move[64];
  int move_count = GetMove(move);
  
  Point best;
  
  // 只存在一个可行着法，直接返回
  if (move_count == 1) {
    stopThink = true;
    best.p = move[0];
    best.val = 0;
    return best;
  }

  //上一层搜索的最佳点移动到首位
  if(depth > 2){
    for(int i = bestIndex - 1;i >= 0;i--){
        move[i + 1] = move[i];
    }
    move[0] = bestPoint.p;
    best = bestPoint;
  }

  // 遍历可选点
  int val;
  for (int i = 0; i < move_count; i++) {
    // 搜索非必败点
    if (!IsLose[move[i].x][move[i].y]){

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

      if (val > alpha) {
        alpha = val;
        bestIndex = i;
        best.p = move[i];
        best.val = val;
        if(val == 10000) {
          stopThink = true;
          return best;
        }
      }
    }

  }

  return best;
}

//带pvs的搜索
int AI::AlphaBeta(int depth, int alpha, int beta) {
  total++;

  static int cnt = 1000;
  if (--cnt <= 0) {
    cnt = 1000;
    if (GetTime() + 50 >= StopTime()) {
      stopThink = true;
    }
  }
  //对方已成五
  if (CheckWin()) return -10000;
  // 叶节点
  if (depth == 0) {
    if (IsType(LastMove(), opp, block4)) {
      depth++;
    } else {
      return evaluate();
    }
  }

  int val = ProbeHash(depth, alpha, beta);
  if (val != unknown) {
    hashCount++;
    return val;
  }

  Pos move[64];
  int move_count = GetMove(move);
  int hashf = hash_alpha;
  int val_best = -10000;
  for (int i = 0; i < move_count; i++) {

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

    if (val >= beta) {
      RecordHash(depth, val, hash_beta);
      return val;
    }
    if (val > val_best){
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
int AI::CutCand(Pos * move, Point * cand, int candCount) {
  int moveCount = 0;

  if (cand[0].val >= 2400) {                //存在活四以上的棋形，返回最高分的点
    move[0] = cand[0].p;
    moveCount++;
  }else if (cand[0].val == 1200) {          //此时对方存在活三，返回对方活四点和双方冲四点
    move[0] = cand[0].p;
    moveCount++;
    if (cand[1].val == 1200) {
       move[1] = cand[1].p;
       moveCount++;
    }
    Cell *p;
    int n = candCount < MaxMoves ? candCount : MaxMoves;
    for (int i = moveCount; i < n; ++i) {
      p = &cell[cand[i].p.x][cand[i].p.y];
      if (IsType(p, who, block4) || IsType(p, opp, block4)) {
        move[moveCount] = cand[i].p;
        moveCount++;
      }
    }
  }
  return moveCount;
}

// 获取最好的MaxMoves个着法
int AI::GetMove(Pos * move) {
  int candCount = 0;                //待选着法数
  int moveCount = 0;                //筛选排序后的着法数
  int val;
  for (int i = b_start; i < b_end; i++) {
    for (int j = b_start; j < b_end; j++) {
      if (cell[i][j].IsCand && cell[i][j].piece == Empty) {
        val = ScoreMove(&cell[i][j]);
        if (val > 0) {
          cand[candCount].p.x = i;
          cand[candCount].p.y = j;
          cand[candCount].val = val;
          ++candCount;
        }
      }
    }
  }
  // 排序
  sort(cand, candCount);
  // 剪枝
  moveCount = CutCand(move, cand, candCount);
  // moveCount==0说明没有可以剪枝的特殊情况
  if (moveCount == 0) {
    moveCount = MaxMoves < candCount ? MaxMoves : candCount;
    for (int k = 0; k < moveCount; k++) {
      move[k] = cand[k].p;
    }
  }

  return moveCount;
}

// 插入排序
void AI::sort(Point * a, int n) {
  int i, j;
  Point key;
  for (i = 1; i < n; i++) {
    key = a[i];
    for (j = i; j > 0 && a[j - 1].val < key.val; j--) {
      a[j] = a[j - 1];
    }
    a[j] = key;
  }
}

// 局面评价
int AI::evaluate() {
  int whoType[8] = { 0 };               //记录下子方棋形数
  int oppType[8] = { 0 };               //记录另一方棋形数
  int block4_temp;

  for (int i = b_start; i < b_end; ++i) {
    for (int j = b_start; j < b_end; ++j) {
      if (cell[i][j].IsCand && cell[i][j].piece == Empty) {
        block4_temp = whoType[block4];
        TypeCount(&cell[i][j], who, whoType);
        TypeCount(&cell[i][j], opp, oppType);
        if (whoType[block4] - block4_temp >= 2){
          whoType[flex4]++;
        }
      }
    }
  }

  if (whoType[win] >= 1) return 10000;
  if (oppType[win] >= 2) return -10000;
  if (oppType[win] == 0 && whoType[flex4] >= 1) return 10000;

  int Cscore = 0, Hscore = 0;
  for (int i = 1; i < 8; ++i) {
    Cscore += whoType[i] * Cval[i];
    Hscore += oppType[i] * Hval[i];
  }

  return Cscore - Hscore;
}

// 着法打分
int AI::ScoreMove(Cell *c) {
  int score[2];
  score[who] = pval[c->pattern[who][0]][c->pattern[who][1]][c->pattern[who][2]][c->pattern[who][3]];
  score[opp] = pval[c->pattern[opp][0]][c->pattern[opp][1]][c->pattern[opp][2]][c->pattern[opp][3]];

  /*下子方分值需乘2，因为它的棋形下一手就能形成，而对方的棋形还要下下手
   *200表示有双活三以上棋形，此时只返回其中更高的一方分值
   *否则返回双方分值之和
   */
  if (score[who] >= 200 || score[opp] >= 200){
    return score[who] >= score[opp] ? score[who] * 2 : score[opp];
  }else{
    return score[who] * 2 + score[opp];
  }
}
