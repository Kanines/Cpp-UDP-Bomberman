#include "stdafx.h"
#include "Controls.h"

void controls(double time)
{
	double k = 20 * time;
	int g = 0;
	float size = 1;
	float size2 = size - k - 0.02;

	if (GetAsyncKeyState(VK_UP) < g){
		if ((map.getMapXY(TX - size2, TY + size) != Wall) && (map.getMapXY(TX + size2, TY + size) != Wall) && 
			(map.getMapXY(TX - size2, TY + size) != DWall) && (map.getMapXY(TX + size2, TY + size) != DWall))
			TY += k;
	}
	else if (GetAsyncKeyState(VK_DOWN) < g)
	{
		if ((map.getMapXY(TX - size2, TY - size) != Wall) && (map.getMapXY(TX + size2, TY - size) != Wall) && 
			(map.getMapXY(TX - size2, TY - size) != DWall) && (map.getMapXY(TX + size2, TY - size) != DWall))
			TY -= k;
	}
	else if (GetAsyncKeyState(VK_RIGHT) < g)
	{
		if ((map.getMapXY(TX + size, TY + size2) != Wall) && (map.getMapXY(TX + size, TY - size2) != Wall) && 
			(map.getMapXY(TX + size, TY + size2) != DWall) && (map.getMapXY(TX + size, TY - size2) != DWall))
			TX += k;
	}
	else if (GetAsyncKeyState(VK_LEFT) < g)
	{
		if ((map.getMapXY(TX - size, TY - size2) != Wall) && (map.getMapXY(TX - size, TY + size2) != Wall) &&
			(map.getMapXY(TX - size, TY - size2) != DWall) && (map.getMapXY(TX - size, TY + size2) != DWall))
			TX -= k;
	}
	if (GetAsyncKeyState(VK_SPACE) < g)
		bomb = 1;	
}