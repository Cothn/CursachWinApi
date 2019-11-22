#include <windows.h>
#include <wingdi.h>

typedef struct GameObject
{
	int delta_x;
	int delta_y;
	int x;
	int y;

} GameObject;



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MyRedrawWindow(HWND hWnd);
void InitializeGame(GameObject* player, GameObject* enemys, int x, int y, int edge);
void InitializeEnemy(GameObject* objects, int edge);
//void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y);
void MovePlayer(GameObject* object, int width, int height);
void MoveEnemy(GameObject** objects);
void DrawGameField(GameObject* object, HDC hdc, HDC hdcMemSurface);

static LPSTR OverMessage = (LPSTR)"Game Over!";
static int CELL_SIZE = 15;
static int ENEMY_MAX_SPEED = 12;
static GameObject player;

static HDC hdcMemSurface;
static bool GameEnd = false;
const int M = 35;
const int N = 40;
int CountEnemy =20;

int gameField[M][N];
GameObject* enemys;

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
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		1+(N+1)*CELL_SIZE, 24+(M+1)*CELL_SIZE, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	 enemys = new GameObject[N/2];
	InitializeGame(&player, enemys, 0, 0, 1);


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
		if (!GameEnd) {
			MovePlayer(&player, N, M);
		}
		else
		{
			GameEnd = false;
			InitializeGame(&player, enemys, 0, 0, 1);
			MessageBox(hWnd, OverMessage, OverMessage, MB_OK | MB_APPLMODAL);
		}
		MyRedrawWindow(hWnd);
		break;

	case WM_CREATE:
	{
		const HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		const HDC hdcWin = GetDC(hWnd);
		hdcMemSurface = CreateCompatibleDC(hdcWin);
		ReleaseDC(hWnd, hdcWin);
		SetTimer(hWnd, 1, 1000/20, NULL);
		return 0;
	}

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


void InitializeGame(GameObject* player, GameObject* enemys, int x, int y, int edge)
{

	//инициализация края
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			if (i < edge || j < edge || i >= M - edge || j >= N - edge)  gameField[i][j] = 1;
			else gameField[i][j] = 0;
			

	//Инициализация игрока
	player->x = x;
	player->y = y;
	player->delta_x = 0;
	player->delta_y = 0;

	InitializeEnemy(enemys, edge);
}

void InitializeEnemy(GameObject* objects, int edge)
{
	//Инициализация противников
	for (int i = 0; i < CountEnemy; i++)
	{
		objects[i].x = edge + rand() % (N - edge * 2);
		objects[i].y = edge + rand() % (M - edge * 2);
		objects[i].delta_x = ENEMY_MAX_SPEED / 2 - rand() % ENEMY_MAX_SPEED;
		objects[i].delta_y = ENEMY_MAX_SPEED / 2 - rand() % ENEMY_MAX_SPEED;
	}
}

void MovePlayer(GameObject* object, int width, int height)
{

	object->x += object->delta_x;
	if ((object->x ) <= 0)
	{
		object->x = 0;
		object->delta_x = 0;
	}
	else if ((object->x) >= width)
	{
		object->x = width-1;
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
		object->y = height-1 ;
		object->delta_y = 0;
	}
	
	//проверка конечной точки
	if (gameField[object->y][object->x] == 2) GameEnd = true;
	if (gameField[object->y][object->x] == 0) gameField[object->y][object->x] = 2;

}

void MoveEnemy(GameObject** objects)
{
	for (int i = 0; i < CountEnemy; i++)
	{
		objects[i]->x += objects[i]->delta_x;
		if (gameField[objects[i]->y][objects[i]->x] == 1)
		{
			objects[i]->delta_x *= -1;
			objects[i]->x += objects[i]->delta_x;
		}

		objects[i]->y += objects[i]->delta_y;
		if (gameField[objects[i]->y][objects[i]->x] == 1)
		{
			objects[i]->delta_y *= -1;
			objects[i]->y += objects[i]->delta_y;
		}

		//проверка конечной точки
		if (gameField[objects[i]->y][objects[i]->x] == 2) {
			GameEnd = true;
		}
	}


}



void DrawGameField(GameObject* object, HDC hdc,  HDC hdcMemSurface)
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


