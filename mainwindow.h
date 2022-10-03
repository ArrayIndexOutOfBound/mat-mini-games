#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSizePolicy>
#include <QScreen>
#include <QGuiApplication>

#include "uno.h"
#include "unomulti.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void windowGotClosed(int who);

private slots:
    void on_pushUno_clicked();

    void on_pushQuitter_clicked();

    void on_pushUnoMulti_clicked();

private:
    Ui::MainWindow *ui;
    Uno * unowindow;
    UnoMulti * unomultiwindow;

    QScreen * screenMain;

};
#endif // MAINWINDOW_H
