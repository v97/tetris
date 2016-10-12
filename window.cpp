#include <unistd.h>
#include <QtGui>
#include <QLabel>
#include <QTimer>
#include <strings.h>
#include <window.h>
#include <QColor>

using namespace std;

Window::Window(QWidget *parent) : QWidget(parent), gameover(true), score(0), level(1) //gameover is true until the user presses the up arrow key - then the game starts

{
	statistics = new QLabel("Press the up arrow key\nto play tetris!");	//For score and level
	mainLayout.addWidget(&gameBoard, 0, 0, 2, 1, Qt::AlignCenter);		//Game board takes two rows, one column
	mainLayout.addWidget(&previewWindow, 0, 1, 1, 1, Qt::AlignCenter);	//Preview window takes one row, one column
	mainLayout.addWidget(statistics, 1, 1, 1, 1, Qt::AlignCenter);		//Statistics take one row, one column
	setLayout(&mainLayout);							//Three 'elements' contained within the window
	bg.load("background.bmp");						//Blue to white gradient bmp gameboard background image
	gameBoard.setPixmap(QPixmap::fromImage(bg));
	srand(1); //Initialise random seed
}

void Window::initialise(){
	gameover = false;
	score = 0;
	level = 1;
	bg.load("background.bmp");						//Blue -> white gradient background for game board
	gameBoard.setPixmap(QPixmap::fromImage(bg));
	falling = new Block();							//Initialising 'falling' block
	next = new Block();							//Initialising block that will be previewed
	pbg = QImage{((float) bg.width())/GAMEBOARD_GRID_WIDTH*6, ((float) bg.width())/GAMEBOARD_GRID_WIDTH*6, QImage::Format_RGB32}; //Preview window background
	pbg.fill(Qt::white);							//Filling preview window background with white
	drawBlock(6, 6, false);							//Previews 'next' block
	previewWindow.setPixmap(QPixmap::fromImage(pbg));
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));	//Set timer to call update() every time it triggers
	timer->start(1000);							//Set timer to run every second
	for(int tileID = 0; tileID < 200; tileID++){				//Loop over every tile in the game board
		Block::rubble[tileID / GAMEBOARD_GRID_WIDTH][tileID % GAMEBOARD_GRID_WIDTH] = 0; //Clear pre-existing blocks (incase game has been reset from before)
	}
}

void Window::checkDropped(int fullLines){					//Checks if the falling piece has 'landed' somewhere, and then performs some updates - int parameter is return value of shift()
	sprintf(buffer, "Score: %d\n\nLevel: %d", score, level);		//Write score and level into buffer
	if(fullLines != -1){							//i.e. if there are full lines
		score += (fullLines == GAMEOVER ? 0 : fullLines*10);		//10 points per full line, GAMEOVER = -2
		level = 1+score/100;
		if( (gameover = ( (fullLines == GAMEOVER)||(level > 10) )) ){	//Game over when falling block cannot fit inside game board, or when level exceeds 10
			sprintf(buffer, "GAME OVER!\n\nScore: %d\n\nLevel: %d\n\nPress the up arrow key\nto play tetris again!", score, level);
			delete next;
			delete timer;	//No memory leaks!
		}
		else{
			timer->setInterval(1000 - (level-1)*100);	//Interval shortens as level increases
			delete falling;					//Falling piece has 'landed' - it is deposited into the Blocks::rubble array using shift()
			falling = next;					//Remove 'next' block from preview window and drop it from the top of the game board
			next = new Block();
			next->type = (next->type+((int) (next->type == falling->type))*(1+rand()%6))%7;
			pbg.fill(Qt::white);
			drawBlock(6, 6, false);
			previewWindow.setPixmap(QPixmap::fromImage(pbg));	//Update pixels on preview window - only needs to be done when a block hits the ground
		}
	}
	statistics->setText(buffer); //Update statistics
}

void Window::keyPressEvent(QKeyEvent *event){			//Triggered every time the user presses a key
	if(!gameover){						//Must only act if game is active!
		switch(event->key()){				//Check to see which key the user pressed, and act accordingly
			case Qt::Key_X : {
				falling->shift(ANTICLOCKWISE);
				break;}
			case Qt::Key_Z : {
				falling->shift(CLOCKWISE);
				break;}
			case Qt::Key_Down : {
				checkDropped(falling->shift(DOWN));
				break;}
			case Qt::Key_Left : {
				falling->shift(LEFT);
				break;}
			case Qt::Key_Right : {
				falling->shift(RIGHT);
				break;}
		}
		render();
	}
	else if(event->key() == Qt::Key_Up){	//Up arrow must only initialise if the game is over!
		initialise();			//Start the game!
	}
}

void Window::drawBlock(int winWidth, int winHeight, bool drawFallingBlock){	//Can be used to print BOTH 'falling' and 'next' blocks
	QImage *window = (drawFallingBlock ? &bg : &pbg);			//If intending to draw the falling block, the window will be 'bg' - else, 'pbg' for the preview block
	QPainter painter(window);
	Block *block = (drawFallingBlock ? falling : next);			//Local pointer to Block object which will be drawn (preview block or falling block)
	painter.setPen(Qt::black);						//Set outline colour to black
	for(int pieceID = 0; pieceID < 4; pieceID++){
		painter.fillRect((int) (block->getposx(pieceID, drawFallingBlock)*((float)window->width())/winWidth), (int) (block->getposy(pieceID, drawFallingBlock)*((float)window->height())/winHeight) , window->width()/winWidth, window->height()/winHeight, Block::colours[block->type]); //Fills block with colour	
		painter.drawRect((int) (block->getposx(pieceID, drawFallingBlock)*((float)window->width())/winWidth), (int) (block->getposy(pieceID, drawFallingBlock)*((float)window->height())/winHeight) , window->width()/winWidth, window->height()/winHeight); //Paints black border for blocks
	}
}

void Window::render(){				//For updating all of the graphics
	bg.load("background.bmp");		//Clears game board background QImage
	drawBlock(GAMEBOARD_GRID_WIDTH, GAMEBOARD_GRID_HEIGHT);	//Draws falling block
	QPainter bgPainter(&bg);
	bgPainter.setPen(Qt::black);	//Set outline colour to black
	for(int tileID = 0; tileID < GAMEBOARD_GRID_HEIGHT * GAMEBOARD_GRID_WIDTH; tileID++){	//Draw grid
		if(Block::rubble[tileID / GAMEBOARD_GRID_WIDTH][tileID % GAMEBOARD_GRID_WIDTH] != 0){
			bgPainter.fillRect(((float)(tileID % GAMEBOARD_GRID_WIDTH)) * bg.width() / GAMEBOARD_GRID_WIDTH, ((float) (tileID / GAMEBOARD_GRID_WIDTH)) * bg.height() / GAMEBOARD_GRID_HEIGHT, bg.width()/GAMEBOARD_GRID_WIDTH, bg.height()/GAMEBOARD_GRID_HEIGHT, Block::colours[Block::rubble[tileID / GAMEBOARD_GRID_WIDTH][tileID % GAMEBOARD_GRID_WIDTH]-1]); //Fills inactive blocks with their colours
			bgPainter.drawRect(((float)(tileID % GAMEBOARD_GRID_WIDTH)) * bg.width() / GAMEBOARD_GRID_WIDTH, ((float) (tileID / GAMEBOARD_GRID_WIDTH)) * bg.height() / GAMEBOARD_GRID_HEIGHT, bg.width() / GAMEBOARD_GRID_WIDTH, bg.height() / GAMEBOARD_GRID_HEIGHT); //Outlines inactive blocks in black
		}
	}
	gameBoard.setPixmap(QPixmap::fromImage(bg));	//Update pixels on game board		
}
  
void Window::update(){					//Called every second
	checkDropped(falling->shift(DOWN));		//Drop the block, and perform some position checks
	render();					//Refresh display
}
