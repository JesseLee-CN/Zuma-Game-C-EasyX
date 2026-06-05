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

//�������������λ��
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
	drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);


	//�¼�ѭ����������ꡢʱ���¼�
	MOUSEMSG m;
	bool ballMoving = FALSE;
	bool aiming = FALSE;
	int aimy = WINDOWWITH / 2;
	int aimx = WINDOWHEIGHT;
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

		//�����ײ���Ƿ񳬳���Χ
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
		
		//�ƶ���������ײ��
		if(ballMoving == TRUE) 
		{
			drawColBall(&cball, cball.x += vx , cball.y -= vy);
		}
		else
		{
			drawColBall(&cball, WINDOWWITH / 2, WINDOWHEIGHT);
		}
		
		FlushBatchDraw();
		Sleep(16);
	}


}
	
