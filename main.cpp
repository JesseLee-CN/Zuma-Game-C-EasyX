#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include "LinkList.h"
#include "ball.h"

#define WINDOWWITH 600
#define WINDOWHEIGHT 600

COLORREF ballColorTable[] = {
BLUE, GREEN, RED, YELLOW, MAGENTA, BROWN
};
//test for github lkt 2025/12/5
//绘图函数示例
void drawExample(void)
{
	// 初始化绘图窗口
	initgraph(WINDOWWITH, WINDOWHEIGHT);

	// 设置背景色
	setbkcolor(BLACK);
	// 用背景色清空屏幕
	cleardevice();

	// 设置绘图色为红色
	setcolor(RED);
	setfillcolor(RED);

	// 画矩形
	rectangle(100, 100, 300, 300);
	solidcircle(200, 200, 100);

	// 按任意键退出
	_getch();
	closegraph();
}

//初始化球链表
void initBallList(Node* head)
{
	int i;
	ball b;

	for (i = 0; i < 10; ++i) {
		//b.c = rand() % 6;
		b.c = i%6;
		ListInsert(head, 0, b);

	}
}

//更新球链中球的位置
void updateBallPos(Node* head)
{
	Node* p = head;
	int x = 10;
	int y = WINDOWHEIGHT / 2;
	while (p->next != NULL)
	{
		p = p->next;
		
		p->data.x = x;
		x = x + BALLRADIUS * 2;
		p->data.y = y;
	}

}

//球碰撞检测
bool collisionDetection(Node* head, ball b,bool* sameColor,int* id)
{
	Node* p = head->next;
	int dist = 0;
	int index = 0;

	while (p != NULL)
	{
		dist = (p->data.x - b.x) * (p->data.x - b.x) + (p->data.y - b.y) * (p->data.y - b.y);
		if (dist < 400)
		{
			if (b.c == p->data.c)
				* (sameColor) = TRUE;
			else
				*(sameColor) = FALSE;
			*id = index;
			return TRUE;
		}

		p = p->next;
		index++;
	}
	return FALSE;
		
}

//绘制球表
void drawBallList(Node* head)
{
	Node* p;
	ball b;

	p = head;

	while (p->next != NULL)
	{
		p = p->next;
		b = p->data;

		setcolor(ballColorTable[b.c]);
		setfillcolor(ballColorTable[b.c]);
		solidcircle(b.x, b.y, BALLRADIUS);

	}
}

//绘制碰撞球
void drawColBall(ball* b, int x, int y)
{
	b->x = x;
	b->y = y;

	setcolor(ballColorTable[b->c]);
	setfillcolor(ballColorTable[b->c]);
	solidcircle(b->x, b->y, BALLRADIUS);
}


int main()
{
	// 初始化绘图窗口
	initgraph(WINDOWWITH, WINDOWHEIGHT);
	setbkcolor(BLACK);
	cleardevice();

	//初始化球链
	Node* head = NULL;
	head = CreateEmptyList();
	initBallList(head);
	updateBallPos(head);
	drawBallList(head);

	//画碰撞球
	ball cball;
	cball.c = rand() % 6;
	float ballDirction = -1;
	float speed = 10;
	drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);


	//事件循环，处理鼠标、时间事件
	MOUSEMSG m;
	bool ballMoving = FALSE;
	bool aiming = FALSE;
	int aimy = WINDOWWITH / 2;
	int aimx = WINDOWHEIGHT;
	float vx = 0, vy = 0;
	int counter = 0;

	srand(time(NULL));
	while (true)
	{
		counter++;
		// 获取一条鼠标消息
		if (MouseHit())
		{
			m = GetMouseMsg();
			switch (m.uMsg)
			{
			case WM_MOUSEMOVE:
				aimx = m.x;
				aimy = m.y;
				break;
			case WM_LBUTTONDOWN:
				if (!ballMoving)
				{
					aiming = TRUE;
					aimx = m.x;
					aimy = m.y;
				}
				break;
			case WM_LBUTTONUP:
				//更新球的运动方向，球开始运动
				if (aiming && !ballMoving)
				{
					float dx = m.x - (WINDOWWITH / 2);
					float dy = (WINDOWHEIGHT - m.y);
					float length = sqrt(dx * dx + dy * dy);
					if (length > 0) {
						vx = (dx / length) * speed;
						vy = (dy / length) * speed;
					}
					ballMoving = TRUE;
					aiming = FALSE;
				}
				break;

			case WM_RBUTTONUP:
				DestroyList(head);
				closegraph();
				return 0;	// 按鼠标右键退出程序
			}
		}
		
		//定时更新球链、碰撞球
		cleardevice();
		int id;
		bool sameColor;
		bool collision = collisionDetection(head, cball, &sameColor,&id);
		if (collision)
		{
			if (!sameColor)
			{
				ListInsert(head, id, cball);
				updateBallPos(head);

			}
			else
			{
				ListDelete(head, id);
				updateBallPos(head);
			}

			cball.c = rand() % 6;
			drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);
			ballMoving = FALSE;

		}
		drawBallList(head);

		//检测碰撞球是否超出范围
		if (cball.x > WINDOWWITH || cball.x <0 || cball.y > WINDOWHEIGHT || cball.y < 0)
		{
			cball.c = rand() % 6;
			drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);
			ballMoving = FALSE;
		}
		if (!ballMoving && aiming) {
			setcolor(WHITE);
			line(cball.x, cball.y, aimx, aimy);
		}
		
		//移动、绘制碰撞球
		if(ballMoving == TRUE) 
		{
			drawColBall(&cball, cball.x += vx , cball.y -= vy);
		}
		else
		{
			drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);
		}
		
		Sleep(16);
	}


}
	
