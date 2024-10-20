#include <windows.h>
#include <stdio.h>

#define WINDOWWIDTH 640
#define WINDOWHEIGHT 360

#ifdef __MINGW64__
#define PROGNAME "DCLOCK64_0.1"
#else
#define PROGNAME "DCLOCK32_0.1"
#endif

HWND hwnd;
WNDCLASS winc;
MSG msg;
HFONT hFont;
HDC hdc;
PAINTSTRUCT ps;
RECT Rect;
SYSTEMTIME st;

char Date[32]="";
char Time[32]="";
const char FontName[32]="MS Gothic";
int FontSize=0;
int FontPixel=0;

void WindowRedraw(void){
	
	GetLocalTime(&st);
	sprintf(Date, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(Time, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	
	GetClientRect(hwnd, &Rect);
	FontPixel=Rect.right/(strlen(Date)-3);
	FontSize=-MulDiv(FontPixel, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	
	hFont = CreateFont(
	FontSize , 0 , 0 , 0 , FW_BOLD , FALSE , FALSE , FALSE ,
	SHIFTJIS_CHARSET , OUT_DEFAULT_PRECIS ,
	CLIP_DEFAULT_PRECIS , DEFAULT_QUALITY , 
	FIXED_PITCH | FF_MODERN , FontName
	);
	
	SelectObject(hdc, hFont);
	TextOut(hdc , FontPixel*(0.1), FontPixel*(0.1) , Date, strlen(Date));
	TextOut(hdc , FontPixel*(0.1), FontPixel*(1.5) , Time, strlen(Time));
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	DeleteObject(hFont);
}

LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CREATE:
		GetClientRect(hwnd, &Rect);
		hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		Rectangle(hdc, 0, 0, Rect.right, Rect.bottom);
		ReleaseDC(hwnd, hdc);
		SetTimer(hwnd , 1 , 1000 , NULL);
		return 0;
	case WM_TIMER:
		InvalidateRect(hwnd , NULL , FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd , &ps);
		WindowRedraw();
		EndPaint(hwnd , &ps);
		return 0;
	}
	return DefWindowProc(hwnd , msg , wp , lp);
}

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance , PSTR lpCmdLine , int nCmdShow ) {
	
	HANDLE hMSP = CreateMutex(NULL, TRUE, PROGNAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		ReleaseMutex(hMSP);
		CloseHandle(hMSP);
		return 0;
	}
	
	winc.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	winc.lpfnWndProc	= WndProc;
	winc.cbClsExtra	= winc.cbWndExtra	= 0;
	winc.hInstance		= hInstance;
	winc.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor		= LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName	= NULL;
	winc.lpszClassName	= PROGNAME;
	
	if (!RegisterClass(&winc)) return 0;
	
	hwnd = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_COMPOSITED, 
		PROGNAME , PROGNAME ,
		WS_OVERLAPPEDWINDOW  | WS_VISIBLE ,
		CW_USEDEFAULT , CW_USEDEFAULT ,
		WINDOWWIDTH , WINDOWHEIGHT ,
		NULL , NULL ,
		hInstance , NULL
	);
	
	if (hwnd == NULL) return 0;
	
	//main message loop
	while (GetMessage(&msg , NULL , 0 , 0)) DispatchMessage(&msg);
	return msg.wParam;
	
	//exit code
	WSACleanup();
	
	return 0;
}
