#include <windows.h>
#include <wingdi.h>

struct Image
{
	HBITMAP hBitmap;
	LONG width, height;
};

typedef struct ControlObject
{
	int speed_x;
	int speed_y;
	int x;
	int y;
	int width;
	int height;
	Image image;

} ControlObject;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MyRedrawWindow(HWND hWnd);
struct Image loadImage(LPCTSTR path, HINSTANCE hInstance);
void InitializePlayer(ControlObject* object, int x, int y, int width, int height, Image image);
void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y);
void MovePlayer(ControlObject* object, RECT gameWindow);
void OnWindowResize(ControlObject* object, RECT gameWindow);
void DrawPlayer(ControlObject* object, HDC hdc, HDC hdcMemSurface);
void SetPlayerCoordinates(ControlObject* object, int x, int y);
bool IsFocusOnPlayer(ControlObject* object, int x, int y);

static int PLAYERWIDTH = 80;
static int PLAYERHEIGHT = 90;
static ControlObject player;
static HDC hdcMemSurface;
int wheelDelta = 0;
int delta = 10;

static bool is_up = false;
static bool is_down = false;
static bool is_left = false;
static bool is_right = false;
static bool is_player_catch = false;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_SCROLLBAR + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "HelloKsisClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow("HelloKsisClass", "Hello, Ksis!",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	InitializePlayer(&player, 0, 0,PLAYERWIDTH,PLAYERHEIGHT, loadImage(TEXT("1.bmp"), hInstance) );

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC hmdc;
	HBRUSH hBrush; //создаём объект-кисть
	PAINTSTRUCT ps;
	HDC hdc; //создаём контекст устройства

	int x;
	int y;
	x = 0;
	y = 0;
	RECT rect;
	GetClientRect(hWnd, &rect);
	switch (message)
	{

	case WM_TIMER:
		MovePlayer(&player, rect);
		MyRedrawWindow(hWnd);
		break;
	
	case WM_SIZE:
		OnWindowResize(&player, rect);
		MyRedrawWindow(hWnd);
		break;
	
	case WM_CREATE:
	{
		const HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		const HDC hdcWin = GetDC(hWnd);
		hdcMemSurface = CreateCompatibleDC(hdcWin);
		ReleaseDC(hWnd, hdcWin);
		SetTimer(hWnd, 1, 1000/30, NULL);
		return 0;
	}

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
			is_left = false;
			break;
		case VK_UP:
			is_up = false;
			break;
		case VK_RIGHT:
			is_right = false;
			break;
		case VK_DOWN:
			is_down = false;
			break;
		default:
			break;
		}
		break;

	case WM_KEYDOWN:

		switch (wParam)
		{
			case VK_LEFT:
				is_left = true;
				break;	
			case VK_UP:
				is_up = true;
				break;
			case VK_RIGHT:
				is_right = true;
				break;
			case VK_DOWN:
				is_down = true;
				break;
			default:
				break;
		}

		if (is_left)
			x += -delta;
		if (is_right)
			x += delta;
		if (is_up)
			y += -delta;
		if (is_down)
			y += delta;

		SetPlayerSpeed(&player, x,y);
		MyRedrawWindow(hWnd);
		break;

	 case WM_MOUSEWHEEL:

		wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
		{
			x = 0;
			y = 0;

			if ((wParam & MK_SHIFT) == MK_SHIFT)
			{
				x = delta;
			}
			else
			{
				y = delta;
			}
			SetPlayerSpeed(&player, x, y);
		}
		for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
		{
			if ((wParam & MK_SHIFT) == MK_SHIFT)
			{
				x = -delta;
			}
			else
			{
				y = -delta;
			}
			SetPlayerSpeed(&player, x, y);
		}
		MyRedrawWindow(hWnd);
		break;

	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);

		DrawPlayer(&player, hdc, hdcMemSurface);

		EndPaint(hWnd, &ps);
		break;

	case WM_LBUTTONDBLCLK:

		MessageBox(hWnd, "Hello, Ksis!", "Message", MB_OK);
		break;

	case WM_LBUTTONDOWN:
		x = (int)LOWORD(lParam);
		y = (int)HIWORD(lParam);
		if (IsFocusOnPlayer(&player, x, y))
		{
			is_player_catch = true;
		}
		break;

	case WM_LBUTTONUP:
		if (is_player_catch)
			is_player_catch = false;
		break;

	case WM_MOUSEMOVE:

		if ((wParam & MK_LBUTTON) == MK_LBUTTON)
		{
			x = (int)LOWORD(lParam);
			y = (int)HIWORD(lParam);
			if (is_player_catch)
			{
				SetPlayerCoordinates(&player, x, y);
			}
			MyRedrawWindow(hWnd);
		}
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MyRedrawWindow(HWND hWnd)
{
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

bool IsFocusOnPlayer(ControlObject* object, int x, int y)
{
	if ((object->x <= x) && (object->x + object->width >= x))
	{
		if ((object->y <= y) && (object->y + object->height >= y))
			return true;
	}
	return false;
}

void SetPlayerCoordinates(ControlObject* object, int x,int y)
{
	object->x = x;
	object->y = y;
	object->speed_x = 0;
	object->speed_y = 0;
}

void InitializePlayer(ControlObject* object,int x, int y,int width, int height,Image image)
{
	object->x = x;
	object->y = y;
	object->width = width;
	object->height = height;
	object->image = image;
	object->speed_x = 0;
	object->speed_y = 0;
}

void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y)
{
	if(speed_x != 0)
		object->speed_x = speed_x;
	if (speed_y != 0)
		object->speed_y = speed_y;
}

void MovePlayer(ControlObject* object, RECT gameField)
{
	static int counter = 0;

	LONG width = gameField.right - gameField.left;
	LONG height = gameField.bottom - gameField.top;

	if ((object->x + object->speed_x) <= 0)
	{
		object->x = 0;
		object->speed_x = -object->speed_x;
	}
	else if ((object->x + object->width + object->speed_x) >= width)
	{
		object->x = width - object->width;
		object->speed_x = -object->speed_x;
	}
	else
	{
		object->x += object->speed_x;
	}


	if ((object->y + object->speed_y) <= 0)
	{
		object->y = 0;
		object->speed_y = -object->speed_y;
	}
	else if ((object->y + object->height + object->speed_y) >= height)
	{
		object->y = height - object->height;
		object->speed_y = -object->speed_y;
	}
	else
	{
		object->y += object->speed_y;
	}

	--counter;

	if ((object->speed_x > 0) && (counter < 0))
		object->speed_x--;
	else if ((object->speed_x < 0) && (counter < 0))
		object->speed_x++;

	if ((object->speed_y > 0) && (counter < 0))
		object->speed_y--;
	else if ((object->speed_y < 0) && (counter < 0))
		object->speed_y++;
	
	if (counter < 0)
		counter = 8;

}

void DrawPlayer(ControlObject* object, HDC hdc,  HDC hdcMemSurface)
{
	SelectObject(hdcMemSurface, object->image.hBitmap);
	TransparentBlt(
		hdc,//Дескриптор целевого контекста устройства.
		object->x,//Координата x в логических единицах верхнего левого угла прямоугольника назначения.
		object->y,//Координата y в логических единицах верхнего левого угла прямоугольника назначения.
		object->width,//Ширина в логических единицах целевого прямоугольника.
		object->height,//Высота в логических единицах целевого прямоугольника.
		hdcMemSurface,//Дескриптор исходного контекста устройства.
		0,//Координата x в логических единицах исходного прямоугольника.
		0,//Y-координата в логических единицах исходного прямоугольника.
		object->image.width,//Ширина в логических единицах исходного прямоугольника.
		object->image.height,//Высота в логических единицах исходного прямоугольника
		RGB(255, 255, 255)//Цвет RGB в исходном растровом изображении считается прозрачным.
	);
	//Rectangle(hdc, object->x, object->y, object->x + PLAYERWIDTH, object->y + PLAYERHEIGHT);
	//Ellipse(hdc, object->x, object->y, object->x + PLAYERWIDTH, object->y + PLAYERHEIGHT);

}

void OnWindowResize(ControlObject* object, RECT gameField)
{
	LONG width = gameField.right - gameField.left;
	LONG height = gameField.bottom - gameField.top;

	if (object->x + object->width > width)
	{
		object->x = width - object->width;
	}

	if (object->y + object->height > height)
	{
		object->y = height - object->width;
	}
}

static struct Image loadImage(LPCTSTR path, HINSTANCE hInstance)
{
	struct Image image;

	image.hBitmap = (HBITMAP)LoadImage(
		hInstance,
		path,
		IMAGE_BITMAP,
		0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS
	);

	BITMAP bitmapInfo;
	GetObject(image.hBitmap, sizeof(bitmapInfo), &bitmapInfo);
	image.width = bitmapInfo.bmWidth;
	image.height = bitmapInfo.bmHeight;


	return image;
}