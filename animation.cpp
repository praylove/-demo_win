// animation.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "animation.h"

#define MAX_LOADSTRING 100
#define SHOOT_SPEED 150
#define WINWIDE	640
#define WINHIGHT 800
#define ENEMY_NUM 20
#define ENEMY_PH 3
// 全局变量: 

//struct BULLET
//{
//	int x, y;
//	bool exist;
//};

struct PLANE
{
	int x, y;
	int cx, cy;
	int ph;
	bool exist, boom;
};
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HDC hdc, mdc, bufdc;
HBITMAP bg, plane, bullet;
HBITMAP bmp, enemy, boom;
int num, frame, fps;
DWORD tNow, tPre, tClock, tboom[ENEMY_NUM];
int x, y;
int w = 0, bcount, ecount;
int speed, score;
PLANE b[30], p, e[ENEMY_NUM];

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
VOID				Print(HDC);
BOOL				IsSafe(PLANE&, PLANE&);
VOID				Boom(HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ANIMATION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANIMATION));

	MSG msg;

	// 主消息循环: 
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tNow = GetTickCount();
			if (tNow - tPre >= 30)
				Print(hdc);
		}
	} while (msg.message != WM_QUIT);
	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANIMATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ANIMATION);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd, 500, 0, WINWIDE, WINHIGHT, true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc, WINWIDE, 1183);

	bg = (HBITMAP)LoadImage(NULL, _T("bg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	plane = (HBITMAP)LoadImage(NULL, _T("Airplane.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	bullet = (HBITMAP)LoadImage(NULL, _T("bullet.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	enemy = (HBITMAP)LoadImage(NULL, _T("enemy1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	boom = (HBITMAP)LoadImage(NULL, _T("enemy_hurt1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	num = 0;
	x = 300;
	y = 300;

	p.cx = 72;
	p.cy = 66;
	p.x = x;
	p.y = y;
	p.exist = TRUE;

	POINT pt;
	pt.x = 300;
	pt.y = 300;
	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x, pt.y);

	ShowCursor(FALSE);

	RECT rect;
	GetClientRect(hWnd, &rect);
	POINT p;
	p.x = rect.left;
	p.y = rect.top;
	ClientToScreen(hWnd, &p);
	rect.left = p.x;
	rect.top = p.y;
	p.x = rect.right;
	p.y = rect.bottom;
	ClientToScreen(hWnd, &p);
	rect.right = p.x;
	rect.bottom = p.y;
	ClipCursor(&rect);

	SelectObject(mdc, bmp);
	
	SetTimer(hWnd, 1, SHOOT_SPEED, NULL);
	Print(hdc);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_TIMER:
		++speed;
		{
			++bcount;
			for (int i = 0; i < 30; ++i)
			{
				if (!b[i].exist)
				{
					b[i].exist = TRUE;
					b[i].x = x - 5;
					b[i].y = y;
					b[i].cx = 10;
					b[i].cy = 10;
					break;
				}
			}
		}
		if(speed % 5 == 0)
		{
			++ecount;
			for (int i = 0; i < ENEMY_NUM; ++i)
			{
				if (!e[i].exist)
				{
					e[i].exist = TRUE;
					e[i].ph = ENEMY_PH;
					e[i].y = 0;
					e[i].cx = 65;
					e[i].cy = 45;
					srand(tPre);
					e[i].x = rand() % (WINWIDE - e[i].cx) - e[i].cx / 2;
					break;
				}
			}
		}
		break;
	case WM_DESTROY:
		DeleteDC(mdc);
		DeleteDC(bufdc);
		DeleteObject(bg);
		DeleteObject(plane);
		DeleteObject(bullet);
		DeleteObject(enemy);
		DeleteObject(boom);
		ReleaseDC(hWnd, hdc);
		KillTimer(hWnd, 1);

		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		if (x < 36)
			x = 36;
		else if (x > WINWIDE - 36)
			x = WINWIDE - 36;
		y = HIWORD(lParam);
		if (y < 33)
			y = 33;
		else if (y > WINHIGHT - 33)
			y = WINHIGHT - 33;
		break;
	case WM_LBUTTONDOWN:
		
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

VOID Print(HDC hdc)
{
	TCHAR sbuf[30];
	if (num > 3)
		num = 0;
	++frame;
	if (tNow - tClock >= 1000)
	{
		fps = frame;
		frame = 0;
		tClock = tNow;
	}

	SelectObject(bufdc, bg);
	BitBlt(mdc, 0, w, 639, 1133 - w, bufdc, 0, 0, SRCCOPY);
	BitBlt(mdc, 0, 0, 639, w, bufdc, 0, 1133 - w, SRCCOPY);

	SelectObject(bufdc, plane);
	if (p.exist)
	{
		p.x = x - p.cx / 2;
		p.y = y;
		BitBlt(mdc, p.x, p.y, p.cx, p.cy, bufdc, 0, 0, SRCAND);
	}
	else
	{
		MessageBox(NULL, _T("游戏结束！"), NULL, NULL);
		PostQuitMessage(0);
	}

	SelectObject(bufdc, enemy);
	if (ecount > 0)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (e[i].exist)
			{
				BitBlt(mdc, e[i].x, e[i].y, e[i].cx, e[i].cy, bufdc, 0, e[i].cy, SRCAND);
				BitBlt(mdc, e[i].x, e[i].y, e[i].cx, e[i].cy, bufdc, 0, 0, SRCPAINT);
				e[i].y += 10;
				if (e[i].y > WINHIGHT)
				{
					ecount--;
					e[i].exist = FALSE;
				}
				if (!IsSafe(e[i], p))
				{
					p.exist = FALSE;
				}
			}
		}
	}
	SelectObject(bufdc, bullet);
	if (bcount > 0)
	{
		for (int i = 0; i < 30; ++i)
		{
			if (b[i].exist)
			{
				BitBlt(mdc, b[i].x, b[i].y, 10, 10, bufdc, 0, 10, SRCAND);
				BitBlt(mdc, b[i].x, b[i].y, 10, 10, bufdc, 0, 0, SRCPAINT);
				b[i].y -= 20;
				if (b[i].y < 0)
				{
					bcount--;
					b[i].exist = FALSE;
				}
				else
					for (int j = 0; j < ENEMY_NUM; ++j)
					{
						if (e[j].exist && !IsSafe(b[i], e[j]))
						{
							--e[j].ph;
							if (!e[j].ph)
							{
								ecount--;
								e[j].exist = FALSE;
								e[j].boom = TRUE;
								score += 10;
								tboom[j] = GetTickCount();
							}
							bcount--;
							b[i].exist = FALSE;
							break;
						}
					}
			}
		}
	}
	SelectObject(bufdc, boom);
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (e[i].boom)
		{
			if (tNow - tboom[i] < 150)
			{
				BitBlt(mdc, e[i].x, e[i].y, e[i].cx, e[i].cy, bufdc, 0, e[i].cy, SRCAND);
				BitBlt(mdc, e[i].x, e[i].y, e[i].cx, e[i].cy, bufdc, 0, 0, SRCPAINT);
			}
			else
				e[i].boom = FALSE;
		}
	}
	BitBlt(hdc, 0, 0, 639, 1133, mdc, 0, 0, SRCCOPY);

	_stprintf(sbuf, _T("fps: %d"), fps);
	TextOut(hdc, WINWIDE - 100, 0, sbuf, _tcslen(sbuf));
	_stprintf(sbuf, _T("score: %d"), score);
	TextOut(hdc, WINWIDE - 100, 20, sbuf, _tcslen(sbuf));

	w += 10;
	if (w >= 1133)
		w = 0;

	tPre = GetTickCount();
	++num;
}

inline BOOL IsSafe(PLANE& a, PLANE& b)
{
	return (a.x - a.cx / 2) < (b.x - b.cx) || (b.x + b.cx) < (a.x + a.cx * 2 / 3)
		|| (a.y - a.cy / 2) < (b.y - b.cy) || (b.y + b.cy) < (a.y - a.cy * 2 / 3);
}

inline VOID Boom(HDC hdc)
{
	

}