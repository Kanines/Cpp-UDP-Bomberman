#include "stdafx.h"
#include "Server.h"

DWORD WINAPI server(LPVOID lpParameter) {
	struct player players[MAX_CLIENTS];
	WSADATA wsas;
	int result;
	WORD version = MAKEWORD(2, 0);
	result = WSAStartup(version, &wsas);
	SOCKET s;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(ST_PORT1);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	float buf[BUFLEN];
	struct sockaddr_in sc;
	struct sockaddr_in tab[MAX_CLIENTS];
	int nConnections = 0;
	int lenc = sizeof(sc);
	int recv;
	int i = 0;
	players[0].x = -24;
	players[0].y = 21;
	players[1].x = -24;
	players[1].y = -20;
	players[2].x = 24;
	players[2].y = 20;
	players[3].x = 24;
	players[3].y = -20;
	int flagNew = 0;
	int current = 0;
	fd_set fds;
	int n = 1;
	bool noConnections = false;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(s, &fds);

		n = select(s, &fds, NULL, NULL, &tv);
		memset(buf, '\0', BUFLEN);
		if (n > 0 && (recv = recvfrom(s, (char*)buf, BUFLEN * sizeof(float), 0, (struct sockaddr *) &sc, &lenc)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		if (n == 0)
		{
			memset(buf, '\0', BUFLEN);
			noConnections = true;
		}
		else if (n > 0)
			noConnections = false;
		if (!noConnections)
		{
			for (i = 0; i < MAX_CLIENTS; i++)
			{
				if (sc.sin_addr.S_un.S_addr == tab[i].sin_addr.S_un.S_addr && sc.sin_port == tab[i].sin_port)
					break;
				if (i == nConnections)
				{
					tab[i] = sc;
					memset(buf, '\0', BUFLEN);
					buf[0] = 0;
					buf[1] = 1;
					buf[2] = players[i].x;
					buf[3] = players[i].y;
					players[i].lifes = 1;
					players[i].b.exist = false;
					players[i].b.explosion = false;
					players[i].b.bombTime = 0;
					sendto(s, (char*)buf, recv, 0, (struct sockaddr*) &sc, lenc);
					nConnections++;
					flagNew = 1;
					break;
				}
			}
			if (flagNew)
			{
				flagNew = 0;
				continue;
			}
			if (map.getMapXY(buf[0], buf[1]) != Wall)
			{
				if (players[i].x - buf[0] > 0.31)
					buf[0] -= 0.3;
				else if (players[i].x - buf[0] < -0.31)
					buf[0] += 0.3;
				if (players[i].y - buf[1] > 0.31)
					buf[1] -= 0.3;
				else if (players[i].y - buf[1] < -0.31)
					buf[1] += 0.3;
				players[i].x = buf[0];
				players[i].y = buf[1];

			}
			if (buf[2] == 1 && !players[i].b.exist)
			{
				players[i].b.exist = true;
				players[i].b.bombTime = GetTickCount();
				players[i].b.x = players[i].x;
				players[i].b.y = players[i].y;
			}
		}
		memset(buf, '\0', BUFLEN);
		int a = GetTickCount();
		for (int j = 0; j < nConnections; j++)
			if (players[j].b.exist && players[j].b.bombTime - a < -1000)
			{
				players[j].b.exist = false;
				//boom
				players[j].b.explosion = true;
				for (int k = 0; k < nConnections; k++)
				{

					if ((players[k].x + 8 > players[j].b.x && players[k].x - 8 < players[j].b.x
						&&players[k].y + 2 > players[j].b.y && players[k].y - 2 < players[j].b.y) ||
						(players[k].x + 2 > players[j].b.x && players[k].x - 2 < players[j].b.x
							&&players[k].y + 8 > players[j].b.y && players[k].y - 8 < players[j].b.y))
					{
						players[k].lifes = 0;
						players[k].x = -10000;
					}
				}
				if (map.getMapXY(players[j].b.x, players[j].b.y) == DWall)
					map.setMapXY(players[j].b.x, players[j].b.y, Floor);
				if (map.getMapXY(players[j].b.x + 4, players[j].b.y) == DWall)
					map.setMapXY(players[j].b.x + 4, players[j].b.y, Floor);
				if (map.getMapXY(players[j].b.x - 4, players[j].b.y) == DWall)
					map.setMapXY(players[j].b.x - 4, players[j].b.y, Floor);
				if (map.getMapXY(players[j].b.x, players[j].b.y + 4) == DWall)
					map.setMapXY(players[j].b.x, players[j].b.y + 4, Floor);
				if (map.getMapXY(players[j].b.x, players[j].b.y - 4) == DWall)
					map.setMapXY(players[j].b.x, players[j].b.y - 4, Floor);
			}

		buf[1] = nConnections;
		for (i = 0; i < nConnections; i++)
		{
			buf[2 * i + 2] = players[i].x;
			buf[2 * i + 3] = players[i].y;
			if (players[i].b.exist)
			{
				buf[2 * i + 2 + 2 * nConnections] = players[i].b.x;
				buf[2 * i + 3 + 2 * nConnections] = players[i].b.y;
			}
			else
				buf[2 * i + 2 + 2 * nConnections] = -10000;
		}
		for (i = 2; i < nConnections * 5 + 2; i += 5)
		{
			buf[i] = players[(i - 2) / 5].x;
			buf[i + 1] = players[(i - 2) / 5].y;
			if (players[(i - 2) / 5].b.exist)
			{
				buf[i + 2] = players[(i - 2) / 5].b.x;
				buf[i + 3] = players[(i - 2) / 5].b.y;
			}
			else
			{
				buf[i + 2] = -10000;
				buf[i + 3] = players[(i - 2) / 5].b.y;
			}
		}
		for (int j = 0; j < map.Cols; j++)
		{
			for (int k = 0; k < map.Rows; k++)
				if (map.getMap(j, k) == Floor)
				{
					buf[i] = j;
					i++;
					buf[i] = k;
					i++;
				}
		}
		buf[i] = 99999;
		for (i = 0; i < nConnections; i++)
		{
			buf[0] = i;
			if (sendto(s, (char*)buf, recv, 0, (struct sockaddr*) &tab[i], lenc) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			players[i].b.explosion = false;
		}
	}
	closesocket(s);
	WSACleanup();
}
