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
void InitializeGame(GameObject* player, GameObject* enemy, int x, int y, int edge);
void InitializeEnemy(GameObject* objects, int edge);
//void SetPlayerSpeed(ControlObject* object, int speed_x, int speed_y);
void MovePlayer(GameObject* object, GameObject* enemy, int width, int height);
void MoveEnemy(GameObject* objects);
void DrawGameField(GameObject* object, GameObject* enemy, HDC hdc, HDC hdcMemSurface);
void UpdateField();
void drop(int y, int x);

static LPSTR OverMessage = (LPSTR)"Game Over!";
static LPSTR WinMessage = (LPSTR)"Player Win!";
static int CELL_SIZE = 15;
static int ENEMY_MAX_SPEED = (12 % (CELL_SIZE)) * 2;
static GameObject player;

static HDC hdcMemSurface;
static bool GameEnd = false;
static bool PlayerWin = false;
const int M = 35;
const int N = 40;
const int WIN_PERCENT = 70;
int CountEnemy =5;

int gameField[M][N];
GameObject* Enemys;
class stack
{
	int a[N * M * 2+1];
	public:
		int top;
		stack()
		{
			top = 0;
		}
		void push(int i)
		{
			a[top] = i;
			top++;
		}
		int pop() 
		{
			return(a[--top]);
		}
};

stack s;

//stack::stack()
//{
//	top = 0;
//}



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


	Enemys = new GameObject[N / 2];
	InitializeGame(&player, Enemys, 0, 0, 1);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
	HBRUSH hBrush; //создаЄм объект-кисть
	PAINTSTRUCT ps;
	HDC hdc; //создаЄм контекст устройства

	RECT rect;
	GetClientRect(hWnd, &rect);
	switch (message)
	{

	case WM_TIMER:
		if (!GameEnd) {
			MovePlayer(&player, Enemys, N, M);
		}
		else
		{
			GameEnd = false;

			if(PlayerWin)
			{
				PlayerWin = false;
				MessageBox(hWnd, WinMessage, WinMessage, MB_OK | MB_APPLMODAL);
				InitializeGame(&player, Enemys, 0, 0, 1);
			}
			else {
				InitializeGame(&player, Enemys, 0, 0, 1);
				MessageBox(hWnd, OverMessage, OverMessage, MB_OK | MB_APPLMODAL);
			}

		}
		MyRedrawWindow(hWnd);
		break;

	case WM_CREATE:
	{
		const HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
		const HDC hdcWin = GetDC(hWnd);
		hdcMemSurface = CreateCompatibleDC(hdcWin);
		ReleaseDC(hWnd, hdcWin);
		SetTimer(hWnd, 1, 1000/15, NULL);
		return 0;
	}

	case WM_ERASEBKGND:
		return 1;
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

		DrawGameField(&player, Enemys, hdc, hdcMemSurface);
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


void InitializeGame(GameObject* player, GameObject* enemy, int x, int y, int edge)
{

	//инициализаци€ кра€
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			if (i < edge || j < edge || i >= M - edge || j >= N - edge)  gameField[i][j] = 1;
			else gameField[i][j] = 0;
			

	//»нициализаци€ игрока
	player->x = x;
	player->y = y;
	player->delta_x = 0;
	player->delta_y = 0;

	InitializeEnemy(enemy, edge);
}

void InitializeEnemy(GameObject* objects, int edge)
{
	//»нициализаци€ противников
	for (int i = 0; i < CountEnemy; i++)
	{
		objects[i].x = (edge + rand() % (N - edge * 2))* CELL_SIZE;
		objects[i].y = (edge + rand() % (M - edge * 2))* CELL_SIZE;
		objects[i].delta_x = ENEMY_MAX_SPEED / 2 - rand() % ENEMY_MAX_SPEED;
		objects[i].delta_y = ENEMY_MAX_SPEED / 2 - rand() % ENEMY_MAX_SPEED;
	}
}

void MovePlayer(GameObject* object, GameObject* enemy, int width, int height)
{

	//проверка конечной точки
	if ((object->delta_x != 0) || (object->delta_y != 0)) {

		object->x += object->delta_x;
		if ((object->x) < 0)
		{
			object->x = 0;
		}
		else if ((object->x) >= width)
		{
			object->x = width - 1;
		}

		object->y += object->delta_y;
		if ((object->y) < 0)
		{
			object->y = 0;
		}
		else if ((object->y) >= height)
		{
			object->y = height - 1;
		}

		switch (gameField[object->y][object->x])
		{
			case 0:
			{
				gameField[object->y][object->x] = 2;
				break;
			}
			case 1:
			{
				if (gameField[object->y- object->delta_y][object->x - object->delta_x] == 2) {
					object->delta_x = 0;
					object->delta_y = 0;

					//определение свободных областей
					for (int i = 0; i < CountEnemy; i++)
						drop(Enemys[i].x / CELL_SIZE, Enemys[i].y / CELL_SIZE);

					UpdateField(); //обновление игрового пол€
				}
				break;

			}
			case 2:
			{
				GameEnd = true;
				break;
			}
		}

	}
	MoveEnemy(enemy);
}



void drop(int x, int y)
{

	s.push(x);
	s.push(y);
	while (s.top != 0) {
		if (gameField[y][x] == 0) gameField[y][x] = -1;
		if (gameField[y - 1][x] == 0)
		{
			s.push(x);
			s.push(y);
			y--;
		}
		else if (gameField[y + 1][x] == 0)
		{
			s.push(x);
			s.push(y);
			y++;
		}
		else if (gameField[y][x - 1] == 0)
		{
			s.push(x);
			s.push(y);
			x--;
		}
		else if (gameField[y][x + 1] == 0)
		{
			s.push(x);
			s.push(y);
			x++;
		}
		else
		{
			y = s.pop();
			x = s.pop();
		}
	}

}



void UpdateField()
{
	int ColEnemyCell= 0;
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			if (gameField[i][j] == -1)
			{
				gameField[i][j] = 0;
				ColEnemyCell++;
			}
			else gameField[i][j] = 1;

	//проверка на победу игрока
	float EnemyPercent = ((float)ColEnemyCell / (N * M))*100;
	if (EnemyPercent < 100 - WIN_PERCENT)
	{
		GameEnd = true;
		PlayerWin = true;
	}
}

void MoveEnemy(GameObject* objects)
{
	for (int i = 0; i < CountEnemy; i++)
	{
		objects[i].x += objects[i].delta_x;
		if ((gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE] == 1)
			|| (gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE+1] == 1))
		{
			objects[i].delta_x = -objects[i].delta_x;

			if (objects[i].delta_x > 0) {
				objects[i].x += (CELL_SIZE - objects[i].x % CELL_SIZE) * 2; //отскок вправо
			}
			else
			{

				objects[i].x += (objects[i].x % CELL_SIZE) * -2; //отскок влево
			}
		}

		objects[i].y += objects[i].delta_y;
		if ((gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE] == 1)
			|| (gameField[objects[i].y / CELL_SIZE + 1][objects[i].x / CELL_SIZE ] == 1))
		{
			objects[i].delta_y = -objects[i].delta_y;
			objects[i].y += objects[i].delta_y;
			if (objects[i].delta_y > 0) {
				objects[i].y += (CELL_SIZE - objects[i].y % CELL_SIZE) * 2; //отскок вниз
			}
			else
			{
				objects[i].y -= (objects[i].y % CELL_SIZE) * 2; //отскок вверх
			}
		}

		//проверка конечной точки
		if (gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE] == 2) {
			GameEnd = true;
		}
	}


}



void DrawGameField(GameObject* object, GameObject* enemy, HDC hdc,  HDC hdcMemSurface)
{

	HDC BuffHdc = CreateCompatibleDC(hdc);
	HBITMAP BuffHbm = CreateCompatibleBitmap(hdc, 1 + (N + 1) * CELL_SIZE, 24 + (M + 1) * CELL_SIZE);
	HANDLE BuffHan = SelectObject(BuffHdc, BuffHbm);

	//перерисовка пол€
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++) {
			if (gameField[i][j] == 1)
			{
				Rectangle(BuffHdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);

			}
			if (gameField[i][j] == 2)
			{
				Rectangle(BuffHdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
				Rectangle(BuffHdc, j * CELL_SIZE + 1, i * CELL_SIZE + 1, j * CELL_SIZE + CELL_SIZE - 1, i * CELL_SIZE + CELL_SIZE - 1);
			}
		}
	}

	//Draw Player
	Rectangle(BuffHdc, object->x * CELL_SIZE, object->y * CELL_SIZE, object->x * CELL_SIZE + CELL_SIZE, object->y * CELL_SIZE + CELL_SIZE);
	Rectangle(BuffHdc, object->x * CELL_SIZE+5, object->y * CELL_SIZE + 5, object->x * CELL_SIZE + CELL_SIZE - 5, object->y * CELL_SIZE + CELL_SIZE - 5);

	//ѕерерисовка врагов
	for (int i = 0; i < CountEnemy; i++)
	{
		Ellipse(BuffHdc, enemy[i].x , enemy[i].y , enemy[i].x  + CELL_SIZE, enemy[i].y  + CELL_SIZE);
		Rectangle(BuffHdc, enemy[i].x + 5, enemy[i].y  + 5, enemy[i].x + CELL_SIZE - 5, enemy[i].y + CELL_SIZE - 5);

	}

	BitBlt(hdc, 0, 0, 1 + (N + 1) * CELL_SIZE, 24 + (M + 1) * CELL_SIZE, BuffHdc, 0, 0, SRCCOPY);

	//освобождаем пам€ть
	SelectObject(BuffHdc, BuffHan);
	DeleteObject(BuffHbm);
	DeleteObject(BuffHdc);

}

