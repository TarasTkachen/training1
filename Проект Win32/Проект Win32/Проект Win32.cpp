#include <windows.h>
#include <iostream>
#include <cmath>
#include <commdlg.h>
#include <shlobj.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void czCreateMenu(HWND hWwnd);
void SaveScreen(HWND hWND);
bool save(HWND hWnd, OPENFILENAME openFile);
double f(double);
HDC dc;
static BOOL bFirstSave = TRUE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszMenuName = L"hMenu";
	wc.lpszClassName = L"CMyWnd";
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH));
	RegisterClass(&wc);
	HWND hWnd = CreateWindow(L"CMyWnd", L"Граффик функции", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 200, 120, NULL, NULL, hInstance, NULL);
	czCreateMenu(hWnd);
	dc = GetDC(hWnd);
	ShowWindow(hWnd, nCmdShow);
	SetTimer(hWnd, 1, USER_TIMER_MINIMUM, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void czCreateMenu(HWND hwnd)
{
	HMENU MainMenu = CreateMenu();
	HMENU hPopupMenu1 = CreatePopupMenu();
	HMENU hPopupMenu2 = CreatePopupMenu();
	HMENU hPopupMenu2_1 = CreatePopupMenu();
	HMENU hPopupMenu3 = CreatePopupMenu();
	AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu1, L"&File");
	{
		AppendMenu(hPopupMenu1, MF_STRING | MF_CHECKED, 101, L"New");
		AppendMenu(hPopupMenu1, MF_SEPARATOR, 0, L"");
		AppendMenu(hPopupMenu1, MF_STRING, 102, L"Save");
		AppendMenu(hPopupMenu1, MF_STRING, 103, L"Save as...");
		AppendMenu(hPopupMenu1, MF_SEPARATOR, 0, L"");
		AppendMenu(hPopupMenu1, MF_STRING, 104, L"Exit");
	}
	AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu2, L"&View");
	{
		AppendMenu(hPopupMenu2, MF_STRING | MF_POPUP, (UINT)hPopupMenu2_1, L"&Color");
		{
			AppendMenu(hPopupMenu2_1, MF_STRING, 201, L"Black");
			AppendMenu(hPopupMenu2_1, MF_STRING, 202, L"Blue");
			AppendMenu(hPopupMenu2_1, MF_STRING, 203, L"Green");
			AppendMenu(hPopupMenu2_1, MF_STRING, 204, L"Red");
		}
	}
	AppendMenu(MainMenu, MF_STRING, 300, L"&Reset");
	AppendMenu(MainMenu, MF_STRING | MF_POPUP, (UINT)hPopupMenu3, L"&Help");
	{
		AppendMenu(hPopupMenu3, MF_STRING, 401, L"&About");
	}
	SetMenu(hwnd, MainMenu);
}

double f(double x)
{
	return sin(x) + 0.5 * sin(5 * x);
}

#define WIN32_LEAN_AND_MEAN
#define IDM_NEW 101
#define IDM_SAVE_AS 103
#define IDM_EXIT 104
#define IDM_BLACK 201
#define IDM_BLUE 202
#define IDM_GREEN 203
#define IDM_RED 204
#define IDM_RESET 300
#define IDM_ABOUT 401
#define IDM_SAVE 102

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static bool Move = true;
	static int Phase = 0, Width, Height;
	HPEN hPen;
	OPENFILENAME openFile{};
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		Move = !Move;

	case WM_TIMER:
		if (Move)
			Phase++;
		else
			break;
	case WM_PAINT:
		Rectangle(dc, -1, -1, Width + 1, Height + 1);
		MoveToEx(dc, 0, Height * (0.5 + 0.3 * f(0.1 * Phase)), NULL);
		for (long int i = 0; i < Width; i++)
			LineTo(dc, i, Height * (0.5 + 0.3 * f(0.1 * (i + Phase))));
		break;
	case WM_SIZE:
		Width = LOWORD(lParam), Height = HIWORD(lParam);
		break;
	case WM_KEYDOWN:
		if (wParam != VK_ESCAPE)
			break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_SAVE:
			if (bFirstSave == FALSE)
			{
				save(hWnd, openFile);
				break;
			}
			else
			{
				SaveScreen(hWnd);
				break;
			}
		case IDM_SAVE_AS:
			SaveScreen(hWnd);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_BLACK:
			hPen = CreatePen(PS_SOLID,
				1, RGB(0, 0, 0));
			SelectObject(dc, hPen);
			break;
		case IDM_BLUE:
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			SelectObject(dc, hPen);
			break;
		case IDM_GREEN:
			hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
			SelectObject(dc, hPen);
			break;
		case IDM_RED:
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			SelectObject(dc, hPen);
			break;
		case IDM_NEW:
		case IDM_RESET:
			Phase = 0;
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, L"Эта програма была написана гениальным программистом, а потом отредактирована мной",
				L"О программе", MB_OK | MB_ICONWARNING);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void SaveScreen(HWND hWnd)
{
	OPENFILENAME openFile;
	TCHAR szPath[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&openFile, sizeof(OPENFILENAME));
	openFile.lStructSize = sizeof(OPENFILENAME);
	szFile[0] = '\0';
	openFile.hwndOwner = hWnd;
	openFile.lpstrFile = szFile;
	openFile.nMaxFile = sizeof(szFile) / sizeof(*szFile);
	openFile.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
	openFile.lpstrDefExt = L"*.bmp";
	int bFirstSave{};
	if (TRUE == bFirstSave)
	{
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL,
			0, szPath)))
			openFile.lpstrInitialDir = szPath;
	}
	if (GetSaveFileName(&openFile) == TRUE)
	{
		UpdateWindow(hWnd);
		save(hWnd, openFile);
		bFirstSave = FALSE;
	}
}

bool save(HWND hWnd, OPENFILENAME openFile)
{
	HDC hdcScreen;
	HDC hdcWindow;
	HDC hdcMemDC = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	hdcScreen = GetDC(NULL);
	hdcWindow = GetDC(hWnd);
	hdcMemDC = CreateCompatibleDC(hdcWindow);
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32)
		* 4 * bmpScreen.bmHeight;
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);
	GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight,
		lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	HANDLE hFile = CreateFile(openFile.lpstrFile, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwSizeofDIB = dwBmpSize
		+ sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)
		+ (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader,
		sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi,
		sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
	CloseHandle(hFile);
	if (!hdcMemDC)
	{
		MessageBox(hWnd, L"CreateCompatibleDC has failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
		return TRUE;
	}
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	hbmScreen = CreateCompatibleBitmap(hdcWindow,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	if (!hbmScreen)
	{
		MessageBox(hWnd, L"CreateCompatibleBitmap Failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
		return TRUE;
	}

	SelectObject(hdcMemDC, hbmScreen);

	if (!BitBlt(hdcMemDC, 0, 0, rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top, hdcWindow, 0, 0, SRCCOPY))
	{
		MessageBox(hWnd, L"BitBlt has failed", L"Failed", MB_OK);
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);
		return TRUE;
	}
}