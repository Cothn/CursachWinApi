#include <windows.h>
#include <wingdi.h>

typedef struct ControlObject
{
	int delta_x;
	int delta_y;
	int x;
	int y;

} ControlObject;



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MyRedrawWindow(HWND hWnd);
void InitializeGame(ControlObject* object, int x, int y, int edge);
//void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y);
void MovePlayer(ControlObject* object, int width, int height);
void MoveEnemy(ControlObject* object, RECT gameWindow);
void DrawGameField(ControlObject* object, HDC hdc, HDC hdcMemSurface);


static int CELL_SIZE = 15;
static ControlObject player;
static HDC hdcMemSurface;
static bool GameEnd = false;
const int M = 40;
const int N = 40;

int gameField[M][N];



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

	hWnd = CreateWindow("HelloKsisClass", "Hello, Xonix!",
		WS_OVERLAPPEDWINDOW, CELL_SIZE*5, CELL_SIZE * 5,
		(M+1)*CELL_SIZE, (N+1)*CELL_SIZE, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	InitializeGame(&player, 0, 0, 1);


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
		MovePlayer(&player, N, M);
		MyRedrawWindow(hWnd);
		break;
	
	//case WM_SIZE:
	//	OnWindowResize(&player, rect);
	//	MyRedrawWindow(hWnd);
	//	break;
	
	case WM_CREATE:
	{
		const HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		const HDC hdcWin = GetDC(hWnd);
		hdcMemSurface = CreateCompatibleDC(hdcWin);
		ReleaseDC(hWnd, hdcWin);
		SetTimer(hWnd, 1, 1000/20, NULL);
		return 0;
	}

	//case WM_KEYUP:
	//{
	//	switch (wParam)
	//	{
	//	case VK_LEFT:
	//		is_left = false;
	//		break;
	//	case VK_UP:
	//		is_up = false;
	//		break;
	//	case VK_RIGHT:
	//		is_right = false;
	//		break;
	//	case VK_DOWN:
	//		is_down = false;
	//		break;
	//	default:
	//		break;
	//	}
	//	break;
	//}

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
			player.delta_x = -1;
			player.delta_y = 0;
			break;
		case VK_UP:
			player.delta_x = 0;
			player.delta_y = -1;
			break;
		case VK_RIGHT:
			player.delta_x = 1;
			player.delta_y = 0;
			break;
		case VK_DOWN:
			player.delta_x = 0;
			player.delta_y = 1;
			break;
		default:
			break;
		}

		//switch (wParam)
		//{
		//case VK_LEFT:
		//	is_left = true;
		//	break;
		//case VK_UP:
		//	is_up = true;
		//	break;
		//case VK_RIGHT:
		//	is_right = true;
		//	break;
		//case VK_DOWN:
		//	is_down = true;
		//	break;
		//default:
		//	break;
		//}

		//if (is_left)
		//	x += -delta;
		//if (is_right)
		//	x += delta;
		//if (is_up)
		//	y += -delta;
		//if (is_down)
		//	y += delta;

		//SetPlayerSpeed(&player, x, y);
		MyRedrawWindow(hWnd);
		break;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		DrawGameField(&player, hdc, hdcMemSurface);

		EndPaint(hWnd, &ps);
		break;
	}

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


void SetPlayerCoordinates(ControlObject* object, int x,int y)
{
	object->x = x;
	object->y = y;
	object->delta_x = 0;
	object->delta_y = 0;
}

void InitializeGame(ControlObject* object,int x, int y, int edge)
{

	//инициализация края
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			if (i < edge || j < edge || i >= M - edge || j >= N - edge)  gameField[i][j] = 1;

	//Инициализация игрока
	object->x = x;
	object->y = y;
	object->delta_x = 0;
	object->delta_y = 0;
}

//void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y)
//{
//	if(speed_x != 0)
//		object->speed_x = speed_x;
//	if (speed_y != 0)
//		object->speed_y = speed_y;
//}

void MovePlayer(ControlObject* object, int width, int height)
{
	static int counter = 0;

	object->x += object->delta_x;
	if ((object->x ) <= 0)
	{
		object->x = 0;
		object->delta_x = 0;
	}
	else if ((object->x) >= width)
	{
		object->x = width;
		object->delta_x = 0;
	}


	object->y += object->delta_y;
	if ((object->y ) <= 0)
	{
		object->y = 0;
		object->delta_y = 0;
	}
	else if ((object->y ) >= height)
	{
		object->y = height ;
		object->delta_y = 0;
	}
	
	//проверка конечной точки
	if (gameField[object->y][object->x] == 2) GameEnd = true;
	if (gameField[object->y][object->x] == 0) gameField[object->y][object->x] = 2;

}

void MoveEnemy(ControlObject* object, RECT gameField)
{
	static int counter = 0;

	LONG width = gameField.right - gameField.left;
	LONG height = gameField.bottom - gameField.top;

	if ((object->x + object->delta_x) <= 0)
	{
		object->x = 0;
		object->delta_x = -object->delta_x;
	}
	else if ((object->x + CELL_SIZE + object->delta_x) >= width)
	{
		object->x = width - CELL_SIZE;
		object->delta_x = -object->delta_x;
	}
	else
	{
		object->x += object->delta_x;
	}


	if ((object->y + object->delta_y) <= 0)
	{
		object->y = 0;
		object->delta_y = -object->delta_y;
	}
	else if ((object->y + CELL_SIZE + object->delta_y) >= height)
	{
		object->y = height - CELL_SIZE;
		object->delta_y = -object->delta_y;
	}
	else
	{
		object->y += object->delta_y;
	}

	--counter;

	if ((object->delta_x > 0) && (counter < 0))
		object->delta_x--;
	else if ((object->delta_x < 0) && (counter < 0))
		object->delta_x++;

	if ((object->delta_y > 0) && (counter < 0))
		object->delta_y--;
	else if ((object->delta_y < 0) && (counter < 0))
		object->delta_y++;

	if (counter < 0)
		counter = 8;

}



void DrawGameField(ControlObject* object, HDC hdc,  HDC hdcMemSurface)
{
	//SelectObject(hdcMemSurface, object->image.hBitmap);
	//TransparentBlt(
	//	hdc,//Дескриптор целевого контекста устройства.
	//	object->x,//Координата x в логических единицах верхнего левого угла прямоугольника назначения.
	//	object->y,//Координата y в логических единицах верхнего левого угла прямоугольника назначения.
	//	object->width,//Ширина в логических единицах целевого прямоугольника.
	//	object->height,//Высота в логических единицах целевого прямоугольника.
	//	hdcMemSurface,//Дескриптор исходного контекста устройства.
	//	0,//Координата x в логических единицах исходного прямоугольника.
	//	0,//Y-координата в логических единицах исходного прямоугольника.
	//	object->image.width,//Ширина в логических единицах исходного прямоугольника.
	//	object->image.height,//Высота в логических единицах исходного прямоугольника
	//	RGB(255, 255, 255)//Цвет RGB в исходном растровом изображении считается прозрачным.
	//);



	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++) {
			if (gameField[i][j] == 1)
			{
				Rectangle(hdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
			}
			if (gameField[i][j] == 2)
			{
				Rectangle(hdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
				Rectangle(hdc, j * CELL_SIZE + 1, i * CELL_SIZE + 1, j * CELL_SIZE + CELL_SIZE - 1, i * CELL_SIZE + CELL_SIZE - 1);
			}
		}
	}

	Rectangle(hdc, object->x * CELL_SIZE, object->y * CELL_SIZE, object->x * CELL_SIZE + CELL_SIZE, object->y * CELL_SIZE + CELL_SIZE);
	Rectangle(hdc, object->x * CELL_SIZE+5, object->y * CELL_SIZE + 5, object->x * CELL_SIZE + CELL_SIZE - 5, object->y * CELL_SIZE + CELL_SIZE - 5);

}



