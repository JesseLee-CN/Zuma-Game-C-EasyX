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

enum GameState { MENU, CUSTOMIZE, PLAYING, CLEARING, SETTLEMENT };

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
//绘图演示程序
void drawExample(void)
{
	// 初始化绘图窗口
	initgraph(winWidth, winHeight);

	// 设置背景颜色
	setbkcolor(BLACK);
	// 用背景色清屏幕
	cleardevice();

	// 设置画图色为红色
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
void initBallList(Node* head, int count)
{
	int i;
	ball b;

	for (i = 0; i < count; ++i) {
		b.c = i % 6;
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

// 碰撞检测：找到发射球与球链的碰撞段，返回插入索引
// 策略：找距离最近的两个球，若相邻则插入二者之间，否则插入最近球之前
bool collisionDetection(Node* head, ball b, int* id)
{
	int threshold = 2 * BALLRADIUS;
	int threshSq = threshold * threshold;

	Node* p = head->next;
	int index = 0;

	float best1 = 1e9f, best2 = 1e9f;
	int idx1 = -1, idx2 = -1;

	while (p != NULL) {
		float dx = p->data.x - b.x;
		float dy = p->data.y - b.y;
		float dist = dx * dx + dy * dy;

		if (dist < best1) {
			best2 = best1; idx2 = idx1;
			best1 = dist;  idx1 = index;
		} else if (dist < best2) {
			best2 = dist;  idx2 = index;
		}

		p = p->next;
		index++;
	}

	if (idx1 < 0 || best1 > threshSq)
		return FALSE;

	// 两最近球是否在链中相邻？若相邻则插入二者之间
	if (idx2 >= 0 && (idx1 == idx2 + 1 || idx2 == idx1 + 1))
		*id = (idx1 > idx2) ? idx1 : idx2;
	else
		*id = idx1;

	return TRUE;
}

//绘制球链
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
void drawColBall(ball* b, float x, float y)
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

void drawButton(int x, int y, int w, int h, const char* text, int fontH, bool hovered)
{
	setfillcolor(hovered ? RGB(80, 80, 80) : RGB(40, 40, 40));
	setcolor(hovered ? WHITE : RGB(180, 180, 180));
	fillroundrect(x, y, x + w, y + h, 8, 8);
	roundrect(x, y, x + w, y + h, 8, 8);

	settextstyle(fontH, 0, NULL);
	setbkmode(TRANSPARENT);
	settextcolor(hovered ? WHITE : RGB(200, 200, 200));
	int tw = textwidth(text);
	int th = textheight(text);
	outtextxy(x + (w - tw) / 2, y + (h - th) / 2, text);
}

// Check if (mx,my) is inside a rectangle
bool inRect(int mx, int my, int x, int y, int w, int h)
{
	return mx >= x && mx <= x + w && my >= y && my <= y + h;
}

int main()
{
	// 以桌面分辨率创建大缓冲区，确保缩放/最大化有足够空间显示
	{
		RECT wa;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &wa, 0);
		int bufW = wa.right - wa.left;
		int bufH = wa.bottom - wa.top;
		initgraph(bufW, bufH);

		HWND hwnd = GetHWnd();
		SetWindowLongPtr(hwnd, GWL_STYLE,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN);

		RECT cr = {0, 0, winWidth, winHeight};
		AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hwnd, HWND_TOP, 0, 0,
			cr.right - cr.left, cr.bottom - cr.top,
			SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

		// 同步 winWidth/winHeight 与 SetWindowPos 后的实际客户区尺寸，
		// 确保游戏坐标系与 EasyX 鼠标坐标一致
		RECT actualRect;
		GetClientRect(hwnd, &actualRect);
		winWidth = actualRect.right - actualRect.left;
		winHeight = actualRect.bottom - actualRect.top;
	}

	recomputeDimensions();
	setbkcolor(BLACK);
	cleardevice();

	// ---- Game state variables ----
	GameState state = MENU;
	bool gameWon = false;
	int clearFrame = 0;

	// Difficulty configuration
	int startBalls = 10;
	int startShots = -1;   // -1 = infinite

	// Customize page values
	int customBalls = 20;
	int customShots = 80;
	bool customShotsInfinite = false;

	// Game objects (allocated when entering PLAYING)
	Node* head = NULL;
	ball cball;
	float speed = 10;
	MOUSEMSG m;
	bool ballMoving = FALSE;
	bool aiming = FALSE;
	int aimx = 0, aimy = 0;
	float vx = 0, vy = 0;
	int counter = 0;
	int totalScore = 0;
	ScorePopup popups[MAX_POPUPS];
	int popupCount = 0;
	int remainingShots = -1;

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
			if (state == PLAYING && head != NULL)
				updateBallPos(head);
			aimx = centerX + 50;
			aimy = centerY;
		}

		counter++;

		// ---- Mouse input handling (state-dependent) ----
		while (MouseHit())
		{
			m = GetMouseMsg();

			if (state == MENU) {
				switch (m.uMsg) {
				case WM_MOUSEMOVE:
					aimx = m.x; aimy = m.y;
					break;
				case WM_LBUTTONUP:
				{
					int btnW = winWidth * 55 / 100;
					if (btnW < 200) btnW = 200;
					int btnH = winHeight * 10 / 100;
					if (btnH < 40) btnH = 40;
					int gap = btnH + 6;
					int startY = winHeight * 3 / 10;
					int btnX = centerX - btnW / 2;

					// Build difficulty definitions inline with the click handler
					struct { const char* name; int balls; int shots; } diffs[] = {
						{"INFINITE",  10,  -1},
						{"EASY",      10, 100},
						{"COMMON",    20,  80},
						{"HARD",      30,  50},
						{"CUSTOMIZED", 0,   0},
					};

					for (int i = 0; i < 5; i++) {
						int by = startY + i * gap;
						if (inRect(m.x, m.y, btnX, by, btnW, btnH)) {
							if (i == 4) {
								state = CUSTOMIZE;
							} else {
								startBalls = diffs[i].balls;
								startShots = diffs[i].shots;
								// Initialize game
								head = CreateEmptyList();
								initBallList(head, startBalls);
								updateBallPos(head);
								cball.c = rand() % 6;
								drawColBall(&cball, centerX, centerY);
								ballMoving = FALSE;
								aiming = FALSE;
								totalScore = 0;
								popupCount = 0;
								remainingShots = startShots;
								gameWon = false;
								state = PLAYING;
							}
							break;
						}
					}
					break;
				}
				case WM_RBUTTONUP:
					EndBatchDraw();
					closegraph();
					return 0;
				}
			}
			else if (state == CUSTOMIZE) {
				switch (m.uMsg) {
				case WM_MOUSEMOVE:
					aimx = m.x; aimy = m.y;
					break;
				case WM_LBUTTONUP:
				{
					int ctrlBtnS = winHeight / 20;
					if (ctrlBtnS < 22) ctrlBtnS = 22;
					if (ctrlBtnS > 36) ctrlBtnS = 36;
					int ctrlFontH = ctrlBtnS * 3 / 4;
					int rowH = ctrlBtnS + 8;
					int row1Y = winHeight * 35 / 100;
					int row2Y = row1Y + rowH + 12;

					// Value label widths
					settextstyle(ctrlFontH, 0, NULL);
					int labelW = textwidth("Shots:  ") + 10;
					int valW = textwidth("200") + 30;
					int rowCenterX = centerX;

					// Row 1: Balls  [-][val][+]
					int ballsMinusX = rowCenterX - labelW/2;
					int ballsValX  = ballsMinusX + ctrlBtnS + 4;
					int ballsPlusX = ballsValX + valW + 4;
					// Row 2: Shots  [-][val][+][inf]
					int shotsMinusX = rowCenterX - labelW/2;
					int shotsValX  = shotsMinusX + ctrlBtnS + 4;
					int shotsPlusX = shotsValX + valW + 4;
					int shotsInfX  = shotsPlusX + ctrlBtnS + 8;

					int startBtnW = winWidth * 30 / 100;
					if (startBtnW < 120) startBtnW = 120;
					int startBtnH = winHeight * 8 / 100;
					if (startBtnH < 32) startBtnH = 32;
					int startBtnX = centerX - startBtnW / 2;
					int startBtnY = winHeight * 65 / 100;
					int backBtnY = startBtnY + startBtnH + 8;

					if (inRect(m.x, m.y, ballsMinusX, row1Y, ctrlBtnS, ctrlBtnS)) {
						if (customBalls > 5) customBalls--;
					}
					else if (inRect(m.x, m.y, ballsPlusX, row1Y, ctrlBtnS, ctrlBtnS)) {
						if (customBalls < 50) customBalls++;
					}
					else if (!customShotsInfinite && inRect(m.x, m.y, shotsMinusX, row2Y, ctrlBtnS, ctrlBtnS)) {
						if (customShots > 10) customShots--;
					}
					else if (!customShotsInfinite && inRect(m.x, m.y, shotsPlusX, row2Y, ctrlBtnS, ctrlBtnS)) {
						if (customShots < 200) customShots++;
					}
					else if (inRect(m.x, m.y, shotsInfX, row2Y, ctrlBtnS + 6, ctrlBtnS)) {
						customShotsInfinite = !customShotsInfinite;
					}
					else if (inRect(m.x, m.y, startBtnX, startBtnY, startBtnW, startBtnH)) {
						startBalls = customBalls;
						startShots = customShotsInfinite ? -1 : customShots;
						head = CreateEmptyList();
						initBallList(head, startBalls);
						updateBallPos(head);
						cball.c = rand() % 6;
						drawColBall(&cball, centerX, centerY);
						ballMoving = FALSE;
						aiming = FALSE;
						totalScore = 0;
						popupCount = 0;
						remainingShots = startShots;
						gameWon = false;
						state = PLAYING;
					}
					else if (inRect(m.x, m.y, startBtnX, backBtnY, startBtnW, startBtnH)) {
						state = MENU;
					}
					break;
				}
				case WM_RBUTTONUP:
					EndBatchDraw();
					closegraph();
					return 0;
				}
			}
			else if (state == SETTLEMENT) {
				switch (m.uMsg)
				{
				case WM_MOUSEMOVE:
					aimx = m.x; aimy = m.y;
					break;
				case WM_LBUTTONUP:
				{
					int btnW = winWidth * 3 / 10;
					if (btnW < 120) btnW = 120;
					int btnH = winHeight * 8 / 100;
					if (btnH < 36) btnH = 36;
					int btnSpacing = btnH / 2;
					int btnX = centerX - btnW / 2;
					int tryAgainY = centerY + winHeight / 6;
					int exitY = tryAgainY + btnH + btnSpacing;

					if (inRect(m.x, m.y, btnX, tryAgainY, btnW, btnH)) {
						DestroyList(head);
						head = NULL;
						state = MENU;
					}
					else if (inRect(m.x, m.y, btnX, exitY, btnW, btnH)) {
						EndBatchDraw();
						DestroyList(head);
						closegraph();
						return 0;
					}
					break;
				}
				case WM_RBUTTONUP:
					EndBatchDraw();
					DestroyList(head);
					closegraph();
					return 0;
				}
			}
			else if (state == PLAYING) {
				switch (m.uMsg)
				{
				case WM_MOUSEMOVE:
					aimx = m.x; aimy = m.y;
					break;
				case WM_LBUTTONDOWN:
					if (!ballMoving) {
						aiming = TRUE;
						aimx = m.x;
						aimy = m.y;
					}
					break;
				case WM_LBUTTONUP:
					if (aiming && !ballMoving) {
						float dx = m.x - centerX;
						float dy = centerY - m.y;
						float length = sqrt(dx * dx + dy * dy);
						if (length > 0) {
							vx = (dx / length) * speed;
							vy = (dy / length) * speed;
						}
						if (remainingShots > 0)
							remainingShots--;
						ballMoving = TRUE;
						aiming = FALSE;
					}
					break;
				case WM_RBUTTONUP:
					EndBatchDraw();
					DestroyList(head);
					closegraph();
					return 0;
				}
			}
			// CLEARING: ignore all mouse input
		}

		// ---- Rendering (state-dependent) ----
		cleardevice();

		switch (state) {

		// ==================== MENU ====================
		case MENU:
		{
			int titleFontH = winHeight / 8;
			if (titleFontH < 24) titleFontH = 24;
			settextstyle(titleFontH, 0, NULL);
			setbkmode(TRANSPARENT);
			settextcolor(RGB(255, 200, 50));
			const char* title = "ZUMA";
			int ttw = textwidth(title);
			outtextxy(centerX - ttw / 2, winHeight / 12, title);

			struct { const char* name; const char* desc; } diffs[] = {
				{"INFINITE",   "Balls: 10   |  Shots: Infinite"},
				{"EASY",       "Balls: 10   |  Shots: 100"},
				{"COMMON",     "Balls: 20   |  Shots: 80"},
				{"HARD",       "Balls: 30   |  Shots: 50"},
				{"CUSTOMIZED", "Set your own rules"},
			};

			int btnW = winWidth * 55 / 100;
			if (btnW < 200) btnW = 200;
			int btnH = winHeight * 10 / 100;
			if (btnH < 40) btnH = 40;
			int gap = btnH + 6;
			int startY = winHeight * 3 / 10;

			for (int i = 0; i < 5; i++) {
				int btnX = centerX - btnW / 2;
				int btnY = startY + i * gap;
				bool hover = inRect(aimx, aimy, btnX, btnY, btnW, btnH);

				setfillcolor(hover ? RGB(60, 60, 80) : RGB(30, 30, 50));
				setcolor(hover ? WHITE : RGB(150, 150, 200));
				fillroundrect(btnX, btnY, btnX + btnW, btnY + btnH, 10, 10);
				roundrect(btnX, btnY, btnX + btnW, btnY + btnH, 10, 10);

				int nameFontH = btnH * 55 / 100;
				settextstyle(nameFontH, 0, NULL);
				setbkmode(TRANSPARENT);
				settextcolor(hover ? WHITE : RGB(220, 220, 255));
				int ntw = textwidth(diffs[i].name);
				outtextxy(btnX + (btnW - ntw) / 2,
					btnY + btnH * 8 / 100, diffs[i].name);

				int descFontH = btnH * 30 / 100;
				if (descFontH < 9) descFontH = 9;
				settextstyle(descFontH, 0, NULL);
				settextcolor(hover ? RGB(200, 200, 255) : RGB(140, 140, 180));
				int dtw = textwidth(diffs[i].desc);
				outtextxy(btnX + (btnW - dtw) / 2,
					btnY + btnH * 62 / 100, diffs[i].desc);
			}
			break;
		}

		// ==================== CUSTOMIZE ====================
		case CUSTOMIZE:
		{
			// Title
			int titleFontH = winHeight / 10;
			if (titleFontH < 20) titleFontH = 20;
			settextstyle(titleFontH, 0, NULL);
			setbkmode(TRANSPARENT);
			settextcolor(RGB(200, 200, 255));
			const char* title = "CUSTOMIZED";
			int ttw = textwidth(title);
			outtextxy(centerX - ttw / 2, winHeight / 12, title);

			// Control sizes
			int ctrlBtnS = winHeight / 20;
			if (ctrlBtnS < 22) ctrlBtnS = 22;
			if (ctrlBtnS > 36) ctrlBtnS = 36;
			int ctrlFontH = ctrlBtnS * 3 / 4;
			int rowH = ctrlBtnS + 8;
			int row1Y = winHeight * 35 / 100;
			int row2Y = row1Y + rowH + 12;

			settextstyle(ctrlFontH, 0, NULL);
			int labelW = textwidth("Shots:  ") + 10;
			int valW = textwidth("200") + 30;
			int rowCenterX = centerX;

			// ---- Row 1: Balls ----
			{
				int minusX = rowCenterX - labelW/2;
				int valX   = minusX + ctrlBtnS + 4;
				int plusX  = valX + valW + 4;
				int ctrlY  = row1Y;

				settextcolor(RGB(200, 200, 200));
				outtextxy(rowCenterX - labelW/2 - textwidth("Balls: ") - 10, ctrlY + 2, "Balls:");

				// Minus button
				bool hMinus = inRect(aimx, aimy, minusX, ctrlY, ctrlBtnS, ctrlBtnS);
				setfillcolor(hMinus ? RGB(100, 60, 60) : RGB(50, 30, 30));
				setcolor(hMinus ? WHITE : RGB(150, 120, 120));
				fillroundrect(minusX, ctrlY, minusX + ctrlBtnS, ctrlY + ctrlBtnS, 4, 4);
				settextcolor(hMinus ? WHITE : RGB(200, 180, 180));
				settextstyle(ctrlFontH, 0, NULL);
				setbkmode(TRANSPARENT);
				int mtw = textwidth("-");
				outtextxy(minusX + (ctrlBtnS - mtw)/2, ctrlY + 1, "-");

				// Value
				settextcolor(RGB(255, 255, 200));
				char valBuf[8];
				sprintf(valBuf, "%d", customBalls);
				int vtw = textwidth(valBuf);
				outtextxy(valX + (valW - vtw)/2, ctrlY + 2, valBuf);

				// Plus button
				bool hPlus = inRect(aimx, aimy, plusX, ctrlY, ctrlBtnS, ctrlBtnS);
				setfillcolor(hPlus ? RGB(60, 100, 60) : RGB(30, 50, 30));
				setcolor(hPlus ? WHITE : RGB(120, 150, 120));
				fillroundrect(plusX, ctrlY, plusX + ctrlBtnS, ctrlY + ctrlBtnS, 4, 4);
				settextcolor(hPlus ? WHITE : RGB(180, 200, 180));
				int ptw = textwidth("+");
				outtextxy(plusX + (ctrlBtnS - ptw)/2, ctrlY + 1, "+");
			}

			// ---- Row 2: Shots ----
			{
				int minusX = rowCenterX - labelW/2;
				int valX   = minusX + ctrlBtnS + 4;
				int plusX  = valX + valW + 4;
				int infX   = plusX + ctrlBtnS + 8;
				int ctrlY  = row2Y;

				settextcolor(RGB(200, 200, 200));
				outtextxy(rowCenterX - labelW/2 - textwidth("Shots: ") - 10, ctrlY + 2, "Shots:");

				// Minus button
				bool shotsActive = !customShotsInfinite;
				bool hMinus = shotsActive && inRect(aimx, aimy, minusX, ctrlY, ctrlBtnS, ctrlBtnS);
				setfillcolor((!shotsActive) ? RGB(25, 25, 25) : (hMinus ? RGB(100, 60, 60) : RGB(50, 30, 30)));
				setcolor((!shotsActive) ? RGB(60, 60, 60) : (hMinus ? WHITE : RGB(150, 120, 120)));
				fillroundrect(minusX, ctrlY, minusX + ctrlBtnS, ctrlY + ctrlBtnS, 4, 4);
				settextcolor((!shotsActive) ? RGB(80, 80, 80) : (hMinus ? WHITE : RGB(200, 180, 180)));
				settextstyle(ctrlFontH, 0, NULL);
				setbkmode(TRANSPARENT);
				int mtw = textwidth("-");
				outtextxy(minusX + (ctrlBtnS - mtw)/2, ctrlY + 1, "-");

				// Value
				settextcolor(shotsActive ? RGB(255, 255, 200) : RGB(100, 100, 80));
				char valBuf[8];
				sprintf(valBuf, "%d", customShots);
				int vtw = textwidth(valBuf);
				outtextxy(valX + (valW - vtw)/2, ctrlY + 2, valBuf);

				// Plus button
				bool hPlus = shotsActive && inRect(aimx, aimy, plusX, ctrlY, ctrlBtnS, ctrlBtnS);
				setfillcolor((!shotsActive) ? RGB(25, 25, 25) : (hPlus ? RGB(60, 100, 60) : RGB(30, 50, 30)));
				setcolor((!shotsActive) ? RGB(60, 60, 60) : (hPlus ? WHITE : RGB(120, 150, 120)));
				fillroundrect(plusX, ctrlY, plusX + ctrlBtnS, ctrlY + ctrlBtnS, 4, 4);
				settextcolor((!shotsActive) ? RGB(80, 80, 80) : (hPlus ? WHITE : RGB(180, 200, 180)));
				int ptw = textwidth("+");
				outtextxy(plusX + (ctrlBtnS - ptw)/2, ctrlY + 1, "+");

				// Infinite toggle
				bool hInf = inRect(aimx, aimy, infX, ctrlY, ctrlBtnS + 6, ctrlBtnS);
				setfillcolor(customShotsInfinite ? (hInf ? RGB(80, 80, 140) : RGB(50, 50, 100)) : (hInf ? RGB(50, 50, 50) : RGB(30, 30, 30)));
				setcolor(customShotsInfinite ? (hInf ? WHITE : RGB(150, 150, 220)) : (hInf ? RGB(180, 180, 180) : RGB(100, 100, 100)));
				fillroundrect(infX, ctrlY, infX + ctrlBtnS + 6, ctrlY + ctrlBtnS, 4, 4);
				settextcolor(customShotsInfinite ? (hInf ? WHITE : RGB(200, 200, 255)) : (hInf ? WHITE : RGB(150, 150, 150)));
				int iw = textwidth("inf");
				settextstyle(ctrlFontH - 2, 0, NULL);
				outtextxy(infX + (ctrlBtnS + 6 - iw)/2, ctrlY + 2, "inf");
			}

			// ---- START GAME and BACK buttons ----
			int actFontH = ctrlFontH + 2;
			int startBtnW = winWidth * 30 / 100;
			if (startBtnW < 120) startBtnW = 120;
			int startBtnH = winHeight * 8 / 100;
			if (startBtnH < 32) startBtnH = 32;
			int startBtnX = centerX - startBtnW / 2;
			int startBtnY = winHeight * 65 / 100;
			int backBtnY = startBtnY + startBtnH + 8;

			bool hoverStart = inRect(aimx, aimy, startBtnX, startBtnY, startBtnW, startBtnH);
			bool hoverBack  = inRect(aimx, aimy, startBtnX, backBtnY, startBtnW, startBtnH);

			drawButton(startBtnX, startBtnY, startBtnW, startBtnH, "START GAME", actFontH, hoverStart);
			drawButton(startBtnX, backBtnY,  startBtnW, startBtnH, "BACK", actFontH, hoverBack);
			break;
		}

		// ==================== PLAYING ====================
		case PLAYING:
		{
			drawSpiralGuide();
			int id;
			bool collision = collisionDetection(head, cball, &id);
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

				if (head->next == NULL) {
					gameWon = true;
					state = CLEARING;
					clearFrame = 0;
				}

				cball.c = rand() % 6;
				drawColBall(&cball, centerX, centerY);
				ballMoving = FALSE;

				// Check game over after collision resolution
				if (state == PLAYING && remainingShots == 0 && head->next != NULL) {
					int remBalls = 0;
					Node* np = head->next;
					while (np != NULL) { remBalls++; np = np->next; }
					totalScore -= remBalls * 20;
					if (totalScore < 0) totalScore = 0;
					gameWon = false;
					state = CLEARING;
					clearFrame = 0;
				}
			}
			drawBallList(head);

			// Boundary check
			if (cball.x > winWidth || cball.x <0 || cball.y > winHeight || cball.y < 0)
			{
				cball.c = rand() % 6;
				drawColBall(&cball, centerX, centerY);
				ballMoving = FALSE;

				if (remainingShots == 0 && head->next != NULL) {
					int remBalls = 0;
					Node* np = head->next;
					while (np != NULL) { remBalls++; np = np->next; }
					totalScore -= remBalls * 20;
					if (totalScore < 0) totalScore = 0;
					gameWon = false;
					state = CLEARING;
					clearFrame = 0;
				}
			}
			if (!ballMoving && aiming) {
				setcolor(WHITE);
				line(cball.x, cball.y, aimx, aimy);
			}

			// Move / render collision ball
			if (ballMoving == TRUE)
			{
				drawColBall(&cball, cball.x += vx , cball.y -= vy);
			}
			else
			{
				drawColBall(&cball, centerX, centerY);
			}
			break;
		}

		// ==================== CLEARING ====================
		case CLEARING:
		{
			clearFrame++;

			if (clearFrame < 60) {
				drawSpiralGuide();
				drawColBall(&cball, centerX, centerY);

				int lineGap = 16 - clearFrame * 15 / 59;
				if (lineGap < 1) lineGap = 1;
				setcolor(BLACK);
				for (int y = 0; y < winHeight; y += lineGap)
					line(0, y, winWidth, y);

			} else if (clearFrame < 120) {
				float t = (clearFrame - 60) / 60.0f;
				if (t > 1.0f) t = 1.0f;

				int cornerFontH = BALLRADIUS * 3;
				if (cornerFontH < 14) cornerFontH = 14;
				int centerFontH = winHeight / 6;
				if (centerFontH < 24) centerFontH = 24;
				int curFontH = cornerFontH + (int)((centerFontH - cornerFontH) * t);

				settextstyle(curFontH, 0, NULL);
				setbkmode(TRANSPARENT);
				settextcolor(RGB(255, 255, 200));

				char scoreStr[32];
				sprintf(scoreStr, "Score: %d", totalScore);
				int tw = textwidth(scoreStr);

				int startX = winWidth - tw - 10;
				int startY = 10;
				int endX = centerX - tw / 2;
				int endY = centerY - curFontH / 2;

				int curX = startX + (int)((endX - startX) * t);
				int curY = startY + (int)((endY - startY) * t);
				outtextxy(curX, curY, scoreStr);
			} else {
				state = SETTLEMENT;
			}

			// Popups and corner score during Phase A
			if (clearFrame < 60) {
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
					settextcolor(RGB(brightness, brightness, 0));
					char buf[16];
					sprintf(buf, "+%d", popups[i].score);
					int tw = textwidth(buf);
					outtextxy((int)(popups[i].x - tw / 2), (int)popups[i].y, buf);
				}

				int scoreFontH = BALLRADIUS * 3;
				if (scoreFontH < 14) scoreFontH = 14;
				settextstyle(scoreFontH, 0, NULL);
				settextcolor(RGB(255, 255, 200));
				char scoreStr[32];
				sprintf(scoreStr, "Score: %d", totalScore);
				int sw = textwidth(scoreStr);
				outtextxy(winWidth - sw - 10, 10, scoreStr);
			}
			break;
		}

		// ==================== SETTLEMENT ====================
		case SETTLEMENT:
		{
			// Title
			int titleFontH = winHeight / 12;
			if (titleFontH < 18) titleFontH = 18;
			settextstyle(titleFontH, 0, NULL);
			setbkmode(TRANSPARENT);
			if (gameWon) {
				settextcolor(RGB(100, 255, 100));
				const char* ttl = "CLEAR!";
				int ttw = textwidth(ttl);
				outtextxy(centerX - ttw / 2, winHeight / 8, ttl);
			} else {
				settextcolor(RGB(255, 100, 100));
				const char* ttl = "GAME OVER";
				int ttw = textwidth(ttl);
				outtextxy(centerX - ttw / 2, winHeight / 8, ttl);
			}

			// Final score
			int scoreFontH = winHeight / 6;
			if (scoreFontH < 24) scoreFontH = 24;
			settextstyle(scoreFontH, 0, NULL);
			settextcolor(RGB(255, 255, 200));
			char scoreStr[32];
			sprintf(scoreStr, "Score: %d", totalScore);
			int sw = textwidth(scoreStr);
			outtextxy(centerX - sw / 2, centerY - scoreFontH, scoreStr);

			// Buttons
			int btnW = winWidth * 3 / 10;
			if (btnW < 120) btnW = 120;
			int btnH = winHeight * 8 / 100;
			if (btnH < 36) btnH = 36;
			int btnFontH = btnH / 2;
			int btnSpacing = btnH / 2;
			int btnX = centerX - btnW / 2;
			int tryAgainY = centerY + winHeight / 6;
			int exitY = tryAgainY + btnH + btnSpacing;

			bool hoverTryAgain = inRect(aimx, aimy, btnX, tryAgainY, btnW, btnH);
			bool hoverExit     = inRect(aimx, aimy, btnX, exitY, btnW, btnH);
			drawButton(btnX, tryAgainY, btnW, btnH, "TRY AGAIN", btnFontH, hoverTryAgain);
			drawButton(btnX, exitY,     btnW, btnH, "EXIT",      btnFontH, hoverExit);
			break;
		}
		}

		// ---- Post-render: popups + score HUD (PLAYING only) ----
		if (state == PLAYING) {
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
				settextcolor(RGB(brightness, brightness, 0));
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

			// Draw remaining shots
			int shotFontH = BALLRADIUS * 2;
			if (shotFontH < 10) shotFontH = 10;
			settextstyle(shotFontH, 0, NULL);
			char shotStr[32];
			if (remainingShots < 0) {
				settextcolor(RGB(150, 255, 150));
				sprintf(shotStr, "Shots: inf");
			} else if (remainingShots <= 5) {
				settextcolor(RGB(255, 100, 100));
				sprintf(shotStr, "Shots: %d", remainingShots);
			} else {
				settextcolor(RGB(200, 200, 200));
				sprintf(shotStr, "Shots: %d", remainingShots);
			}
			int ssw = textwidth(shotStr);
			outtextxy(winWidth - ssw - 10, 10 + scoreFontH + 2, shotStr);
		}

		FlushBatchDraw();
		Sleep(16);
	}
}
