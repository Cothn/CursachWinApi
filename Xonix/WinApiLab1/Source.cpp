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
void MovePlayer(GameObject* object, GameObject* enemy, int width, int height);
void MoveEnemy(GameObject* objects);
void DrawGameField(GameObject* object, GameObject* enemy, HDC hdc);
void UpdateField();
void CheckCell(int y, int x);


//константы приложения
static const LPSTR OverMessage = (LPSTR)"Game Over!";
static const LPSTR WinMessage = (LPSTR)"Player Win!";
static const int CELL_SIZE = 15;
const int Height = 35;
const int Width = 40;
const int WIN_PERCENT = 70;
const int MaxCountEnemy = Width / 2;
const int MaxStackSize = Width * Height * 2 + 1;

//флаги
bool GameEnd = false;
bool PlayerWin = false;

//изменяемые параметры
int CountEnemy =5;
int EnemyMaxSpeed = (12 % (CELL_SIZE));

//размеры окна в пикселях
const int pixelWidth = 1 + (Width + 1) * CELL_SIZE;
const int pixelHeight = 24 + (Height + 1) * CELL_SIZE;

//Глобальные переменные
int gameField[Height][Width];
GameObject* Enemys = new GameObject[MaxCountEnemy];
GameObject player;

class stack
{
	int obj[MaxStackSize];
public:
	int top;
	stack()
	{
		top = 0;
	}
	void push(int i)
	{
		obj[top] = i;
		top++;
	}
	int pop()
	{
		return(obj[--top]);
	}
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = NULL;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "XonixClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow("XonixClass", "Hello, Xonix!",
		WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		pixelWidth, pixelHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

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

	PAINTSTRUCT ps;
	HDC hdc; //создаём контекст устройства

	switch (message)
	{

		case WM_CREATE:
		{
			// инициализация игры
			InitializeGame(&player, Enemys, 0, 0, 1);
			SetTimer(hWnd, 1, 1000/15, NULL);
			return 0;
		}

		case WM_TIMER:
		{
			if (!GameEnd) {
				MovePlayer(&player, Enemys, Width, Height);
			}
			else
			{
				GameEnd = false;

				if (PlayerWin)
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

		case WM_KEYUP:
		{
			switch (wParam)
			{
			case VK_ADD:
				if (CountEnemy < MaxCountEnemy)
				{
					CountEnemy++;
					InitializeGame(&player, Enemys, 0, 0, 1);
				}
				break;
			case VK_SUBTRACT:
				if (CountEnemy > 0)
				{
					CountEnemy--;
					InitializeGame(&player, Enemys, 0, 0, 1);
				}
				break;
			case VK_MULTIPLY:
				if (EnemyMaxSpeed < CELL_SIZE)
				{
					EnemyMaxSpeed++;
				}
				break;
			case VK_DIVIDE:
				if (EnemyMaxSpeed > 1)
				{
					EnemyMaxSpeed--;
				}
				break;
			case VK_END:
				InitializeGame(&player, Enemys, 0, 0, 1);
				break;

			default:
				break;
			}
		}

		case WM_ERASEBKGND:
			return 1;

		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			ps.fErase = false;
			DrawGameField(&player, Enemys, hdc);
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
	//инициализация края
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++)
			if (i < edge || j < edge || i >= Height - edge || j >= Width - edge)  gameField[i][j] = 1;
			else gameField[i][j] = 0;
			

	//Инициализация игрока
	player->x = x;
	player->y = y;
	player->delta_x = 0;
	player->delta_y = 0;

	InitializeEnemy(enemy, edge);
}

void InitializeEnemy(GameObject* objects, int edge)
{
	//Инициализация противников
	for (int i = 0; i < CountEnemy; i++)
	{
		objects[i].x = (edge + rand() % (Width - edge * 2))* CELL_SIZE;
		objects[i].y = (edge + rand() % (Height - edge * 2))* CELL_SIZE;
		objects[i].delta_x = EnemyMaxSpeed - rand() % (EnemyMaxSpeed * 2 + 1);
		objects[i].delta_y = EnemyMaxSpeed - rand() % (EnemyMaxSpeed * 2 + 1);
	}
}

void MovePlayer(GameObject* object, GameObject* enemy, int width, int height)
{
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

		//проверка конечной точки
		switch (gameField[object->y][object->x])
		{
			case 0:
			{
				gameField[object->y][object->x] = 2;
				break;
			}
			case 1:
			{
				//проверяем наличие "хвоста"
				if (gameField[object->y- object->delta_y][object->x - object->delta_x] == 2) {
					object->delta_x = 0;
					object->delta_y = 0;

					//определение вражеской територии
					for (int i = 0; i < CountEnemy; i++)
						CheckCell(enemy[i].x / CELL_SIZE, enemy[i].y / CELL_SIZE);

					UpdateField();
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

void CheckCell(int x, int y)
{
	stack CordinateStack;

	CordinateStack.push(x);
	CordinateStack.push(y);
	while (CordinateStack.top != 0) {
		if (gameField[y][x] == 0) gameField[y][x] = -1;
		if (gameField[y - 1][x] == 0)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y--;
		}
		else if (gameField[y + 1][x] == 0)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y++;
		}
		else if (gameField[y][x - 1] == 0)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			x--;
		}
		else if (gameField[y][x + 1] == 0)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			x++;
		}
		else
		{
			y = CordinateStack.pop();
			x = CordinateStack.pop();
		}
	}

}

void UpdateField()
{
	int ColEnemyCell= 0;
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++)
			if (gameField[i][j] == -1)
			{
				gameField[i][j] = 0;
				ColEnemyCell++;
			}
			else gameField[i][j] = 1;

	//проверка на победу игрока
	float EnemyPercent = ((float)ColEnemyCell / (Width * Height))*100;
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
			objects[i].x += objects[i].delta_x;
		}

		objects[i].y += objects[i].delta_y;
		if ((gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE] == 1)
			|| (gameField[objects[i].y / CELL_SIZE + 1][objects[i].x / CELL_SIZE ] == 1))
		{
			objects[i].delta_y = -objects[i].delta_y;		
			objects[i].y += objects[i].delta_y;
		}

		//проверка конечной точки
		if (gameField[objects[i].y / CELL_SIZE][objects[i].x / CELL_SIZE] == 2) {
			GameEnd = true;
		}
	}


}

void DrawGameField(GameObject* object, GameObject* enemy, HDC hdc)
{
	//буферизация
	HDC BuffHdc = CreateCompatibleDC(hdc);
	HBITMAP BuffHbm = CreateCompatibleBitmap(hdc, pixelWidth, pixelHeight);
	HANDLE BuffHan = SelectObject(BuffHdc, BuffHbm);

	//перерисовка игрового поля
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++) {
			if (gameField[i][j] == 1)
			{
				Rectangle(BuffHdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
			}
			if (gameField[i][j] == 2)
			{
				Rectangle(BuffHdc, j * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE + CELL_SIZE, i * CELL_SIZE + CELL_SIZE);
				Rectangle(BuffHdc, j * CELL_SIZE + CELL_SIZE/10, i * CELL_SIZE + CELL_SIZE / 10, j * CELL_SIZE + CELL_SIZE - CELL_SIZE / 10, i * CELL_SIZE + CELL_SIZE - CELL_SIZE / 10);
			}
		}
	}

	//Draw Player
	Rectangle(BuffHdc, object->x * CELL_SIZE, object->y * CELL_SIZE, object->x * CELL_SIZE + CELL_SIZE, object->y * CELL_SIZE + CELL_SIZE);
	Ellipse(BuffHdc, object->x * CELL_SIZE+ CELL_SIZE / 3, object->y * CELL_SIZE + CELL_SIZE / 3, object->x * CELL_SIZE + CELL_SIZE - CELL_SIZE / 3, object->y * CELL_SIZE + CELL_SIZE - CELL_SIZE / 3);

	//Перерисовка врагов
	for (int i = 0; i < CountEnemy; i++)
	{
		Ellipse(BuffHdc, enemy[i].x , enemy[i].y , enemy[i].x  + CELL_SIZE, enemy[i].y  + CELL_SIZE);
		Rectangle(BuffHdc, enemy[i].x + CELL_SIZE / 3, enemy[i].y  + CELL_SIZE / 3, enemy[i].x + CELL_SIZE - CELL_SIZE / 3, enemy[i].y + CELL_SIZE - CELL_SIZE / 3);

	}

	BitBlt(hdc, 0, 0, pixelWidth, pixelHeight, BuffHdc, 0, 0, SRCCOPY);

	//освобождаем память
	SelectObject(BuffHdc, BuffHan);
	DeleteObject(BuffHbm);
	DeleteObject(BuffHdc);

}

