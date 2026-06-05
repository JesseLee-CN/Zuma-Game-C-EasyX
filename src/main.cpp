#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include "LinkList.h"
#include "ball.h"

#define WINDOWWITH 600
#define WINDOWHEIGHT 600
#define CANNONX    (WINDOWWITH / 2)
#define CANNONY    (WINDOWHEIGHT / 2)
#define R_OUTER    260
#define R_INNER    30
#define TOTAL_THETA (6 * 3.14159265358979)
#define PI         3.14159265358979

COLORREF ballColorTable[] = {
BLUE, GREEN, RED, YELLOW, MAGENTA, BROWN
};
//test for github lkt 2025/12/5
//��ͼ����ʾ��
void drawExample(void)
{
	// ��ʼ����ͼ����
	initgraph(WINDOWWITH, WINDOWHEIGHT);

	// ���ñ���ɫ
	setbkcolor(BLACK);
	// �ñ���ɫ�����Ļ
	cleardevice();

	// ���û�ͼɫΪ��ɫ
	setcolor(RED);
	setfillcolor(RED);

	// ������
	rectangle(100, 100, 300, 300);
	solidcircle(200, 200, 100);

	// ��������˳�
	_getch();
	closegraph();
}

//��ʼ��������
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

//��螺旋顺序更新球的位置（由外向内）
void updateBallPos(Node* head)
{
	int n = 0;
	Node* p = head;
	while (p->next != NULL) {
		p = p->next;
		n++;
	}

	int index = 0;
	p = head;
	while (p->next != NULL) {
		p = p->next;
		double t = (n == 1) ? 0.0 : (double)index / (n - 1);
		double r = R_OUTER - t * (R_OUTER - R_INNER);
		double theta = t * TOTAL_THETA;
		p->data.x = (int)(CANNONX + r * cos(theta));
		p->data.y = (int)(CANNONY + r * sin(theta));
		index++;
	}
}

//����ײ���
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

//�������
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

//������ײ��
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
	// ��ʼ����ͼ����
	initgraph(WINDOWWITH, WINDOWHEIGHT);
	setbkcolor(BLACK);
	cleardevice();

	//��ʼ������
	Node* head = NULL;
	head = CreateEmptyList();
	initBallList(head);
	updateBallPos(head);
	drawBallList(head);

	//����ײ��
	ball cball;
	cball.c = rand() % 6;
	float speed = 10;
	drawColBall(&cball, CANNONX, CANNONY);


	//�¼�ѭ����������ꡢʱ���¼�
	MOUSEMSG m;
	bool ballMoving = FALSE;
	bool aiming = FALSE;
	int aimx = CANNONX + 50;
	int aimy = CANNONY;
	float vx = 0, vy = 0;
	int counter = 0;

	BeginBatchDraw();
	srand(time(NULL));
	while (true)
	{
		counter++;
		// 处理全部待处理鼠标消息
		while (MouseHit())
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
				//��������˶�������ʼ�˶�
				if (aiming && !ballMoving)
				{
					float dx = m.x - CANNONX;
					float dy = CANNONY - m.y;
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
				EndBatchDraw();
				DestroyList(head);
				closegraph();
				return 0;	// ������Ҽ��˳�����
			}
		}
		
		//��ʱ������������ײ��
		cleardevice();
		int id;
		bool sameColor;
		bool collision = collisionDetection(head, cball, &sameColor, &id);
		if (collision)
		{
			ListInsert(head, id, cball);
			updateBallPos(head);
			EliminateRuns(head);
			updateBallPos(head);

			cball.c = rand() % 6;
			drawColBall(&cball, CANNONX, CANNONY);
			ballMoving = FALSE;

		}
		drawBallList(head);

		//�����ײ���Ƿ񳬳���Χ
		if (cball.x > WINDOWWITH || cball.x <0 || cball.y > WINDOWHEIGHT || cball.y < 0)
		{
			cball.c = rand() % 6;
			drawColBall(&cball, CANNONX, CANNONY);
			ballMoving = FALSE;
		}
		if (!ballMoving && aiming) {
			setcolor(WHITE);
			line(cball.x, cball.y, aimx, aimy);
		}
		
		//�ƶ���������ײ��
		if(ballMoving == TRUE) 
		{
			drawColBall(&cball, cball.x += vx , cball.y -= vy);
		}
		else
		{
			drawColBall(&cball, CANNONX, CANNONY);
		}
		
		FlushBatchDraw();
		Sleep(16);
	}


}
	
