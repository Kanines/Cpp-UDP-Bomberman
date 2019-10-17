#include "stdafx.h"
#include "SOIL.h"
class Map
{
	char map[13][15];
	int  mapList[13][15];

public:
	static const int Rows = 13;
	static const int Cols = 15;
	Map()
	{
		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Cols; j++)
			{
				if (i == 0 || j == 0 || i == Rows - 1 || j == Cols - 1)
					map[i][j] = Wall;
				else if (i % 2 == 0 && j % 2 == 0)
					map[i][j] = Wall;
				else if ((i == 1 && j == 1) || (i == 1 && j == 2) || (i == 2 && j == 1) ||
					(i == 1 && j == 13) || (i == 2 && j == 13) || (i == 1 && j == 12) ||
					(i == 11 && j == 1) || (i == 10 && j == 1) || (i == 11 && j == 2) ||
					(i == 11 && j == 13) || (i == 10 && j == 13) || (i == 11 && j == 12))
					map[i][j] = Floor;
				else
					map[i][j] = DWall;
			}
		}
	}
	char getMapXY(float x, float y)
	{
		return map[(int)(y / 4 + 6)][(int)(x / 4 + 7.5)];
	}
	void setMapXY(float x, float y, char e)
	{
		map[(int)(y / 4 + 6)][(int)(x / 4 + 7.5)] = e;
	}
	char getMap(int x, int y)
	{
		return map[y][x];
	}
	void set(int x, int y, char e)
	{
		map[y][x] = e;
	}
	void setMap()
	{
		int k = 4;

		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Cols; j++)
			{

				mapList[i][j] = i * Cols + j + 1;
				glNewList(mapList[i][j], GL_COMPILE);

				glBegin(GL_POLYGON);
				glNormal3d(0.0, 1.0, 0.0);

				glTexCoord2f(0.0f, 0.0f);
				glVertex3d(-24 + i * k, 0, -30 + j * k);

				glTexCoord2f(0.0f, 1.0f);
				glVertex3d(-24 + i * k, 0, -26 + j * k);

				glTexCoord2f(1.0f, 1.0f);
				glVertex3d(-20 + i * k, 0, -26 + j * k);

				glTexCoord2f(1.0f, 0.0f);
				glVertex3d(-20 + i * k, 0, -30 + j * k);

				glEnd();
				glEndList();
			}
		}
	}
	void draw()
	{
		GLfloat BlueSurface[] = { 0.1f, 0.0f, 0.8f, 1.0f };
		GLfloat GraySurface[] = { 49, 49, 49, 1.0f };
		GLfloat WhiteSurface[] = { 255, 255, 255, 1.0f };

		GLuint textureWall;
		GLuint textureWall2;

		textureWall = SOIL_load_OGL_texture("../Data/wall.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
			SOIL_FLAG_MIPMAPS |
			SOIL_FLAG_COMPRESS_TO_DXT);

		textureWall2 = SOIL_load_OGL_texture("../Data/wall2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO |
			SOIL_FLAG_MIPMAPS |
			SOIL_FLAG_COMPRESS_TO_DXT);

		for (int i = 0; i < Rows; i++)
		{
			for (int j = 0; j < Cols; j++)
			{

				if (map[i][j] == Wall) {
					glEnable(GL_TEXTURE_2D);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
					glBindTexture(GL_TEXTURE_2D, textureWall);

				}
				else if (map[i][j] == Floor)
				{
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
					//glBindTexture(GL_TEXTURE_2D, 0);
				}
				else
				{
					glEnable(GL_TEXTURE_2D);
					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, WhiteSurface);
					glBindTexture(GL_TEXTURE_2D, textureWall2);
				}
				glCallList(mapList[i][j]);
				glDisable(GL_TEXTURE_2D);
			}
		}

	}
};