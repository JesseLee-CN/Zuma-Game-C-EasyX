#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include "LinkList.h"
#include "ball.h"

#define TOTAL_THETA (4 * 3.14159265358979)
#define PI         3.14159265358979

#define MAX_POPUPS 8

struct ScorePopup {
	float x, y;
	int score;
	int life;
};

int BALLRADIUS;
int winWidth = 600, winHeight = 600;
int centerX, centerY;
double R_OUTER, R_INNER;

void recomputeDimensions()
{
	int ref = (winWidth < winHeight ? winWidth : winHeight) / 2;
	BALLRADIUS = ref * 10 / 300;
	if (BALLRADIUS < 4) BALLRADIUS = 4;
	R_OUTER = ref * 260.0 / 300.0;
	R_INNER = ref * 30.0 / 300.0;
	centerX = winWidth / 2;
	centerY = winHeight / 2;
}

COLORREF ballColorTable[] = {
BLUE, GREEN, RED, YELLOW, MAGENTA, BROWN
};
//test for github lkt 2025/12/5
//��ͼ����ʾ��
void drawExample(void)
{
	// ��ʼ����ͼ����
	initgraph(winWidth, winHeight);

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

//沿螺旋线按弧长步进布置球（由外向内紧密排列）
void updateBallPos(Node* head)
{
	const double k = (R_OUTER - R_INNER) / TOTAL_THETA;
	const double step = 2.0 * BALLRADIUS;
	const double dtheta = 0.002;

	double theta = 0.0;
	Node* p = head;

	while (p->next != NULL) {
		p = p->next;
		double r = R_OUTER - k * theta;
		p->data.x = (int)(centerX + r * cos(theta));
		p->data.y = (int)(centerY + r * sin(theta));

		double arc = 0.0;
		while (arc < step) {
			double r_cur = R_OUTER - k * theta;
			double r_next = R_OUTER - k * (theta + dtheta);
			double ds_cur = sqrt(r_cur * r_cur + k * k);
			double ds_next = sqrt(r_next * r_next + k * k);
			double ds = (ds_cur + ds_next) / 2.0 * dtheta;

			if (arc + ds > step) {
				double remaining = step - arc;
				double rate = (ds_cur + ds_next) / 2.0;
				theta += remaining / rate;
				break;
			}

			arc += ds;
			theta += dtheta;
		}
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
		int threshold = 2 * BALLRADIUS;
		if (dist < threshold * threshold)
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


//绘制螺旋辅助线（灰白色虚线）
void drawSpiralGuide()
{
	const double k = (R_OUTER - R_INNER) / TOTAL_THETA;
	const double dtheta = 0.02;
	const double dashLen = 14.0;
	const double gapLen = 8.0;

	setcolor(RGB(160, 160, 160));

	double theta = 0.0;
	double r = R_OUTER;
	int prev_x = (int)(centerX + r * cos(theta));
	int prev_y = (int)(centerY + r * sin(theta));

	double segArc = 0.0;
	bool drawing = true;

	while (theta <= TOTAL_THETA + 0.5) {
		theta += dtheta;
		r = R_OUTER - k * theta;
		if (r < R_INNER) break;
		int x = (int)(centerX + r * cos(theta));
		int y = (int)(centerY + r * sin(theta));

		double dx = (double)(x - prev_x);
		double dy = (double)(y - prev_y);
		double segLen = sqrt(dx * dx + dy * dy);
		segArc += segLen;

		if (drawing) {
			line(prev_x, prev_y, x, y);
			if (segArc >= dashLen) {
				drawing = false;
				segArc = 0.0;
			}
		} else {
			if (segArc >= gapLen) {
				drawing = true;
				segArc = 0.0;
			}
		}

		prev_x = x;
		prev_y = y;
	}
}

int main()
{
	// 以桌面分辨率创建大缓冲，窗口初始缩放为 600x600
	{
		RECT wa;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
		initgraph(wa.right - wa.left, wa.bottom - wa.top);

		HWND hwnd = GetHWnd();
		SetWindowLongPtr(hwnd, GWL_STYLE,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN);

		RECT cr = {0, 0, winWidth, winHeight};
		AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hwnd, HWND_TOP, 0, 0,
			cr.right - cr.left, cr.bottom - cr.top,
			SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	recomputeDimensions();
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
	drawColBall(&cball, centerX, centerY);


	//�¼�ѭ����������ꡢʱ���¼�
	MOUSEMSG m;
	bool ballMoving = FALSE;
	bool aiming = FALSE;
	int aimx = centerX + 50;
	int aimy = centerY;
	float vx = 0, vy = 0;
	int counter = 0;
	int totalScore = 0;
	ScorePopup popups[MAX_POPUPS];
	int popupCount = 0;

	BeginBatchDraw();
	srand(time(NULL));
	while (true)
	{
		RECT rect;
		GetClientRect(GetHWnd(), &rect);
		int newW = rect.right - rect.left;
		int newH = rect.bottom - rect.top;
		if (newW >= 200 && newH >= 200 && (newW != winWidth || newH != winHeight)) {
			winWidth = newW;
			winHeight = newH;
			recomputeDimensions();
			updateBallPos(head);
			aimx = centerX + 50;
			aimy = centerY;
		}

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
					float dx = m.x - centerX;
					float dy = centerY - m.y;
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
		drawSpiralGuide();
		int id;
		bool sameColor;
		bool collision = collisionDetection(head, cball, &sameColor, &id);
		if (collision)
		{
			ListInsert(head, id, cball);
			updateBallPos(head);
			int earned = EliminateRuns(head);
			totalScore += earned;
			if (earned > 0 && popupCount < MAX_POPUPS) {
				ScorePopup sp;
				sp.x = (float)cball.x;
				sp.y = (float)cball.y;
				sp.score = earned;
				sp.life = 30;
				popups[popupCount++] = sp;
			}
			updateBallPos(head);

			cball.c = rand() % 6;
			drawColBall(&cball, centerX, centerY);
			ballMoving = FALSE;

		}
		drawBallList(head);

		//�����ײ���Ƿ񳬳���Χ
		if (cball.x > winWidth || cball.x <0 || cball.y > winHeight || cball.y < 0)
		{
			cball.c = rand() % 6;
			drawColBall(&cball, centerX, centerY);
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
			drawColBall(&cball, centerX, centerY);
		}
		
		// Update and render score popups
		for (int i = 0; i < popupCount; ) {
			popups[i].y -= 0.6f;
			popups[i].life--;
			if (popups[i].life <= 0) {
				popups[i] = popups[--popupCount];
			} else {
				i++;
			}
		}
		int popupFontH = BALLRADIUS * 2;
		if (popupFontH < 10) popupFontH = 10;
		settextstyle(popupFontH, 0, NULL);
		setbkmode(TRANSPARENT);
		for (int i = 0; i < popupCount; i++) {
			int brightness = popups[i].life * 255 / 30;
			int r = brightness;
			int g = brightness;
			int b = 0;
			settextcolor(RGB(r, g, b));
			char buf[16];
			sprintf(buf, "+%d", popups[i].score);
			int tw = textwidth(buf);
			outtextxy((int)(popups[i].x - tw / 2), (int)popups[i].y, buf);
		}
		// Draw total score in top-right corner
		int scoreFontH = BALLRADIUS * 3;
		if (scoreFontH < 14) scoreFontH = 14;
		settextstyle(scoreFontH, 0, NULL);
		settextcolor(RGB(255, 255, 200));
		char scoreStr[32];
		sprintf(scoreStr, "Score: %d", totalScore);
		int sw = textwidth(scoreStr);
		outtextxy(winWidth - sw - 10, 10, scoreStr);

		FlushBatchDraw();
		Sleep(16);
	}


}
	
