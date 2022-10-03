#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << QDir::currentPath() ;
    qDebug() << QDir::current();
    MainWindow w;
    w.show();
    return a.exec();

    // UNO, DEMINEUR, MORPION, ECHEC, LOUP GAROU, TOUR PAR TOUR, TOWER DEFENSE, TEXAS HOLD EM, WORMS, PRINCE OF PERSIA DS
	// https://www.daniweb.com/programming/software-development/threads/433701/creating-text-file-with-variable-name-in-directory
}
