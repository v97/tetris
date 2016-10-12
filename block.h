#include <unistd.h>
#include <QtGui>
#include <QLabel>
#include <QTimer>
#include <QColor>

using namespace std;

class Block : public QWidget {
	Q_OBJECT
private:
	int mid[2]; //Absolute position of 'center' square on game board
	bool blockPosValid();
	int numFullLines();
public:
	Block(QWidget *parent = 0);
	int state; //Orientation
	int type; //Type of block - determines shape and colour (used as index to colours[7] and types arrays)
	static QColor colours[7]; //->
	static int types[7][4][2]; //->> These three static arrays are initialised in block.cpp
	static int rubble[20][10]; //->
	int getposx(int pieceid, bool falling = true);		//By default, these getpos functions are used for the falling block (preview block only if specified by setting falling argument = false)
	int getposy(int pieceid, bool falling = true);
	int shift(int dir, bool checkBlockPosValidity = true); //Shift handles both rotation and movement - dir is the "action identifier"
};
