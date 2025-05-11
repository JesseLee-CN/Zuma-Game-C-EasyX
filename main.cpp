#include <graphics.h>
#include <conio.h>
#include <time.h>
#include "LinkList.h"
#include "ball.h"

#define WINDOWWITH 600
#define WINDOWHEIGHT 600

COLORREF ballColorTable[] = {
BLUE, GREEN, RED, YELLOW, MAGENTA, BROWN
};

//绘图函数示例
/*void drawExample(void)
{
	// 设置绘图色为红色
	setcolor(RED);
	setfillcolor(RED);

	// 画矩形
	rectangle(100, 100, 300, 300);
	solidcircle(200, 200, 100);

}*/

//绘制碰撞球
void drawColBall(ball* b, int x, int y)
{
	b->x = x;
	b->y = y;

	setcolor(ballColorTable[b->c]);
	setfillcolor(ballColorTable[b->c]);
	solidcircle(b->x, b->y, BALLRADIUS);
}

//绘制球链链表，请补全该函数
void drawBallList(Node* head)
{
	Node* current;
	current = head->next;
	while (current != NULL) {
		setcolor(ballColorTable[current->data.c]);
		setfillcolor(ballColorTable[current->data.c]);
		solidcircle(current->data.x,current->data.y, BALLRADIUS);
		current = current->next;
	}

}

//初始化球链链表，即给球链链表中添加一些球，请补全该函数
void initBallList(Node* head)
{
	unsigned int n = 10;
	srand((unsigned int)time(NULL));
	for (unsigned int i = 0; i < n; i++) {
		ball currentball;
		(currentball).x = BALLRADIUS + BALLRADIUS * 2 * i;
		(currentball).y = WINDOWHEIGHT / 2;
		(currentball).c = rand() % 6;
		ListInsert(head, i, currentball);
	}
}

int main()
{
	// 初始化绘图窗口
	initgraph(WINDOWWITH, WINDOWHEIGHT);
	setbkcolor(BLACK);
	cleardevice();

	//创建一个空的球链表
	Node* head = NULL;
	head = CreateEmptyList();
	
	//初始化球链链表
	initBallList(head);

	//在窗口中绘制球链表
	drawBallList(head);
	//绘图示例函数，补全drawBallList（）函数后请删除
	//drawExample();


	//画碰撞球
	ball cball;
	cball.c = rand() % 6;
	drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);

	//等待任意键退出
	_getch();
	//销毁球链链表和绘图窗口
	DestroyList(head);
	closegraph();
	
	return 0;
}
	
