#ifndef UNOMULTI_H
#define UNOMULTI_H
#include "unocard.h"
//#include "string"
#include <QObject>

#include <QWidget>
#include <QApplication>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include <cstdlib>
#include <QMouseEvent>
#include <QSound>
#include <QMovie>
#include <QSoundEffect>
#include <QTcpSocket>
#include <QScreen>

namespace Ui {
class UnoMulti;
}

class UnoMulti : public QWidget
{
    Q_OBJECT

signals:
    void gettingClosed(int who); // Envoie à mainwindow pour qu'il se fasse delete

public:
    explicit UnoMulti(QWidget *parent = nullptr);
    ~UnoMulti();

private:
    Ui::UnoMulti *ui;
    void Shuffle(); // Creer le deck, le melange et dstribue les cartes
    void playEffect(QString); // Joue un son une seule fois
    void setBackgroundMusic(QString); // Gere l'audio
    void setScreen(); // Gere le Gui
    void goBotgo(); // Gere le bot
    void setTimer(); // A FAIRE, bouton Uno quand last card
    void refreshInfo(); // Gere le tableau de status de la partie
    bool respectDesRegles(int posCarte); // Verifie que les cartes misent respecte les regles
    void checkLastPlayed(); // Check la derniere carte, notamment les bool dans unocard.h + check si joueur victorieux
    void victoire(bool); // Balance tout ce qui est post victoire d'un joueur

    unocard Deck[108];
    unocard Defausse[108];
    unocard LastPlayed;
    QVector<unocard> Joueur1;
    QVector<unocard> Joueur2;

    bool whichTurn=true; // True quand soi, False quand adverse
    bool whoWined=true; // True quand soi, False quand adverse
    int whoAmI=0;
    bool debutGame=false; // Se met a true quand les deux joueurs ont 7 cartes, histoire de bien faire victoire()

    int whereDeck=107; // Track le deck
    int whereDefausse=0; // Track la defausse

    void mousePressEvent(QMouseEvent * event); // Gere la souris au moment d'un click
    //void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event); // Gere la souris au moment du relachement de la souris
    bool BoutonSourie = false;
    QLabel *trieeLbl; // Not useful
    QLabel *melangeLbl; // Not useful
    QLabel *child;

    QTcpSocket *sock;

    QScreen * screenUnoMulti;

private slots:
    void on_pushRouge_clicked();

    void on_pushJaune_clicked();

    void on_pushBleu_clicked();

    void on_pushVert_clicked();

    void on_pushQuitter_clicked(); // Ferme la fenetre

    void on_pushQuitterOriginal_clicked();


    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError);

    void on_pushConnecter_clicked();
    void on_slideMusic_valueChanged(int value);

};

#endif // UNOMULTI_H
