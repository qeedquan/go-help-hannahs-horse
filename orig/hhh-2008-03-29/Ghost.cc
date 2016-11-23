#include "Ghost.h"
#include "time.h"
#include "math.h"
#include <string>

class Ghost;

int Ghost::getfreedirections(){
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;
	int tot = 0;
	if(lev.map[ax][ay-1].type!='#' && lev.map[ax][ay-1].type!='-') tot++;
	if(lev.map[ax][ay+1].type!='#' && lev.map[ax][ay-1].type!='-') tot++;
	if(lev.map[ax-1][ay].type!='#' && lev.map[ax-1][ay].type!='-') tot++;
	if(lev.map[ax+1][ay].type!='#' && lev.map[ax+1][ay].type!='-') tot++;

	return tot;
};

Ghost::Ghost(char *filename, int x, int y, int speed, AnimationFactory* af) :
	Sprite(filename, af){
	this->effect = ' ';
	timeleft = 0;
	respawntime = 0;
};

void Ghost::setRespawn(long time){
	respawntime = time;
};

void Ghost::setLevel(Level lev, int startx, int starty){
	this->lev = lev;
	this->startx = startx;
	this->starty = starty;
};

void Ghost::decTimer(){
	if(timeleft > 0) timeleft--;
};

long Ghost::timer(){
	//if(timeleft>0) timeleft--;
	return timeleft;
};

void Ghost::timer(long timeleft){
	this->timeleft = timeleft;
};

bool Ghost::testdirection(int dir){

	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

	switch(dir){
		case 1: if(lev.map[ax][ay-1].type=='#' || lev.map[ax][ay-1].type=='x') return false; break;
		case 3: if(lev.map[ax][ay+1].type=='#' || lev.map[ax][ay+1].type=='-' || lev.map[ax][ay+1].type=='x') return false; break;
		case 4: if(lev.map[ax-1][ay].type=='#' || lev.map[ax-1][ay].type=='-' || lev.map[ax-1][ay].type=='x') return false; break;
		case 2: if(lev.map[ax+1][ay].type=='#' || lev.map[ax+1][ay].type=='-' || lev.map[ax+1][ay].type=='x') return false; break;
	};
	return true;
};



Ghost::~Ghost(){
};

//SDL_Surface* Ghost::frame(){
//	// Reduce powerpill timer
//	if(timeleft > 0){
//		timeleft--;
//	}
//	if(currentframe>=numframes)currentframe=0;
//	return frames[currentframe++];
//}

// x & y are pacman's current grid positions
void Ghost::move(int x, int y){

	//int pacx = (x / SIZET) - 1;
	//int pacy = (y / SIZET) - 1;

	// need to ensure it keeps going forwards unless interrupted

	if(nextxpos == xpos && nextypos == ypos){
		// actual position matches wanted position, so 
		// pacman is in square, so move
		int lft = 0;
		int rit = 0;
		int back = 0;
		lft = direction - 1;		
		if(lft == 0) lft = 4;
		rit = direction + 1;
		if(rit == 5) rit = 1;
		back = direction - 2;
		if(back < 1) back = (0 - back) + (2 * direction);

		int newdirection = 0; // test left right and set as newdirection
		// temporarily. Then test if straight on. If straight on is ok,
		// take 50-50 chance between straight on and a turn.

		//if(testdirection(direction)==false){
			// cannot go forwards, try left and right...
			if(testdirection(lft) && !testdirection(rit)){
				// left is ok, right is blocked
				newdirection = lft;
			} else if(!testdirection(lft) && testdirection(rit)){
				// right is ok, left is blocked
				newdirection = rit;
			} else if(!testdirection(lft) && !testdirection(rit)){
				// left and right are blocked
				// ...go backwards
				newdirection = back;
			} else if(testdirection(lft) && testdirection(rit)){
				// left / right both ok. pick one at random.
				int rn = rand()%2;
				// random between 0 and 1 inclusive
				if(rn==1){
					newdirection = lft;
				} else {
					newdirection = rit;
				}
			}

		if(!testdirection(direction)){
			direction = newdirection;
		} else if(testdirection(direction) && !testdirection(rit)  
					&& !testdirection(lft)){ 
			// only forward and back are available, go forwards.
			direction = direction;
		} else {
			// 50% move forwards, 50% turn
			int rn = rand()%2;
			if(rn==1){
				direction = newdirection;
			} else {
				direction = direction;
			}
		}

		int ax = (xpos / SIZET) - 1;
		int ay = (ypos / SIZET) - 1;

		switch(direction){
			case 1:	nextypos = lev.map[ax][ay-1].t; break;
			case 2: nextxpos = lev.map[ax+1][ay].l; break;
			case 3: nextypos = lev.map[ax][ay+1].t; break;
			case 4: nextxpos = lev.map[ax-1][ay].l; break;
		};

	} else if(nextxpos!=xpos){
		switch(direction){
			case 4: xpos = xpos - speed; break;
			case 2: xpos = xpos + speed; break;
		};
	} else if(nextypos!=ypos){
		switch(direction){
			case 1: ypos = ypos - speed; break;
			case 3: ypos = ypos + speed; break;
		};
	}

};

void Ghost::x(int x){
	xpos = 50 + (x*SIZET);
	nextxpos = xpos;
};

void Ghost::y(int y){
	ypos = 50 + (y*SIZET);
	nextypos = ypos;
};

int Ghost::x(){
	//xpos = (gridxpos * SIZET) + 50;
	return xpos;
};

int Ghost::y(){
	return ypos;
};
