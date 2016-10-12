#include <unistd.h>
#include <QtGui>
#include <QLabel>
#include <QTimer>
#include <strings.h>
//#include <block.h>
#include <window.h>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); //Using QT for GUI

    Window *window = new Window();
    window->show();
    return a.exec();
}
