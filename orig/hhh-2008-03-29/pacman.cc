
// Food* types. Remember that the vector will need to be purged at the start
// of each level.

#include <iostream>
#include <cstdio>
//#include <mcheck.h>
//#include <vector>
#include "Player.h"
#include "Defines.h"
#include "Level.h"
//#include "Ghost1.h" included from Food.h
#include "Ghost2.h"
#include "Ghost3.h"
#include "Food.h"
#include "Bonus.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "AnimationFactory.h"

#ifndef WIN32
#include <sys/stat.h>
#include <sys/types.h>
#endif

#define pilltime 200

SDL_Surface* screen = NULL;

// Now for some global sprites - better done as classes but Oh Well!
SDL_Surface* hedge = NULL;
SDL_Surface* hedge2 = NULL;
SDL_Surface* hedge3 = NULL;
SDL_Surface* hedge4 = NULL;
SDL_Surface* pill = NULL;
SDL_Surface* powerpill = NULL;
SDL_Surface* grass = NULL;
SDL_Surface* grass2 = NULL;
SDL_Surface* grass3 = NULL;
SDL_Surface* grass4 = NULL;
SDL_Surface* bg = NULL;
SDL_Surface* bg2 = NULL;
SDL_Surface* gate = NULL;
SDL_Surface* story = NULL;
SDL_Surface* oneway = NULL;
SDL_Surface* howto = NULL;
SDL_Surface* border = NULL;
SDL_Surface* cross = NULL;
SDL_Surface* red_door = NULL;
SDL_Surface* red_key = NULL;
SDL_Surface* gradient = NULL;
SDL_Surface* banner = NULL;
SDL_Surface* words = NULL;

SDL_Surface* current_hedge = NULL;
SDL_Surface* current_grass = NULL;

TTF_Font* font = NULL;
TTF_Font* subgamefont = NULL;
TTF_Font* bigfont = NULL;
Mix_Chunk* eatpill = NULL;
Mix_Music* bgm1 = NULL;
Mix_Chunk* eatmagic = NULL;
Mix_Chunk* eatfood = NULL;
Mix_Chunk* scream = NULL;
Mix_Chunk* chomp = NULL;
Mix_Chunk* yeah = NULL;
Mix_Chunk* uhoh = NULL;

static AnimationFactory* af = new AnimationFactory();
Player* hannah;


Ghost* red = NULL;
Ghost* pink = NULL;
Ghost* hay = NULL;
Ghost* blue = NULL;

vector<Ghost*> ghosties;

//THIS IS TEMPORARY - IT NEEDS TO BE IN A VECTOR
//Food* carrot = NULL;
//Food* apple = NULL;

int nextxpos = 50 + (SIZET*7);
int nextypos = 50 + (SIZET*7);
int ypos = nextypos;
int xpos = nextxpos;
int direction = 5; // 1, 2, 3, 4, 5 (u,d,l,r,stat)
int nextdir = 5;
int speed = 6;

unsigned int level = 1;
int lives = 3;
int score = 0;
int numlives = 3; // number of lives to start each game with

int subgamestate = 1;
int pillsleft = 0;

enum effecttype {NONE,FREEZE,INVERT,SLOW,INVIS};
effecttype effect = NONE;
int bonustimer = 0;

Uint32 lastticks;
Uint32 delta_tick;
Uint32 tick;


// Array for the map
//gridcell map[SIZEX][SIZEY];
Level lev;

vector<Food*> levelfood;

using namespace std;

//class Level{
//	gridcell map[SIZEX][SIZEY];
//};
//
typedef struct {
	int score;
	char name[40];
	
} scoreentry;

vector<Level> maps;
vector<Sprite> food;
vector<scoreentry> hiscore;
//vector<Bonus*> bonuses;
Bonus* the_bonus; // Limit to one bonus at a time

void loadHiScores(){
	#ifndef WIN32
	char *scoredir;
	scoredir = getenv("USER");

	string p(scoredir);
	p = p + "/.hannah/hiscore.dat";
	p = "/home/" + p;
	ifstream br;
	if(scoredir!=NULL){
		br.open(p.c_str());
		string hdir(scoredir);
		hdir = "/home/" + hdir;
		hdir = hdir + "/.hannah/";
		if(!br){
			mode_t file_mode; 
			file_mode = (mode_t)0755;
			mkdir(hdir.c_str(), file_mode);
			hdir = hdir + "hiscore.dat";
			ofstream os(hdir.c_str());
			cout << "Creating Hiscore table - " << hdir.c_str() << "\n";
			os << "25600 sab\n12800 zjc\n6400 hjw\n3200 sab\n1600 zjc\n800 hjw\n400 sab\n200 zjc\n100 hjw\n50 sab\n";
			os.close();
			br.open(hdir.c_str());
		} 
	} else {
		br.open(".hiscore.dat");
	}
	#else
	ifstream br("hiscore.dat");
	#endif

	// If hiscore doesn't exist, create it (above)

	char buffer[40];
	int i = 0;
	for(i=0; i<10; i++){
		br.getline(buffer,40);
		char nname[40];
		char nscore[40];
		sscanf(buffer,"%s %s",nscore,nname);

		scoreentry tmp;
		tmp.score = atoi(nscore);
		strcpy(tmp.name,nname);

		//printf("<><>%s %d\n",tmp.name,tmp.score);
		hiscore.push_back(tmp);
	}
	br.close();
};

int checkHiScores(int score){ // return the entry location of the new score
	int i =0;
	int entryAt = 0;
	for(i=0;i<10;i++){
		if(score >= ((scoreentry)hiscore[i]).score){
			entryAt = i;
			int j = 10;
			while(j > entryAt){
				hiscore[j] = hiscore[j-1];
				j--;
			}
			scoreentry news;
			news.score = score;
			strcpy(news.name,"god");
			hiscore[entryAt] = news;
			return entryAt;
		}
	}
	return -1;
};

void setTimer(int time, effecttype e){
	effect = e;
	bonustimer = time;
	if(e==INVIS){
		hannah->setAnimation("invis");
	} else if(hannah->isAnimation("invis")){
		hannah->setAnimation("default");
	}
};

void timer(){
	if(bonustimer>0){
		bonustimer--;
	} else {
		effect = NONE;
		if(hannah->isAnimation("invis"))
			hannah->setAnimation("default");
	}
};

bool checknext(){
	// get pacman's actual x y grid position
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

//	int x; int y;
			switch(direction){
				case 1: if((lev.map[ax][ay-1].type=='#' || lev.map[ax][ay-1].type=='~' || lev.map[ax][ay-1].type=='$')){
											 return true;
										} else { return false;} break;
				case 2:  if((lev.map[ax][ay+1].type=='#' || lev.map[ax][ay+1].type=='-' || lev.map[ax][ay+1].type=='~' || lev.map[ax][ay+1].type=='$')){
											 return true;
										} else { return false;} break;
				case 3:	if((lev.map[ax-1][ay].type=='#' || lev.map[ax-1][ay].type=='$')){
										return true;
									} else { return false; } break;
				case 4: if((lev.map[ax+1][ay].type=='#' || lev.map[ax+1][ay].type=='$')){
										return true;
									} else { return false; } break;
			};
	return false;
};



bool checkcollisions(){
	// get pacman's actual x y grid position
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

//	int x; int y;
			switch(nextdir){
				case 1: if((lev.map[ax][ay-1].type=='#' || lev.map[ax][ay-1].type=='~' || lev.map[ax][ay-1].type=='$')){
											 return true;
										} else { return false;} break;
				case 2:  if((lev.map[ax][ay+1].type=='#' || lev.map[ax][ay+1].type=='-' || lev.map[ax][ay+1].type=='~' || lev.map[ax][ay+1].type=='$')){
											 return true;
										} else { return false;} break;
				case 3:	if((lev.map[ax-1][ay].type=='#' || lev.map[ax-1][ay].type=='$')){
										return true;
									} else { return false; } break;
				case 4: if((lev.map[ax+1][ay].type=='#' || lev.map[ax+1][ay].type=='$')){
										return true;
									} else { return false; } break;
			};

	return false;
};

void blit(SDL_Surface* image, int x, int y){
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_BlitSurface(image, NULL, screen, &dest);
};

void createBonus(){
	// random number between 1 and 300
	//printf("Ticks: %d \n",SDL_GetTicks());
	if(the_bonus==NULL && effect==NONE){
		if(SDL_GetTicks() % 200 == 0){
		//printf("Spawning bonus...\n");
		int r = (rand() % 3) + 1;
		//if(r==1) cout << "Creating freeze\n";
		//if(r==2) cout << "Creating invis\n";
		//if(r==3) cout << "Creating invert\n";
	//	int rx = (rand() % SIZEX);
	//	int ry = (rand() % SIZEY);
	//	if(r < 50){
	//		//printf("creating bonus\n");
	//		while(lev.map[rx][ry].type!='.'){
	//			rx = (rand() % SIZEX);
	//			ry = (rand() % SIZEY);
	//			//printf("Testing (%d,%d) = %c\n",rx,ry,lev.map[rx][ry].type);
	//		}
			
			//Bonus* b;
			switch(r){
			case 1: the_bonus = new Bonus("freeze",7,7,1,af,'f'); break;
			case 2: the_bonus = new Bonus("invis",7,7,1,af,'i'); break;
			case 3: the_bonus = new Bonus("invert",7,7,1,af,'s'); break;
			};
			the_bonus->setLevel(lev,7,7);
			//bonuses.push_back(b);
	//	}
		}
	}
};

void moveBonus(){
	if(the_bonus!=NULL){
		//for(i=0; i<bonuses.size(); i++){
			the_bonus->move(xpos,ypos);
			blit(the_bonus->frame(), the_bonus->x(), the_bonus->y());
			the_bonus->decLife();
			if(the_bonus->lifeforce == 0){
				//vector<Bonus*>::iterator itRemove = bonuses.begin() + i;
				//bonuses.erase(itRemove);
				delete the_bonus;
			//	cout << the_bonus << "\n";
				the_bonus = NULL;
			//	cout << the_bonus << "\n";
			}
		//}
	}
};

void check_bonus_pacman(){
	if(the_bonus!=NULL){
			if((the_bonus->x() < xpos+30) && (the_bonus->x()+30 > xpos) && (the_bonus->y()+30 > ypos) && (the_bonus->y() < ypos+30)){
				// Pacman gets pill
				//vector<Bonus*>::iterator itRemove = bonuses.begin() + i;
				//bonuses.erase(itRemove);
				switch(the_bonus->type){
					case 'f':	setTimer(200,FREEZE); Mix_PlayChannel(-1,yeah,0); break;
					case 's':	setTimer(200,INVERT); Mix_PlayChannel(-1,uhoh,0); break;
					case 'i':	setTimer(200,INVIS); Mix_PlayChannel(-1,yeah,0); break;
				};
				delete the_bonus;
				the_bonus = NULL;
			}
	}
};
/*
void freeBonus(){
	int i;
	for(i=0; i<bonuses.size(); i++){
		Bonus* tmp = (Bonus*)bonuses[i];
		delete tmp;
	}
	bonuses.clear();
};
*/

void drawMap(){
	int x; int y;
	x = 0;
	y = 0;
	int mapx = 0;
	int mapy = 0;
	for(mapy = 0; mapy < SIZEY; mapy++){
	for(mapx = 0; mapx < SIZEX; mapx++){
		switch(lev.map[mapx][mapy].type){
			case '#': blit(current_hedge,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case '.': blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50); 
				  blit(pill, (mapx*SIZET)+50,(mapy*SIZET)+50);
				break;
			case '_': blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case 'p': blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50);
					blit(powerpill,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case '-':	blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50);
								blit(gate,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case '~':	blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50); 
								blit(oneway,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case 'x':	blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50);
								blit(cross,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case '$':	blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50);
								blit(red_door,(mapx*SIZET)+50,(mapy*SIZET)+50); break;
			case 'r':	blit(current_grass,(mapx*SIZET)+50,(mapy*SIZET)+50);
								blit(red_key,(mapx*SIZET)+50,(mapy*SIZET)+50); break;


		};
	}
	}
	blit(hannah->frame(),xpos,ypos);
};

void mapout(){
};

void level_init_food(Level levl){
	// While there are more foods in lev.foods create add to vector of food
	//printf("Init food...\n");
	lev.foods.clear();
	unsigned int i = 0;
	//printf("Level's food buffer is %d\n",levl.foods.size());
	for(i=0; i<levl.foods.size(); i++){
		foodinfo tmpfi = (foodinfo)levl.foods[i];
		//printf("Size levfood: %d \n",levl.foods.size());
		Food *tmpfood = new Food("carrot",tmpfi.sx,tmpfi.sy,tmpfi.speed,af);
		tmpfood->setLevel(levl, tmpfi.sx, tmpfi.sy);
		//printf("Creating new food at: %d %d %d \n",tmpfi.sx,tmpfi.sy,tmpfi.speed);
		levelfood.push_back(tmpfood);
		pillsleft++;
	}
};

void level_move_food(){
	unsigned	int i = 0;
	for(i=0; i<levelfood.size(); i++){
		Food *tmpfood = (Food*)levelfood[i];
		tmpfood->move(xpos,ypos);
	}
};

void level_free_food(){
	//printf("Free food...\n");
	unsigned int i = 0;

	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		delete tmp;
	}
	levelfood.clear();
	//printf("Bool lev empty: %d\n",levelfood.empty());
};

void level_draw_food(){
	unsigned int i = 0;
	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		if(tmp->alive == true){
			blit(tmp->frame(),tmp->x(),tmp->y());
		}
	}
};



Ghost* check_baddies_pacman(){
	// Check pacman for collision with ghosts
	// Is it possible to enumerate through ghosts?
	// Encapsulate ghosts in vector to make iteration easier
	// Return true if collision

	//printf("Check baddies pacman\n");
	if(effect!=INVIS){
		Ghost* tmp = NULL;	
		unsigned int i=0;
		for(i=0; i<ghosties.size(); i++){
			tmp = (Ghost*)ghosties[i];

			if((tmp->x() < xpos+30) && (tmp->x()+30 > xpos) && (tmp->y()+30 > ypos) && (tmp->y() < ypos+30)){
				//printf("You hit a ghost!\n");
	
				return tmp;
			} 
		}
	}
	return NULL;
};

void drawbaddies(){
	blit(red->frame(),red->x(),red->y());
	blit(pink->frame(),pink->x(),pink->y());
	//blit(apple->frame(),apple->x(),apple->y());
	blit(blue->frame(),blue->x(),blue->y());
	//blit(carrot->frame(),carrot->x(),carrot->y());
	level_draw_food();
};

void readMap(string filename){
	Level levl;	
	string fname(filename);
	string dirname(DATA_INSTALL_DIR);
	
	dirname = dirname + "/" + fname;
	levl.pillsleft = 0;
	ifstream br(dirname.c_str());
	char buffer[SIZEX+1];
	// open file, read a line
	if(!br.is_open()){
		printf("Could not open map file %s!\n",dirname.c_str());
	} else {	
		int x = 0;
		int y = 0;

		// map needs to be read BEFORE passing to ghosts, or they
		// just get NULL and therefore have no way to hit a wall.
	
		for(y=0; y<SIZEY; y++){
			br.getline(buffer,SIZEX+1);
			for(x=0; x<SIZEX; x++){
				levl.map[x][y].type = buffer[x];
				if(levl.map[x][y].type == '.') levl.pillsleft++;
				levl.map[x][y].t = (y*SIZET) + 50;
				levl.map[x][y].b = (y*SIZET) + 50 + 49;
				levl.map[x][y].l = (x*SIZET) + 50;
				levl.map[x][y].r = (x*SIZET) + 50 + 49;
			}
		}

		char tempx[5]; char tempy[5]; char spd[5];
		int tempxi; int tempyi; // bit of geordi there :D
		int tempspdi;
	
		// Kill old ghosts and free mem
		// before creating new ones
			
		br.getline(buffer, SIZEX+1); // SIZEX+1 just to use all allocated space
		//printf("Buffer: %s\n", buffer);
		string pinkline(buffer);
		sscanf(pinkline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		//printf("X: %d Y: %d\n",tempxi,tempyi);
		levl.px = tempxi;
		levl.py = tempyi;
		//pink = new Ghost2("pink",lev, tempxi, tempyi);
		//printf("Pink created at %d %d\n", tempxi, tempyi);
		//printf("Actual -> %d %d", pink->x(),pink->y());

		br.getline(buffer, SIZEX+1);
		//printf("Buffer: %s\n", buffer);
		string blueline(buffer);
		sscanf(blueline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		//blue = new Ghost3("blue",lev, tempxi, tempyi);
		
		//printf("X: %d Y: %d\n",tempxi,tempyi);
		levl.bx = tempxi;
		levl.by = tempyi;
	
		br.getline(buffer, SIZEX+1);
		//printf("Buffer: %s\n", buffer);
		string redline(buffer);
		sscanf(redline.c_str(),"%*s %s %s",tempx,tempy);
		tempxi = atoi(tempx);
		tempyi = atoi(tempy);
		//red = new Ghost1("red", lev, tempxi, tempyi);
		
		//printf("X: %d Y: %d\n",tempxi,tempyi);
		levl.rx = tempxi;
		levl.ry = tempyi;

		// Read file til end. Each line is a food item
		//br.getline(buffer, SIZEX+1);
		//printf("Food Buffer: %s\n",buffer);
		while (br.getline(buffer, SIZEX+1)){
			//printf("Food Buffer: %s\n",buffer);
			string foodline(buffer);
			sscanf(foodline.c_str(),"%*s %s %s %s",tempx,tempy,spd);
			tempxi = atoi(tempx);
			tempyi = atoi(tempy);
			tempspdi = atoi(spd);
			foodinfo fd;
			fd.sx = tempxi;
			fd.sy = tempyi;
			//printf("fd.sx %d fd.sy %d\n",fd.sx,fd.sy);
			fd.speed = tempspdi;
			levl.foods.push_back(fd);
			//printf("Size foods %d\n",levl.foods.size());
			//br.getline(buffer, SIZEX+1);
		}
		
		// kill any old ghosts before creating new ones
		br.close();
		}
		maps.push_back(levl);
		//printf("Loaded level\n");
};

SDL_Surface* init(bool fullscreen){
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	if(fullscreen == true){
	//printf("Init screen\n");
	screen = SDL_SetVideoMode(800, 600, 24, 
		SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF|SDL_FULLSCREEN);
	} else {
	screen = SDL_SetVideoMode(800, 600, 24, 
		SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF);

	}
	if( !screen ) {
		fprintf(stderr, 
			"Couldn't create a surface: %s\n", SDL_GetError());
		return NULL;

	} else {
		if(TTF_Init()==-1) {
			printf("TTF_Init: %s\n", TTF_GetError());
    	return NULL;
		}

		if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
			return NULL;
		}

		SDL_WM_SetCaption("Help Hannah's Horse!!", "Help Hannah's Horse!!");

		return screen;
	}
};



void move(){
		
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

	if(!checkcollisions() && xpos==nextxpos && ypos==nextypos){
		direction = nextdir;
			switch(nextdir){
					case 1: nextypos = lev.map[ax][ay-1].t; direction=nextdir; break;
					case 2: nextypos = lev.map[ax][ay+1].t; direction=nextdir; break;
					case 3: nextxpos = lev.map[ax-1][ay].l; direction=nextdir; break;
					case 4: nextxpos = lev.map[ax+1][ay].l; direction=nextdir; break;
			};
	} else if(checkcollisions() && xpos==nextxpos && ypos==nextypos){
		if(nextdir==direction){
			direction = 5;
			nextdir = 5;
		} else if(nextdir!=direction && checknext()){
			direction = 5;
			nextdir=5;
		} else {
			switch(direction){
				case 1: nextypos = lev.map[ax][ay-1].t; break;
				case 2: nextypos = lev.map[ax][ay+1].t; break;
				case 3: nextxpos = lev.map[ax-1][ay].l; break;
				case 4: nextxpos = lev.map[ax+1][ay].l; break;
      };
			direction = direction;
		}
	}

	if(nextxpos!=xpos){
		switch(direction){
			case 3: xpos = xpos - speed; break;
			case 4: xpos = xpos + speed; break;
		};
	} else if(nextypos!=ypos){
		switch(direction){
			case 1: ypos = ypos - speed; break;
			case 2: ypos = ypos + speed; break;
		};
	}		

};

void drawBigTextAt(char* text, int x, int y){
	SDL_Surface* textsurface;
	SDL_Color color = {0,0,0};
	char tt[40];
	sprintf(tt, text);
	textsurface = TTF_RenderText_Blended(bigfont,tt,color);
	blit(textsurface,x,y);
	SDL_FreeSurface(textsurface);	
};

void drawTextAt(char* text, int x, int y){
	SDL_Surface* textsurface;
	SDL_Color color = {0,0,0};
	char tt[40];
	sprintf(tt, text);
	textsurface = TTF_RenderText_Blended(subgamefont,tt,color);
	blit(textsurface,x,y);
	SDL_FreeSurface(textsurface);	
};

void saveHiScores(){
	//printf("Saving Hi Score\n");
	//remove(SCORE_DIR "/hiscore.dat");
	//ofstream br(SCORE_DIR "/hiscore.dat");
	ofstream br;
	#ifdef WIN32
		remove("hiscore.dat");
		br.open("hiscore.dat");
	#else
		char* scoredir;
		scoredir = getenv("USER");
		string s(scoredir);
		s = "/home/" + s;
		s = s + "/.hannah/hiscore.dat";
		remove(s.c_str());
		cout << "Saving score " << s.c_str() << "\n";
		br.open(s.c_str());
	#endif
	
	int i;
	for(i=0; i<10; i++){
		//printf("Saving entry %d\n",i);
		char buf[40];
		scoreentry tmp;
		tmp = (scoreentry)hiscore[i];
		sprintf(buf,"%d %s",tmp.score,tmp.name);
		//printf("%s\n",buf);
		br << buf << "\n";
	}
	br.flush();
	br.close();
	//printf("Finished saving scores\n");
};

void setHiScore(int entryAt){

	// One array for each initial
	char first[][26] = {{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'},
	{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'},
	{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'}};
	int counter = 0;
	int c1 = 0;
	int c2 = 0;
	int c3 = 0;
	int current = 0;
	bool done = false;
	char tt[9] = "aaa     ";
	int indx = 250;
	int indy = 350;

	SDL_Flip(screen);
	SDL_Event e;
	while(!done){
		while(SDL_PollEvent (&e)){
			switch(e.key.state){
				case SDL_PRESSED:
					switch(e.key.keysym.sym){
						case SDLK_RETURN: done = true; strcpy(hiscore[entryAt].name,tt); break;
						case SDLK_RIGHT:
							// Make the current initial equal to where the counter had gotten
							//switch(current){
							//	case 0: c1 = counter; break;
							//	case 1: c2 = counter; break;
							//	case 2: c3 = counter; break;
						//	};
							current++;
							if(current == 3) current = 0;
							// Make the new initial equal to where the counter left off last time
							switch(current){
								case 0: counter = c1; break;
								case 1: counter = c2; break;
								case 2: counter = c3; break;
							};
						break;
						case SDLK_LEFT:
					//		switch(current){
				//				case 0: c1 = counter; break;
			//					case 1: c2 = counter; break;
		//						case 2: c3 = counter; break;
	//						};
							current--;
							if(current == -1) current = 2;
							switch(current){
								case 0: counter = c1; break;
								case 1: counter = c2; break;
								case 2: counter = c3; break;
							};
						break;
						case SDLK_UP:
							counter++;
							if(counter>25) counter = 0;
							switch(current){
								case 0: c1 = counter; break;
								case 1: c2 = counter; break;
								case 2: c3 = counter; break;
							};
							//counter++;
							
							//printf("[%c][%c][%c]\n",first[0][c1],first[1][c2],first[2][c3]);
						break;
						case SDLK_DOWN:
							counter--;
							if(counter<0) counter = 25;
							switch(current){
								case 0: c1 = counter; break;
								case 1: c2 = counter; break;
								case 2: c3 = counter; break;
							};
							//printf("[%c]\n",first[current][c1]);
						break;
						default: break;
					};
					switch(current){
						case 0: indx = 250; break;
						case 1: indx = 325; break;
						case 2: indx = 400; break;
					};
					sprintf(tt,"%c%c%c",first[0][c1],first[1][c2],first[2][c3]);
		//			printf("[%c][%c][%c]\n",first[0][c1],first[1][c2],first[2][c3]);
					
				break;
			};
			
		}

		blit(border,190,100);
		drawTextAt("Enter your initials",210,150);
		drawBigTextAt(tt,250,250);
		blit(powerpill,indx,indy);
		SDL_Flip(screen);
	}
};

/*
void getHiScore(){
	SDL_Flip(screen);
	bool done = false;
	SDL_Event e;
	char name[40];
	int i = 0;
	for(i=0; i<40; i++){ name[i]=' ';}
	i = 0;
	while(!done){
		while(SDL_PollEvent(&e)){
			switch(e.key.state){
				case SDL_PRESSED:
					if(e.key.keysym.sym == SDLK_RETURN){ 
						done = true;
					} else if(e.key.keysym.sym == SDLK_SPACE){
						name[i] = ' ';
						i++;
						drawTextAt(name, 200,250);
						SDL_Flip(screen);
					} else{
						char *ch;
						ch = SDL_GetKeyName(e.key.keysym.sym);
						printf(">%s<\n",ch);
						name[i] = ch[0];
						i++;
						drawTextAt(name, 200,250);
						SDL_Flip(screen);
					}
				break;
			};		
		}		
	}
};*/

void drawHiScore(){
	// Read hi score in from file
	//ifstream br("hiscore.dat");
	int i = 0;
	int yy = 120;
//	char buffer[40];
	
	for(i=0; i<10; i++){
	//	br.getline(buffer,40);
	//	drawTextAt(buffer,200,yy);
	//	yy = yy + 35;
		scoreentry tmp = (scoreentry)hiscore[i];
		char buf[30];
		sprintf(buf,"%d",tmp.score);
		drawTextAt(buf,200,yy);
		drawTextAt(tmp.name,400,yy);
		yy = yy + 35;
	}

	drawTextAt("Press SPACE",430,480);
};

void drawCredits(){
	drawTextAt("Help Hannah's Horse",200,120);
	drawTextAt("(c)2006 Steve Branley",200,170);
	drawTextAt("Graphics - Steve Branley",200,220);
	drawTextAt("Programming - Steve Branley",200,270);
	drawTextAt("Inspiration - Hannah Williams",200,320);
	drawTextAt("Encouragement - Zoe Cunningham",200,370);
	drawTextAt("This game is released under",200,420);
	drawTextAt("the terms of the GPL v2",200,470);
};

void drawsubgamestate(int subgamestate){
	// 1 = "Level 1 - Get Ready!", 2 = "Get Ready!", 3 = "Game Over!"
	// Display "Level 1" / "Game Over!" / "Get Ready!"
	SDL_Surface* textsurface;
	SDL_Color color = {0,0,0};
	char tt[100];
	switch(subgamestate){
		case 1: sprintf(tt, "Level %d - Get Ready!", level); break;
		case 2: sprintf(tt, "Get Ready!"); break;
		case 3: sprintf(tt, "Game Over!"); break;
	};
	//SDL_FreeSurface(textsurface);
	textsurface = TTF_RenderText_Blended(subgamefont,tt,color);
	switch(subgamestate){
		case 1:	blit(textsurface,170,232); break;
		case 2:	blit(textsurface,220,232); break;
		case 3:	blit(textsurface,220,232); break;
	};
	SDL_FreeSurface(textsurface);
	
};

void initHannah(int x, int y){
	nextxpos = 50 + (SIZET*x);
	nextypos = 50 + (SIZET*y);
	ypos = nextypos;
	xpos = nextxpos;
	direction = 5; // 1, 2, 3, 4, 5 (u,d,l,r,stat)
	nextdir = 5;
	//speed = 6;
};

void drawscore(){
	SDL_Surface* textsurface;
	SDL_Color color = {0,0,0};
	char tt[40];
	sprintf(tt, "%d", score);
	textsurface = TTF_RenderText_Blended(font,tt,color);
	blit(textsurface,610,260);
	SDL_FreeSurface(textsurface);

	sprintf(tt, "Level %d", level);
	textsurface = TTF_RenderText_Blended(font,tt,color);
	blit(textsurface,610,350);
	SDL_FreeSurface(textsurface);

	sprintf(tt, "Lives %d", lives);
	textsurface = TTF_RenderText_Blended(font,tt,color);
	blit(textsurface,610,440);

	SDL_FreeSurface(textsurface);
};

void initLevel(int levelnum){
	if(levelnum < 4){
		current_grass = grass;
		current_hedge = hedge;
	} else if(levelnum < 7){
		current_grass = grass2;
		current_hedge = hedge2;
	} else if(levelnum < 10){
		current_grass = grass3;
		current_hedge = hedge3;
	} else if(levelnum < 13){
		current_grass = grass4;
		current_hedge = hedge4;
	} else {
		current_grass = grass;
		current_hedge = hedge;
	}
	//printf("init level\n");
	// Remove any existing ghosts 
	// before creating new ones for new level
	ghosties.clear();
	if(the_bonus!=NULL){
		delete the_bonus;
		the_bonus = NULL;
	}
	
	subgamestate = 1;
	setTimer(0,NONE);
//	if(red != NULL) delete(red);
//	if(blue !=NULL) delete(blue);
//	if(pink !=NULL) delete(pink);
	//if(carrot !=NULL) free(carrot);
	lev = (Level)maps.at(levelnum-1);
	// Number of food is correct here.
	//printf("Just before INIT; Level food size: %d \n",lev.foods.size());
//	red = new Ghost1("red", lev, lev.rx, lev.ry, 5);
	red->setLevel(lev, lev.rx, lev.ry);
	red->x(lev.rx);
	red->y(lev.ry);
	red->setAnimation("default");
	red->timer(0);
	//red->x(lev.rx); red->y(lev.ry);
//	pink = new Ghost2("pink", lev, lev.px, lev.py, 5);
	pink->setLevel(lev, lev.px, lev.py);
	pink->x(lev.px);
	pink->y(lev.py);
	pink->setAnimation("default");
	pink->timer(0);
//	blue = new Ghost3("blue", lev, lev.bx, lev.by, 5);
	blue->setLevel(lev, lev.bx, lev.by);
	blue->x(lev.bx);
	blue->y(lev.by);
	blue->setAnimation("default");
	blue->timer(0);

	//if(levelfood.size()!=0){
	//	level_free_food();
	//}

	//printf("rx %d px %d bx %d\n",lev.rx,lev.px,lev.bx);
	pillsleft = lev.pillsleft;
	level_init_food(lev);

	ghosties.push_back(red);
	ghosties.push_back(pink);
	ghosties.push_back(blue);

	initHannah(7,7);

	//blit(bg2,550,50);
	//drawMap();
	//drawscore();
	//drawsubgamestate(subgamestate);
	//SDL_Flip(screen);

	//SDL_Delay(2000);
};

bool check_food_pacman(){
	// Check pacman for collision with ghosts
	// Is it possible to enumerate through ghosts?
	// Encapsulate ghosts in vector to make iteration easier
	// Return true if collision
	
	unsigned int i=0;
	for(i=0; i<levelfood.size(); i++){
		Food* tmp = (Food*)levelfood[i];
		if((tmp->x() < xpos+30) && (tmp->x()+30 > xpos) && (tmp->y()+30 > ypos) && (tmp->y() < ypos+30) && tmp->alive==true){
			//printf("You hit a carrot!\n");
			Mix_PlayChannel(-1,eatfood,0);
			tmp->alive = false;
			score = score + 50;
			pillsleft--;
			if(pillsleft==0){
				// set subgamestate to "Well Done!"
				// set level to level +1
				level++;
				subgamestate = 1;
				// Free the foods from the last level
				// Only needed once for checkpillscollision and check_foods_pacman
				//level_free_food();
				initLevel(level);
				initHannah(7,7);
			}
			return true;
		}
	}
	return false;
};

void reInitLevel(){
	// put ghosts and pacman back to start
	// after dying
	red->x(lev.rx);
	red->y(lev.ry);
	red->setRespawn(0);
	blue->x(lev.bx);
	blue->y(lev.by);
	blue->setRespawn(0);
	pink->x(lev.px);
	pink->y(lev.py);
	pink->setRespawn(0);

	red->setAnimation("default");
	pink->setAnimation("default");
	blue->setAnimation("default");
	red->timer(0);
	blue->timer(0);
	pink->timer(0);

	// Write "Get Ready!"	
	blit(bg2,550,0);
	// Put Hannah back in the middle
	initHannah(7,7);

	setTimer(0,NONE);

	drawMap();
	move();
	red->move(xpos,ypos);
	blue->move(xpos,ypos);
	pink->move(xpos,ypos);
	drawbaddies();
		
	drawsubgamestate(2);
	hannah->respawn();

	blit(hannah->frame(),xpos,ypos);
	if(the_bonus!=NULL){
		delete the_bonus;
		the_bonus = NULL;
	}

	SDL_Flip(screen);
	//SDL_Delay(2000);
};

void openRedDoor(){
	int ax = 0;
	int ay = 0;
	for(ax=0; ax<SIZEX; ax++){
		for(ay=0; ay<SIZEY; ay++){
			if(lev.map[ax][ay].type=='$')
				lev.map[ax][ay].type='_';
		}
	}
};


void endGame(){
	int bx = 150;
	int by = -500;
	int wx = 150;
	int wy = 650;
	bool bend = false;
	bool wend = false;
	bool end = false;
	SDL_Event e;
	// While banner and text are not in the right places
	while(!bend || !wend){
		if(by<100){
			by = by + 5;
		} else {
			bend = true;
		}
		if(wy>100){
			wy = wy - 5;
		} else {
			wend = true;
		}
		blit(gradient,0,0);
		blit(banner,bx,by);
		blit(words,wx,wy);
		SDL_Flip(screen);
	}
	while(!end){
		while(SDL_PollEvent(&e)){
			if(e.key.state == SDL_PRESSED) end = true;	
		}
	}
	subgamestate = 3;
};


void checkpillcollision(){
	//printf("Checking pill collision\n");
	int ax = (xpos / SIZET) - 1;
	int ay = (ypos / SIZET) - 1;

	if(lev.map[ax][ay].type=='.'){
		lev.map[ax][ay].type='_';
		score = score + 10;
		Mix_PlayChannel(-1,eatpill,0);
		pillsleft--;
		// check for end of level
		if(pillsleft==0){
			// set subgamestate to "Well Done!"
			// set level to level +1
			level++;
			level_free_food();
			if(level>maps.size()){
				// Game is over, get highscore
				endGame();
			} else {

				subgamestate = 1;
				// Free the foods from the last level
				//level_free_food();
				//freeBonus();
				initLevel(level);
				initHannah(7,7);
			}
		}
	} else if(lev.map[ax][ay].type=='p'){
		lev.map[ax][ay].type='_';
		score = score + 100;
		Mix_PlayChannel(-1,eatmagic,0);
		red->timer(pilltime);
		blue->timer(pilltime);
		pink->timer(pilltime);
	//	red->effect = 'f';
	//	blue->effect = 'f';
	//	pink->effect = 'f';
		red->setAnimation("flash");
		blue->setAnimation("flash");
		pink->setAnimation("flash");
	} else if(lev.map[ax][ay].type=='r'){
		lev.map[ax][ay].type='_';
		openRedDoor();
	}

};

int loadSprites(){
	#ifdef WIN32
	hedge = IMG_Load("images\\hedge.png");
	hedge2 = IMG_Load("images\\hedge2.png");
	hedge3 = IMG_Load("images\\hedge3.png");
	hedge4 = IMG_Load("images\\hedge4.png");
	
	pill = IMG_Load("images\\pill.png");
	powerpill = IMG_Load("images\\powerpill.png");
	grass = IMG_Load("images\\grass.png");
	grass2 = IMG_Load("images\\grass2.png");
	grass3 = IMG_Load("images\\grass3.png");
	grass4 = IMG_Load("images\\grass4.png");

	bg = IMG_Load("images\\bg_left.png");
	bg2 = IMG_Load("images\\bg_right2_1.png");
	story = IMG_Load("images\\story.png");
	gate = IMG_Load("images\\gate.png");
	oneway = IMG_Load("images\\oneway.png");
	howto = IMG_Load("images\\howtoplay.png");
	border = IMG_Load("images\\border.png");
	cross = IMG_Load("images\\cross.png");
	red_door = IMG_Load("images\\red_door.png");
	red_key = IMG_Load("images\\red_key.png");

	#else
	hedge = IMG_Load(DATA_INSTALL_DIR "/images/hedge.png");
	if(!hedge) return 0;
	hedge = SDL_DisplayFormat(hedge);

	hedge2 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/hedge2.png"));
	hedge3 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/hedge3.png"));
	hedge4 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/hedge4.png"));
	
	pill = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/pill.png"));
	powerpill = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/powerpill.png"));
	grass = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/grass.png"));
	grass2 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/grass2.png"));
	grass3 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/grass3.png"));
	grass4 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/grass4.png"));

	bg = IMG_Load(DATA_INSTALL_DIR "/images/bg_left.png");
	bg2 = SDL_DisplayFormat(IMG_Load(DATA_INSTALL_DIR "/images/bg_right2_1.png"));
	story = IMG_Load(DATA_INSTALL_DIR "/images/story.png");
	gate = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/gate.png"));
	oneway = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/oneway.png"));
	howto = IMG_Load(DATA_INSTALL_DIR "/images/howtoplay.png");
	border = IMG_Load(DATA_INSTALL_DIR "/images/border.png");
	cross = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/cross.png"));
	red_door = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/red_door.png"));
	red_key = SDL_DisplayFormatAlpha(IMG_Load(DATA_INSTALL_DIR "/images/red_key.png"));
	gradient = IMG_Load(DATA_INSTALL_DIR "/images/gradient.png");
	banner = IMG_Load(DATA_INSTALL_DIR "/images/end_banner.png");
	words = IMG_Load(DATA_INSTALL_DIR "/images/congrats.png");

	//printf("%s /images/red_key.png\n",DATA_INSTALL_DIR);
	#endif

	if(grass!=NULL && pill!=NULL 
	&& powerpill!=NULL && hedge!=NULL 
	&& bg!=NULL){
		SDL_SetColorKey(pill, SDL_SRCCOLORKEY,
			SDL_MapRGB(pill->format, 0x00, 0xFF, 0xFF));
		
		SDL_SetColorKey(story, SDL_SRCCOLORKEY,
			SDL_MapRGB(story->format,0x00,0xFF,0xFF));

		SDL_SetColorKey(powerpill, SDL_SRCCOLORKEY,
			SDL_MapRGB(powerpill->format,0x00,0xFF,0xFF));

		SDL_SetColorKey(gate, SDL_SRCCOLORKEY,
			SDL_MapRGB(gate->format,0x00,0xFF,0xFF));


		return 1;
	} else {
		return 0;
	}
	
};

void loadsounds(){
	#ifdef WIN32
	eatpill = Mix_LoadWAV("sounds\\DripC.wav");
	eatmagic = Mix_LoadWAV("sound\\/gulp.wav");
	eatfood = Mix_LoadWAV("sounds\\crunch.wav");
	scream = Mix_LoadWAV("sounds\\scream.wav");
	chomp = Mix_LoadWAV("sounds\\chomp.wav");
	uhoh = Mix_LoadWAV("sounds\\uh-oh.wav"); 
	yeah = Mix_LoadWAV("sounds\\yeah.wav");

	#else
	eatpill = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/DripC.wav");
	eatmagic = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/gulp.wav");
	eatfood = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/crunch.wav");
	scream = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/scream.wav");
	chomp = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/chomp.wav");
	uhoh = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/uh-oh.wav"); 
	yeah = Mix_LoadWAV(DATA_INSTALL_DIR "/sounds/yeah.wav");
	#endif
	
	printf("Loaded sounds\n");
};

void cleanUp(){

	printf("Cleaning up...\n");

	SDL_FreeSurface(hedge);
	SDL_FreeSurface(hedge2);
	SDL_FreeSurface(hedge3);
	SDL_FreeSurface(hedge4);
	SDL_FreeSurface(pill);
	SDL_FreeSurface(powerpill);
	SDL_FreeSurface(grass);
	SDL_FreeSurface(grass2);
	SDL_FreeSurface(grass3);
	SDL_FreeSurface(grass4);
	SDL_FreeSurface(bg);
	SDL_FreeSurface(bg2);
	SDL_FreeSurface(gate);
	SDL_FreeSurface(oneway);
	SDL_FreeSurface(howto);
	SDL_FreeSurface(story);
	SDL_FreeSurface(border);
	SDL_FreeSurface(cross);
	SDL_FreeSurface(red_key);
	SDL_FreeSurface(red_door);
	SDL_FreeSurface(gradient);
	SDL_FreeSurface(banner);
	SDL_FreeSurface(words);

	TTF_CloseFont(font);
	TTF_CloseFont(subgamefont);
	TTF_CloseFont(bigfont);
	
	Mix_HaltChannel(-1);
	Mix_FreeChunk(eatpill);
	Mix_FreeMusic(bgm1);
	Mix_FreeChunk(eatmagic);
	Mix_FreeChunk(eatfood);
	Mix_FreeChunk(scream);
	Mix_FreeChunk(chomp);
	Mix_FreeChunk(yeah);
	Mix_FreeChunk(uhoh);

	if(the_bonus!=NULL){ delete the_bonus; the_bonus = NULL;}
	
	delete(hannah);
	delete(red);
	delete(pink);
	delete(blue);
	//level_free_food();
	//
	delete(af);

	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
};

void initGame(){

	font=TTF_OpenFont(DATA_INSTALL_DIR "/font.ttf", 36);
	bigfont=TTF_OpenFont(DATA_INSTALL_DIR "/subgamefont.ttf",108);
	bgm1 = Mix_LoadMUS(DATA_INSTALL_DIR "/bgm1.ogg");

	if(!bgm1) printf("Mix_LoadMUS(\"bgm1.mp3\"): %s\n", Mix_GetError());

	subgamefont=TTF_OpenFont(DATA_INSTALL_DIR "/subgamefont.ttf", 24);
	if(!font || !subgamefont) printf("Unable to load font! %s \n", TTF_GetError());

	#ifdef WIN32
	readMap("maps\\map1.dat");
	readMap("maps\\map2.dat");
	readMap("maps\\map3.dat");
	readMap("maps\\map4.dat");
	readMap("maps\\map5.dat");
	readMap("maps\\map6.dat");
	readMap("maps\\map7.dat");
	readMap("maps\\map8.dat");
	readMap("maps\\map9.dat");
	readMap("maps\\map10.dat");
	readMap("maps\\map11.dat");
	readMap("maps\\map12.dat");
	readMap("maps\\map13.dat");
	readMap("maps\\map14.dat");
	readMap("maps\\map15.dat");

	#else
	readMap("/maps/map1.dat");
	readMap("/maps/map2.dat");
	readMap("/maps/map3.dat");
	readMap("/maps/map4.dat");
	readMap("/maps/map5.dat");
	readMap("/maps/map6.dat");
	readMap("/maps/map7.dat");
	readMap("/maps/map8.dat");
	readMap("/maps/map9.dat");
	readMap("/maps/map10.dat");
	readMap("/maps/map11.dat");
	readMap("/maps/map12.dat");
	readMap("/maps/map13.dat");
	readMap("/maps/map14.dat");
	readMap("/maps/map15.dat");

	printf("Loaded maps\n");
	
	#endif

	// Load extra anims for sprites
	//hannah->loadAnimation("die", false);
	af->loadAnimation("hannah","die",false,"hannah-die");
	af->loadAnimation("hannah","default",true,"hannah-default");
	af->loadAnimation("hannah","invis",true,"hannah-invis");


	af->loadAnimation("red","default",true,"red-default");
	af->loadAnimation("pink","default",true,"pink-default");
	af->loadAnimation("blue","default",true,"blue-default");

	//red->loadAnimation("flash",true);
	//pink->loadAnimation("flash",true);
	//blue->loadAnimation("flash",true);
	af->loadAnimation("red","flash",true,"red-flash");
	af->loadAnimation("pink","flash",true,"pink-flash");
	af->loadAnimation("blue","flash",true,"blue-flash");

	af->loadAnimation("carrot","default",true,"carrot-default");

	af->loadAnimation("freeze","default",true,"freeze-default");
	af->loadAnimation("invis","default",true,"invis-default");
	af->loadAnimation("invert","default",true,"invert-default");

	red = new Ghost1("red",0,0,5,af);
	pink = new Ghost2("pink",0,0,5,af);
	blue = new Ghost3("blue",0,0,5,af);

	hannah = new Player("hannah",af);

	//af->list();
};



int main(int argc, char **argv){

	//printf("Started main method\n");

	//Level lev;

	//mtrace();
	level = 1;

	//printf("%d arguments passed\n",argc);
	bool fullscreen = false;
	bool music = true;

	if(argc > 1){
	//	level = atoi(argv[1]);
		//printf("%d Arguments given\n",argc);
		int a;
		for(a=0; a<argc; a++){
			//printf("Args: %s\n",argv[a]);
			if(strcmp(argv[a],"-fullscreen") == 0){
				//printf("FULLSCREEN\n");
				fullscreen = true;
			} else if(strcmp(argv[a],"-nomusic") == 0){
				// Turn off music
				music = false;
			} else if(strcmp(argv[a],"-map") == 0){
				level = atoi(argv[a+1]);
				//printf("Level: %d\n",level);
			} else if(strcmp(argv[a],"-lives") == 0){
				//numlives = atoi(argv[a+1]);
				if(a+1 >= argc){
					printf("The -lives parameter requires a number!\n");
					exit(1);
				} else {
					numlives = strtol(argv[a+1],NULL,10);
					if(numlives == 0){
						printf("The -lives parameter requires a number!\n");
						exit(1);
					} else {
						printf("Lives: %d\n",numlives);
						lives = numlives;
					}
				}
			}
		}
	}

	screen = init(fullscreen);

	int ok = loadSprites();
	if(ok == 0){
		cout << "\n\nThis game needs to be installed - run \"make install\" as root.\nI will fix this in later version to allow play from current directory.\n\n";
	}
	score = 0;

	//TTF_Init();
	if(ok!=0) initGame();
	srand(time(0));
	
	if(screen!=NULL && ok>0){
		// Screen was assigned and sprites loaded so carry on
		
		SDL_Event e;
		int exit = 0;

		blit(bg,0,0);
		blit(bg2,550,0);
		
		subgamestate = 1; // used for displaying "Get Ready" etc at start of level

		loadsounds();
		loadHiScores();
		if(music == true)
		Mix_PlayMusic(bgm1, -1);

		//printf("Successfully loaded ghosts\n");

		int gamestate = 0; // 0 = intro, 1 = game, 2 = hiscore, 3 = howtoplay

	//	initLevel(level);

		while(!exit){
			// test keys

			// Some speed control
			tick = SDL_GetTicks();
			delta_tick = tick - lastticks;
			lastticks = tick;

			//limit to 40 revolutions per second (40*25ms = 1 second)
			if(delta_tick < 32){
				SDL_Delay(32 - delta_tick);
				tick = SDL_GetTicks();
				delta_tick = delta_tick + tick - lastticks;
				lastticks = tick;
			}


//			int ax = (xpos / SIZET) - 1;
//			int ay = (ypos / SIZET) - 1;
	
				while(SDL_PollEvent(&e)){
					switch(e.type){
						case SDL_QUIT: exit = 1; break;
					}
					switch(e.key.state){
						case SDL_PRESSED:
							if(e.key.keysym.sym==SDLK_RIGHT){	if(effect==INVERT) nextdir = 3; else nextdir = 4;		}
							if(e.key.keysym.sym==SDLK_LEFT){		if(effect==INVERT) nextdir = 4; else nextdir = 3;		}
							if(e.key.keysym.sym==SDLK_UP){		nextdir = 1;		}
							if(e.key.keysym.sym==SDLK_DOWN){		nextdir = 2;		}
							if(e.key.keysym.sym==SDLK_h){
								if(gamestate==0){
									gamestate=3;
								}
							}
							if(e.key.keysym.sym==SDLK_s){
								if(gamestate==0){
									gamestate=4; // show hiscore
								}
							}
							if(e.key.keysym.sym==SDLK_p){
								if(gamestate==1 && subgamestate==0){
									subgamestate=4;
								} else if(gamestate==1 && subgamestate==4){
									subgamestate=0;
								}
							}
							if(e.key.keysym.sym==SDLK_c){
								if(gamestate==0) gamestate = 5;
							} 
							if(e.key.keysym.sym==SDLK_m){
								if(Mix_PlayingMusic()){
									Mix_HaltMusic();
								} else {
									Mix_PlayMusic(bgm1,-1);
								}
							}

							if(e.key.keysym.sym==SDLK_ESCAPE){
								if(gamestate==0){
									// Quit game
									//level_free_food();
									saveHiScores();
									exit=1;
								} else {
									// Game is running - return to main screen
									// Reset the whole game (maybe need a method for this?)
									level_free_food();
									//freeBonus();
									gamestate=0;
									level = 1;
								}
							}
							if(e.key.keysym.sym==SDLK_SPACE){
								if(gamestate==0){
									lives = numlives;
									score = 0;
									hannah->setAnimation("default");
									gamestate = 1;
									blit(bg,0,0);
									initLevel(level);
								} else if(gamestate==3 || gamestate==4 || gamestate==5){
									gamestate=0;
									level = 1;
								}

							}
							break;
					}
				}


				// game logic
				if(gamestate==0){
					//blit(bg,0,0);
					blit(story,0,0);
					SDL_Flip(screen);
	
				} else if(gamestate==1){
					// Draw bg and map, all bits need this					
					blit(bg2,550,0);
					drawMap();
					drawscore();
					// Game is running
					if(subgamestate==0){
						timer();
						checkpillcollision();
						move();
						drawbaddies();
						createBonus();

						// BUG: if move not called (i.e. effect=='s') then timer never decreases.
						if(effect==FREEZE){
						//	if(red->effect != 's')
						//	red->move(xpos,ypos);
						//	else red->decTimer();
						} else red->move(xpos,ypos);
						if(effect==FREEZE){
						//	if(pink->effect != 's')
						//	pink->move(xpos,ypos);
						//	else pink->decTimer();
						} else pink->move(xpos,ypos);
						if(effect==FREEZE){
						//	if(blue->effect != 's')
						//	blue->move(xpos,ypos);
						//	else blue->decTimer();
						} else blue->move(xpos,ypos);
						level_move_food();
						moveBonus();
						//SDL_Delay(20);
						//SDL_Flip(screen);
						check_bonus_pacman();
						Ghost* tmpghost = check_baddies_pacman();
						if(tmpghost!=NULL && tmpghost->timer()==0 && hannah->isAnimation("default")){
							//printf("Red timer: %d\n",red->timer());

							Mix_PlayChannel(-1,scream,0);
							hannah->setAnimation("die");
							

							//hannah->setAnimation("die");
							//if(hannah->animationFinished()){
								
							//}
						//} else if(){

						} else if(tmpghost!=NULL && tmpghost->timer()>0){
							//printf("Red timer: %d\n",tmpghost->timer());
							// Powerpill is active, eat ghost
							tmpghost->timer(0);
							tmpghost->setAnimation("default");
							tmpghost->setRespawn(50);
							tmpghost->x(tmpghost->startx);
							tmpghost->y(tmpghost->starty);
							Mix_PlayChannel(-1,chomp,0);
							score = score + 400;
							//SDL_Delay(300);
							//red->x(lev.rx);
							//red->y(lev.ry);
						}

						unsigned int gn = 0;
						for(gn=0; gn<ghosties.size(); gn++){
							tmpghost = (Ghost*)ghosties[gn];
							if(tmpghost->timer() == 0 && tmpghost->isAnimation("flash")){
								tmpghost->setAnimation("default");
							}
						}

						if(hannah->isAnimation("die") && hannah->animationFinished()){
							
							lives--;
							if(lives == 0){
								level_free_food();
								//freeBonus();
								subgamestate = 3;
							} else {
								reInitLevel();
								subgamestate = 2;
							}
							//hannah->respawn();
						}

						check_food_pacman();
					//	SDL_Flip(screen);
						SDL_UpdateRect(screen,50,50,450,450);
						SDL_UpdateRect(screen,610,260,150,150);
					// Game is saying "Level N - Get ready"
					} else if(subgamestate==1){
						drawbaddies();
						drawscore();
						drawsubgamestate(1);
						SDL_Flip(screen);
						SDL_Delay(2000);
						subgamestate = 0;
					// Game is saying "Get Ready!" after dying
					} else if (subgamestate==2){
						drawbaddies();
						drawscore();
						drawsubgamestate(2);
						//SDL_Delay(2000);
						SDL_Flip(screen);
						SDL_Delay(2000);
						subgamestate = 0;
					// Game is saying "Game Over"
					} else if (subgamestate==3){
						drawscore();
						drawsubgamestate(subgamestate);
						gamestate = 0;
						//SDL_Delay(2000);
						SDL_Flip(screen);
						SDL_Delay(2000);

						// check for hiscore
						int pos = checkHiScores(score);
						if(pos > -1){
							blit(border,190,100);
							setHiScore(pos);
							SDL_Flip(screen);
							gamestate = 0;
						}

						lives = numlives;
						score = 0;
						level = 1;
					} else if(subgamestate==4){
						//pause
						drawbaddies();
						drawscore();
						SDL_Flip(screen);
					} 
				} else if(gamestate==2){
					// Show hi scores
				} else if(gamestate==3){
					blit(howto,190,100);
					SDL_Flip(screen);
				} else if(gamestate==4){
					blit(border,190,100);
					//getHiScore();
					drawHiScore();
					SDL_Flip(screen);
					//gamestate = 0;
				} else if(gamestate==5){
					blit(border,190,100);
					drawCredits();
					SDL_Flip(screen);
				} else if(gamestate==6){
					endGame();
					gamestate = 0;
				} 
				//SDL_Flip(screen);
			}
	
			// End of loop reached - cleanup
		cleanUp();
	} else {
		// Quit - do any cleanup
		SDL_Quit();
	}
};

