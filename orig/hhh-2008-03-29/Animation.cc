// Loads and stores an animation sequence
//
#include "Animation.h"
#include <string>
#include <fstream>
#include <iostream>

#define delay_set 3

Animation::Animation(char* spritename, char* filename, bool loop, char* knownAs){
	this->currentframe = 0;
	this->name = filename;
	this->loop = loop;
	this->finished = false;
	this->delay = delay_set;
	numframes = 0;

	string sprite(spritename);
	string fullpath(filename);

	this->knownAs = knownAs;
	//printf("Loading sprite at %s/%s/sprite.dat\n",spritename,filename);
	#ifdef WIN32
	fullpath = sprite + "\\" + fullpath + "\\sprite.dat";
	#else
	fullpath = DATA_INSTALL_DIR "/" + sprite + "/" + fullpath + "/sprite.dat";
	#endif
	//printf("Opening %s \n",fullpath.c_str());
	ifstream br(fullpath.c_str());
	char buffer[30];
	if(!br.is_open()){
		printf("Could not open spritefile!\n");
	} else {
		// File opened OK, get numframes followed by list
		//printf("File Opened OK\n");
		br.getline(buffer,30);
		numframes = atoi(buffer);
		//printf("Reading %d lines\n",numframes);
		int i = 0;
		for(i = 0; i < numframes; i++){
			br.getline(buffer,30);
			//printf("Loading image %s \n", buffer);
			string filepath(filename);
			filepath = "/" + sprite + "/" + filepath + "/" + buffer;
			string dir(DATA_INSTALL_DIR);
			dir = dir + "/" + filepath;
			//printf("Filepath: %s\n",filepath.c_str());
			SDL_Surface* s;
			s = SDL_DisplayFormatAlpha(IMG_Load(dir.c_str()));
			//SDL_SetColorKey(s, SDL_SRCCOLORKEY,
				//SDL_MapRGB(s->format,0x00,0xFF,0xFF));

			frames.push_back(s);
		}		
		numframes = frames.size();
		br.close();
	}
	//printf("Finished with Animation constructor: %d frames\n",numframes);
};

char* Animation::getKnown(){
	return knownAs;
};

void Animation::reset(){
	currentframe=0;
	finished=false;
};

SDL_Surface* Animation::getFrame(int fnum){
	//cout << knownAs << " > " << fnum << "\n";
	if(fnum>=0 && fnum<numframes-1){
		return frames[fnum];
	} else {
		return frames[0];
	}
};

SDL_Surface* Animation::getFrame(){
	if(currentframe==numframes-1 && loop==true){
		currentframe = 0;
	}
	if(currentframe==numframes-1 && loop==false){
		currentframe = currentframe;
		finished = true;
	} 
	if(currentframe<numframes-1){
		delay--;
		if(delay==0){
			currentframe++;
			delay = delay_set;
		}
	}
	//printf("Return frame %d\n",currentframe);
	return frames[currentframe];
};

Animation::~Animation(){
	int current = 0;
	for(current=0; current<numframes; current++){
		//cout << "Deleting Animation Frame\n";
		SDL_FreeSurface(frames[current]);
	}
};
