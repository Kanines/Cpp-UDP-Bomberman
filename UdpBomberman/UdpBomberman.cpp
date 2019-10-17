#include "stdafx.h"
#include "UdpBomberman.h"
#include "Map.h"
#include "SOIL.h"

Map map;
float TX = -24, TY = 21;
float bombs[4];
int bomb = 0;
float outBuf[BUFLEN];

// Global Variables:
HINSTANCE hInst;						// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
void MyKeyboardFunc(unsigned char Key, int x, int y);
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand(GetTickCount());
	MSG msg;
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_UDPWINDOWCLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	glEnableClientState(GL_VERTEX_ARRAY);
	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UDPWINDOWCLIENT));
	DWORD id;

	CreateThread(NULL, 0, send, hAccelTable, 0, &id);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		100, 100, 1000, 800, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UDPWINDOWCLIENT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_UDPWINDOWCLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HDC g_context = NULL;

	switch (message)
	{
	case WM_CREATE:
	{
		g_context = GetDC(hWnd);
		if (SetWindowPixelFormat(g_context) == FALSE)
			return 0;

		if (CreateViewGLContext(g_context) == FALSE)
			return 0;

		CreateMaze();
		SetTimer(hWnd, 1, 33, NULL);
		return 0;
	}
	case WM_COMMAND:

		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_SERVER:
			DWORD id;
			CreateThread(NULL, 0, server, NULL, 0, &id);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Add any drawing code here...

		RenderScene();
		SwapBuffers(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	case WM_SIZE:
	{
		int cx = LOWORD(lParam);
		int cy = HIWORD(lParam);

		GLsizei width, height;
		GLfloat aspect;
		width = cx;
		height = cy;

		if (cy == 0)
			aspect = (GLfloat)width;
		else
			aspect = (GLfloat)width / (GLfloat)height;

		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(75, aspect, 1, 50.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDrawBuffer(GL_BACK);

		glEnable(GL_LIGHTING);

		glEnable(GL_DEPTH_TEST);

		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void CreateMaze()
{

	glNewList(Floor, GL_COMPILE);
	glBegin(GL_POLYGON);
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(-20, 0, -20.0);
	glVertex3d(-20, 0, 20.0);
	glVertex3d(20, 0, 20.0);
	glVertex3d(20, 0, -20.0);
	glEnd();
	glEndList();

	map.setMap();

	glNewList(Player1, GL_COMPILE);
	glBegin(GL_POLYGON);
	glNormal3d(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(-1, 0, -1);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d(-1, 0, 1);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d(1, 0, 1);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d(1, 0, -1);
	glEnd();
	glEndList();

	glNewList(Bomb1, GL_COMPILE);
	glBegin(GL_POLYGON);
	glNormal3d(0.0, 1.0, 0.0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(-0.5, 0, -0.5);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d(-0.5, 0, 0.5);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d(0.5, 0, 0.5);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d(0.5, 0, -0.5);
	glEnd();
	glEndList();

	int num_segments = 100;
	float r = 2.75;

	glNewList(Particle, GL_COMPILE);
	glBegin(GL_POLYGON);
	glNormal3d(0.0, 1.0, 0.0);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 

		float x = r * cosf(theta);//calculate the x component 
		float y = r * sinf(theta);//calculate the y component 

		glVertex3d(x, 0, y);//output vertex 
	}
	glEnd();
	glEndList();

	float sk = 4.0;

	glNewList(Particle2, GL_COMPILE);
	glBegin(GL_POLYGON);
	glNormal3d(0.0, 1.0, 0.0);

	glTexCoord2f(0.0f, 0.33f);
	glVertex3d(-1.5*sk, 0, -0.5*sk);

	glTexCoord2f(0.0f, 0.67f);
	glVertex3d(-1.5*sk, 0, 0.5*sk);

	glTexCoord2f(0.33f, 0.67f);
	glVertex3d(-0.5*sk, 0, 1.5*sk);

	glTexCoord2f(0.33f, 1.0f);
	glVertex3d(-0.5*sk, 0, 1.5*sk);

	glTexCoord2f(0.67f, 1.0f);
	glVertex3d(0.5*sk, 0, 1.5*sk);

	glTexCoord2f(0.67f, 0.67f);
	glVertex3d(0.5*sk, 0, 0.5*sk);

	glTexCoord2f(1.0f, 0.67f);
	glVertex3d(1.5*sk, 0, 0.5*sk);

	glTexCoord2f(1.0f, 0.33f);
	glVertex3d(1.5*sk, 0, -0.5*sk);

	glTexCoord2f(0.67f, 0.33f);
	glVertex3d(0.5*sk, 0, -0.5*sk);

	glTexCoord2f(0.67f, 0.0f);
	glVertex3d(0.5*sk, 0, -1.5*sk);

	glTexCoord2f(0.33f, 0.0f);
	glVertex3d(-0.5*sk, 0, -1.5*sk);

	glTexCoord2f(0.33f, 0.33f);
	glVertex3d(-0.5*sk, 0, -0.5*sk);

	glTexCoord2f(0.0f, 0.33f);
	glVertex3d(-1.5*sk, 0, -0.5*sk);

	glEnd();
	glEndList();
}
float rand_FloatRange(float a, float b)
{
	return ((b - a)*((float)rand() / RAND_MAX)) + a;
}
void RenderScene()
{
	GLfloat BrownSurface[] = { 0.6f, 0.4f, 0.0f, 1.0f };
	GLfloat LightAmbient[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	GLfloat LightDiffuse[] = { 0.8f, 0.8f, 0.8f, 0.8f };
	GLfloat LightPosition[] = { 0.0f, 1.0f, 1.0f, 0.0f };
	GLfloat WhiteSurface[] = { 255, 255, 255, 1.0f };
	GLfloat OrangeSurface[] = { 1.0f, 0.6f, 0.0f, 1.0f };
	GLfloat GreenSurface[] = { 0, 1, 0.0f, 1.0f };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);		//1 składowa: światło otaczające (bezkierunkowe)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);		//2 składowa: światło rozproszone (kierunkowe)
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	gluLookAt(0, 40, 0, 0, 0, 0, 1, 0, 0);

	map.draw();

	int current = outBuf[0];

	GLuint textureBomb;
	GLuint textureExplosion;


	textureBomb = SOIL_load_OGL_texture("../Data/bomb.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);

	textureExplosion = SOIL_load_OGL_texture("../Data/explosion.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);
	GLuint texturePlayer[4];
	texturePlayer[0] = SOIL_load_OGL_texture("../Data/marioRed.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);
	texturePlayer[1] = SOIL_load_OGL_texture("../Data/marioGreen.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);
	texturePlayer[2] = SOIL_load_OGL_texture("../Data/marioBlue.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);
	texturePlayer[3] = SOIL_load_OGL_texture("../Data/marioBlack.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
		SOIL_FLAG_MIPMAPS |
		SOIL_FLAG_COMPRESS_TO_DXT);

	static bool flag = false;
	int i;
	for (i = 2; i < 5 * outBuf[1] + 2; i += 5)
	{

		glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, BrownSurface);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, texturePlayer[(i - 2) / 5]);
		glTranslatef(outBuf[i + 1], 0.01, outBuf[i]);
		glCallList(Player1);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, GreenSurface);
		glPushMatrix();
		glTranslatef(outBuf[i + 3], 0.01, outBuf[i + 2]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureBomb);
		glCallList(Bomb1);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();


		// Polygon TEST
			/*
			glPushMatrix();
			glTranslatef(outBuf[i + 1], 0.01, outBuf[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, OrangeSurface);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
			//glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, textureExplosion);
			glCallList(Particle2);
			glPopMatrix();
			*/

		if (outBuf[i + 2] != bombs[(i - 2) / 5] && outBuf[i + 2] == -10000 && flag)
		{

			glPushMatrix();
			glTranslatef(outBuf[i + 3], 0.01, bombs[(i - 2) / 5]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, OrangeSurface);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
			//glEnable(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, textureExplosion);
			glCallList(Particle);
			glPopMatrix();
			//glDisable(GL_TEXTURE_2D);

			int j = outBuf[1] * 5 + 2;
			while (outBuf[j] != 99999 || j == 250)
			{
				map.set((int)outBuf[j], (int)outBuf[j + 1], Floor);
				j += 2;
			}

		}
		flag = true;
		bombs[(i - 2) / 5] = outBuf[i + 2];
	}

	glPopMatrix();
	glFlush();
}
DWORD WINAPI read(LPVOID lpParameter) {

	SOCKET si = (SOCKET)lpParameter;

	struct  sockaddr_in  sc;
	int lenc = sizeof(sc);
	bool flag = true;
	while (1) {
		if (recvfrom(si, (char*)outBuf, BUFLEN * sizeof(float), 0, (struct sockaddr *) &sc, &lenc) != SOCKET_ERROR)
		{
			if (flag)
			{
				TX = outBuf[5 * (int)outBuf[0] + 2];
				TY = outBuf[5 * (int)outBuf[0] + 3];
				flag = false;
			}
			int current = outBuf[0];
		}
	};
	return 0;
}
DWORD WINAPI send(LPVOID lpParameter) {

	DWORD id;
	SOCKET s;
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	memset((char *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(ST_PORT1);

	// Server IP Adress

	sa.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//sa.sin_addr.S_un.S_addr = inet_addr("192.168.0.13");
	//sa.sin_addr.S_un.S_addr = inet_addr("153.19.54.94");

	CreateThread(NULL, 0, read, (LPVOID)s, 0, &id);
	SetThreadPriority(&id, 1);
	int slen = sizeof(sa);
	float message[BUFLEN];
	memset(message, '\0', BUFLEN);
	message[0] = TX;
	message[1] = TY;
	double time;
	int ticks = GetTickCount();

	//sendto(s, (char*)message, sizeof(message), 0, (struct sockaddr *) &sa, slen);
	while (1) {

		time = (GetTickCount() - ticks) * 0.001;
		ticks = GetTickCount();
		if (outBuf[(int)outBuf[0] * 5 + 2] != -10000)
			controls(time);

		if (message[0] != TX || message[1] != TY || bomb != 0) {
			message[0] = TX;
			message[1] = TY;
			if (bomb == 1)
				message[2] = 1;
			bomb = 0;
			if (sendto(s, (char*)message, sizeof(message), 0, (struct sockaddr *) &sa, slen) == SOCKET_ERROR)
			{

				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			message[2] = 0;
			//Sleep(20);
		}
	}
	closesocket(s);
	WSACleanup();
	return 0;
}