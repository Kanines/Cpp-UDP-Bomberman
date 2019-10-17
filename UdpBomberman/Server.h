#include "Map.h"
const float end = 123456.789;
extern Map map;
struct bomb{
	float x, y;
	int bombTime;
	bool exist;
	bool explosion;
};
struct player{
	float x, y;
	int lifes;
	struct bomb b;
};
