#ifndef _Level_H_
#define _Level_H_

#include "Defines.h"

using namespace std;

class Level{
	public:
		Level();
		virtual ~Level();
		gridcell map[SIZEX][SIZEY]; // Map data
		int px,py;		// Starting position, pink ghost
		int rx,ry;		// Red ghost
		int bx,by;		// Blue ghost
		int pillsleft;		// Pills on level
		vector<foodinfo> foods; // Food on level
};

#endif
