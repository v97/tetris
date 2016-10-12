#include <unistd.h>
#include <QtGui>
#include <QLabel>
#include <QTimer>
#include <strings.h>
#include <block.h>

#define GAMEBOARD_GRID_WIDTH 10 //#define tags for readability in .cpp programs
#define GAMEBOARD_GRID_HEIGHT 20
#define ANTICLOCKWISE 2
#define CLOCKWISE -2
#define DOWN 0
#define LEFT -1
#define RIGHT 1
#define GAMEOVER -2

using namespace std;

class Window : public QWidget{	
private: //Important data is not accessible to other classes (encapsulation)
	Q_OBJECT
	QImage bg, pbg;
	QLabel gameBoard, previewWindow, *statistics; //Three elements in the window;
	QGridLayout mainLayout; //To facilitate the layout of the main window
	int score;
	int level;
	bool gameover;
	char buffer[100];
	QTimer *timer;
	Block *falling, *next; //Pointers to the only two block objects that will ever be displayed (they will be generated in cycles, on the heap)
	void initialise();
	void checkDropped(int numlines);
	void keyPressEvent(QKeyEvent *event);
	void drawBlock(int winHeight, int winWidth, bool drawFallingBlock = true); //This draws the falling block by default, unless drawFallingBlock is false - then it draws the preview block
	void render();

public:
	Window(QWidget *parent = 0);

public slots: //Slot and signal mechanism to run update() every second
	void update();
};
