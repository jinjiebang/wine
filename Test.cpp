#include <iostream>
using namespace std;
void GetXy(Pos &p){
	char x;
     int y;
     cin >> x >> y;
      p.x = x - 65;
      p.y = 15 - y;
}
void ShowShape(AI* ai){
	int type[8]={0};
	int role=ai->color(ai->step);
	Pos p=ai->remMove[ai->step];
	
	ai->TypeCount(p.x,p.y,role,type);
	for(int i=1;i<8;++i){
		if(type[i]){
			cout<<type[i]<<"个";
			switch (i) {
				case block2:
				cout<<"眠二";
				break;
				case flex2:
				cout<<"活二";
				break;
				case block3:
				cout<<"眠三";
				break;
				case flex3:
				cout<<"活三";
				break;
				case block4:
				cout<<"眠四";
				break;
				case flex4:
				cout<<"活四";
				break;
				case win:
				cout<<"连五";
				break;
			}
			cout<<endl;
		}
	}
}
	
	
void DrawChess(AI * ai) {
	int size=ai->size;
	cout<<"思考时间:"<<ai->ThinkTime<<endl;
	cout<<"局势:"<<ai->BestVal<<endl;
	cout<<"局面:"<<ai->total<<endl;
  for (int i = 0; i < size; i++) {
    if (size - i < 10)
      cout << " ";
    cout << size- i;
    for (int j = 0; j < size; j++) {
      switch (ai->cell[j][i].piece) {
      case 0:
        cout << "+ ";
        break;
      case 1:
        cout << "● ";
        break;
      case 2:
        cout << "○ ";
        break;
      case 3:
        cout << "□ ";
        break;
      }
    }
    cout << endl;
  }
  cout << "  ";
  for (int i = 0; i < size; i++) {
    char c = 65 + i;
    cout << c << " ";
  }
  cout << endl;
  ShowShape(ai);
}

//调试代码//