wine程序介绍

1.棋盘结构

容易想到用3个数字来表示:黑子 白子 空

const int White=0;

const int Black=1;

const int Empty=2;

下面的piece就表示这三种状态

pattern表示4个方向的棋型，黑白双方要分开，所以是pattern[2][4]

struct Cell{

	int piece;

	int pattern[2][4];

};

例子:cell[8][8].pattern[1][0]表示，黑棋下在（8，8）的0方向能成的棋型

五子棋的棋型都被定义成了一组常量

const int win = 7;              // 连五

const int flex4 = 6;            // 活四

const int block4 = 5;           // 冲四

const int flex3 = 4;            // 活三

const int block3 = 3;           // 眠三

const int flex2 = 2;            // 活二

const int block2 = 1;           // 眠二


由于经常要遍历四个方向，定义了4个方向向量

const int dx[4]={1,0,1,1};

const int dy[4]={0,1,1,-1};

分别对应了下x,y在四个方向上移动一格的增量



2.增量式棋盘

估值时要知道各个点的棋形，每次都计算将很慢。

所以用了增量式计算的办法，每次添加或移走棋子，都立即更新pattern,这个更新函数是UpdateType
