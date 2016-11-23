#ifndef _DEFINES_
#define _DEFINES_

#include <vector>

#define TOP 50
#define BOT 550
#define LEFT 50
#define RIGHT 550

#define SIZEX 15 // Map is 15 tiles wide, by 15 down
#define SIZEY 15
#define SIZET 30 // Tile Size (they're square)

#define SPEED 15;

using namespace std;

typedef struct{
	int sx,sy,speed;
} foodinfo;

typedef struct{
	char type;
	int l,r,t,b;
} gridcell;

typedef struct{
	char name[15];
	int score;
} hiscoreentry;

#endif
