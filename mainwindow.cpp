#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "QString"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setWindowTitle("Les mini-jeux de Mathieu");
    screenMain = QGuiApplication::primaryScreen();
    QString os = QGuiApplication::platformName();

    if (os=="android" or os=="ios" or screenMain->size().height()<1080)
    {
        QFont font;

        font.setPointSize(ui->label_Menu->font().pointSize()*(50.0/70.0));
        ui->label_Menu->setFont(font);

        font.setPointSize(ui->pushUno->font().pointSize()*(50.0/70.0));
        ui->pushUno->setFont(font);
        ui->pushUnoMulti->setFont(font);

        font.setPointSize(ui->pushQuitter->font().pointSize()*(50.0/70.0));
        ui->pushQuitter->setFont(font);
    }

    float h = screenMain->availableSize().height(); // available sinon ca prend aussi la partie reserved
    float w = screenMain->availableSize().width();

    // Calculs avec comme base la taille 1920*1080, s'adapter aux écrans

    resize(screenMain->availableSize().width(),screenMain->availableSize().height());
    ui->gridLayoutWidget->setGeometry(w*(1.0/48.0),h*(35.0/108.0),w*(91.0/96.0),h*(35.0/108.0));    
    ui->label_Menu->setGeometry(w*(1.0/64.0),h*(1.0/12.0),w*(31.0/32.0),h*(19.0/108.0));
    ui->pushQuitter->setGeometry(w*(37.0/48.0),h*(7.0/9.0),w*(37.0/192.0),h*(1.0/12.0));

    QTimer::singleShot(0, this, SLOT(showFullScreen())); // Plein ecran

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::windowGotClosed(int who) // 1 = Uno , 2 = UnoMulti
{
    qDebug() << "Mainwindow : windowGotClosed, valeur = " << who ;
    this->show();
    switch (who) {
    case 1:
        disconnect(unowindow, SIGNAL(gettingClosed(int)),this, SLOT(windowGotClosed(int)));
        delete unowindow;
        break;
    case 2:
        disconnect(unomultiwindow, SIGNAL(gettingClosed(int)),this, SLOT(windowGotClosed(int)));
        delete unomultiwindow;
        break;
    default:
        break;
    }
}

void MainWindow::on_pushUno_clicked()
{
    unowindow = new Uno();
    connect(unowindow, SIGNAL(gettingClosed(int)),this, SLOT(windowGotClosed(int)));

    unowindow->show();
    this->hide();
}


void MainWindow::on_pushQuitter_clicked()
{
    close();
}

void MainWindow::on_pushUnoMulti_clicked()
{
    unomultiwindow = new UnoMulti();
    connect(unomultiwindow, SIGNAL(gettingClosed(int)),this, SLOT(windowGotClosed(int)));
    unomultiwindow->show();
    this->hide();
}

