// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include "targetver.h"

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

// TODO: reference additional headers your program requires here
#include <gl\gl.h>                             
#include <gl\glu.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
#define BUFLEN 250
#define ST_PORT1 50000
#define MAX_LOADSTRING 100
#define MAX_CLIENTS 10
enum GLDisplayListNames
{
	Background = 1000,
	Player1 = 1001,
	Bomb1 = 1002,
	Particle = 1003,
	Particle2 = 1004
};
enum MapElements
{
	Floor = 1,
	Wall = 2,
	Player = 3,
	Bomb = 4,
	DWall = 5

};
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CreateViewGLContext(HDC hDC);
BOOL SetWindowPixelFormat(HDC hDC);
DWORD WINAPI send(LPVOID lpParameter);
void CreateMaze();
void RenderScene();
void controls(double time);
DWORD WINAPI server(LPVOID lpParameter);