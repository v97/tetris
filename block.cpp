#include <unistd.h>
#include <QtGui>
#include <QLabel>
#include <QTimer>
#include <QColor>
#include <strings.h>
#include <window.h>

using namespace std;

Block::Block(QWidget *parent) : QWidget(parent), type(rand()%7), state(rand()%4), mid({GAMEBOARD_GRID_WIDTH/2, 2}){} //Generates random block type, colour and orientation and initialises starting position

int Block::types[7][4][2] = { //Static array of relative positions (to center) of each 'piece' of each block
	{{0, 0}, {0, 1}, {0, -1}, {0, -2}},	//Line
	{{0, 0}, {-1, 0}, {-1, 1}, {0, 1}},	//Square
	{{0, 0}, {0, -1}, {1, 0}, {1, 1}},	//'Z' shape 1
	{{0, 0}, {1, -1}, {1, 0}, {0, 1}},	//'Z' shape 2
	{{0, 0}, {0, -1}, {0, 1}, {1, 1}},	//'L' shape 1
	{{0, 0}, {0, -1}, {0, 1}, {-1, 1}},	//'L' shape 2
	{{0, 0}, {-1, 0}, {1, 0}, {0, -1}}	//'T' shape
};

QColor Block::colours[7]={Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, QColor{255, 165, 0}, Qt::magenta}; //Static array with corresponding colours for painting - (255, 165, 0) is orange

int Block::rubble[20][10] = {0}; //Static array to contain the colours of all settled block pieces

int Block::getposx(int pieceid, bool falling){ //Returns absolute x-position of the specified piece of the block, taking into account orientation, center position and whether the block is falling or preview
	return (falling ? mid[0] : 2) + (((state > 1) && falling) ? -1 : 1) * types[type][pieceid][(falling ? state%2 : 0)]; //Basically, this is matrix multiplication!
}

int Block::getposy(int pieceid, bool falling){ //Y-position
	return (falling ? mid[1] : 2) + (((state%3 == 0) || (!falling)) ? 1 : -1) * types[type][pieceid][(falling ? state+1 : 1)%2]; //Matrix multiplication again for y-component
}

bool Block::blockPosValid(){
	for(int pieceID = 0; pieceID < 4; pieceID++){
		if((getposy(pieceID) >= GAMEBOARD_GRID_HEIGHT) || (getposx(pieceID) < 0) || (getposx(pieceID) >= GAMEBOARD_GRID_WIDTH)){ //If piece is beyond the left, right or bottom edge, return 'invalid'
			return false;
		}
		else if((getposy(pieceID) >= 0) && (rubble[getposy(pieceID)][getposx(pieceID)] != 0)){ //If piece clashes with already settled block, return false
			return false;
		}
	}
	return true;
}

int Block::numFullLines(){
	int numFullLines = 0;
	for(int row = 0; row < GAMEBOARD_GRID_HEIGHT; row++){			//Loop through every row of the settled pieces to check for full lines
		bool filled = true;
		for(int col = 0; col < GAMEBOARD_GRID_WIDTH; col++){
			filled = (filled && (rubble[row][col] != 0));		//filled == true only if EVERY slot in the row is empty
		}
		if(filled){
			for(int srcrow = row - 1; srcrow >= 0; srcrow--){	//Delete full lines
				for(int srccol = 0; srccol < GAMEBOARD_GRID_WIDTH; srccol++){
					if(srcrow > 0){
						rubble[srcrow+1][srccol] = rubble[srcrow][srccol]; //Drops each row that is above the filled line
					}
					else{
						rubble[0][srccol] = 0;	//Sets top row empty
					}
				}
			}
		}
		numFullLines += (int) filled;	//Increments total by one if row is filled
	}
	return numFullLines;
}

int Block::shift(int dir, bool checkBlockPosValidity){
	if(((dir+4)%4) == 2){			//(If dir == -2 OR 2)
		state = (state + 4 + dir/2)%4;	//Rotate block 90 degrees, clockwise if abs(dir) > 0
	}
	else{
		if(dir){		//dir != 0
			mid[0] += dir;	//Move block left/right
		}
		else{			//dir == 0 means DOWN
			mid[1]++;	//Move block down
		}
	}
	if(checkBlockPosValidity && !blockPosValid()){ //Check for validity of block's new position
		if(dir == 0){
			for (int pieceID = 0; pieceID < 4; pieceID++){
				rubble[getposy(pieceID)-1][getposx(pieceID)] = type + 1;// 1 is subtracted because current position is invalid, and correction hasn't been made yet
				if(getposy(pieceID)-1 < 2){	//If any settled block occupies the top row (or the row below it)
					mid[1]--;		//Move the block back up
					return GAMEOVER;	//GAME OVER = -2 (define tag in window.h)
				}
			}
			return numFullLines();
		}
		shift(-dir, false); //Block position is invalid, so this reverses the rotation/movement without checking validity (MUST be valid since the block came from there)
	}
	return -1; //No full lines
}
