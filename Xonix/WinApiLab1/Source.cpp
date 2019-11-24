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
void InitializeGame(GameObject* player, GameObject* enemy, int start_x, int start_y, int edge);
void InitializeEnemy(GameObject* objects, int edge);
float MovePlayer(GameObject* object, GameObject* enemy, float enemyPercent);
void MoveEnemy(GameObject* objects);
void DrawGameField(GameObject* object, GameObject* enemy, float enemyPercent, int enemyMaxSpeed, HDC hdc);
float UpdateField();
void CheckCell(int y, int x);


//константы приложения
#define PercentMessage "Percent: "
#define SpeedMessage "Speed: "
#define OverMessage "Game Over!"
#define WinMessage "Player Win!"
#define CELL_SIZE 15
#define Height  35
#define Width 45
#define WIN_PERCENT 70
#define STATISTIC_PIXEL 20
const int MaxCountEnemy = Width / 2;
const int MaxStackSize = Width * Height * 2 + 1;
const int Edge = 1;

//размеры окна в пикселях
const int pixelWidth = 16 + (Width) * CELL_SIZE;
const int pixelHeight = 16 +(Height) * CELL_SIZE + STATISTIC_PIXEL*2;

//флаги
bool GameEnd = false;
bool PlayerWin = false;

//изменяемые параметры
int CountEnemy =5;
int EnemyMaxSpeed = (12 % (CELL_SIZE));

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

//Глобальные переменные
float EnemyPercent = 100;
int gameField[Height][Width];
GameObject* Enemys = new GameObject[MaxCountEnemy];
GameObject Player;


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
			InitializeGame(&Player, Enemys, 0, 0, Edge);
			SetTimer(hWnd, 1, 1000/15, NULL);
			return 0;
		}

		case WM_TIMER:
		{
			if (!GameEnd) {
				EnemyPercent = MovePlayer(&Player, Enemys, EnemyPercent);
				MoveEnemy(Enemys);
			}
			else
			{
				GameEnd = false;
				if (PlayerWin)
				{
					PlayerWin = false;
					MessageBox(hWnd, WinMessage, WinMessage, MB_OK | MB_APPLMODAL);
					EnemyPercent = 100;
					InitializeGame(&Player, Enemys, 0, 0, Edge);
				}
				else {
					EnemyPercent = 100;
					InitializeGame(&Player, Enemys, 0, 0, Edge);
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
				Player.delta_x = -1;
				Player.delta_y = 0;
				break;
			case VK_UP:
				Player.delta_x = 0;
				Player.delta_y = -1;
				break;
			case VK_RIGHT:
				Player.delta_x = 1;
				Player.delta_y = 0;
				break;
			case VK_DOWN:
				Player.delta_x = 0;
				Player.delta_y = 1;
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
					InitializeGame(&Player, Enemys, 0, 0, 1);
				}
				break;
			case VK_SUBTRACT:
				if (CountEnemy > 0)
				{
					CountEnemy--;
					InitializeGame(&Player, Enemys, 0, 0, 1);
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
				InitializeGame(&Player, Enemys, 0, 0, 1);
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
			DrawGameField(&Player, Enemys, EnemyPercent, EnemyMaxSpeed, hdc);
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

void InitializeGame(GameObject* player, GameObject* enemy, int start_x, int start_y, int edge)
{
	//инициализация края
	for (int i = 0; i < Height; i++)
		for (int j = 0; j < Width; j++)
			if (i < edge || j < edge || i >= Height - edge || j >= Width - edge)  gameField[i][j] = 1;
			else gameField[i][j] = 0;
			

	//Инициализация игрока
	player->x = start_x;
	player->y = start_y;
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

float MovePlayer(GameObject* object, GameObject* enemy, float enemyPercent)
{
	if ((object->delta_x != 0) || (object->delta_y != 0)) {


		object->x += object->delta_x;
		if ((object->x) < 0)
		{
			object->x = 0;
		}
		else if ((object->x) >= Width)
		{
			object->x = Width - 1;
		}

		object->y += object->delta_y;
		if ((object->y) < 0)
		{
			object->y = 0;
		}
		else if ((object->y) >= Height)
		{
			object->y = Height - 1;
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

					enemyPercent = UpdateField();

					//проверка на победу игрока
					if (enemyPercent < 100 - WIN_PERCENT)
					{
						GameEnd = true;
						PlayerWin = true;
					}


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
	return enemyPercent;
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

float UpdateField()
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


	//процент вражеских територии
	return ((float)ColEnemyCell / ((Width-1) * (Height-1)))*100; 
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

void DrawGameField(GameObject* object, GameObject* enemy, float enemyPercent, int enemyMaxSpeed, HDC hdc)
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

	
	//Вывод статистики
	char* stat = new char(3);
	char message [100] = PercentMessage;
	_itoa(100 - enemyPercent, stat, 10);
	strcat(message, stat);
	strcat(message, " ");
	strcat(message, SpeedMessage);
	_itoa(enemyMaxSpeed, stat, 10);
	strcat(message, stat);
	TextOut(BuffHdc, 0, (Height)* CELL_SIZE +2 , message, strlen(message));

	BitBlt(hdc, 0, 0, pixelWidth, pixelHeight, BuffHdc, 0, 0, SRCCOPY);

	//освобождаем память
	SelectObject(BuffHdc, BuffHan);
	DeleteObject(BuffHbm);
	DeleteObject(BuffHdc);

}

