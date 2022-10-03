#ifndef UNO_H
#define UNO_H
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
#include <QColor>

#include <QtGui>
#include <QDesktopWidget>
#include <QSizePolicy>
#include <QScreen>

namespace Ui {
class Uno;
}

class Uno : public QWidget
{
    Q_OBJECT

public:
    explicit Uno(QWidget *parent = nullptr);
    ~Uno();

    signals:
        void gettingClosed(int who); // Envoie à mainwindow pour qu'il se fasse delete

private slots:
    void on_pushRouge_clicked();

    void on_pushJaune_clicked();

    void on_pushBleu_clicked();

    void on_pushVert_clicked();

    void on_pushInfo_clicked(); // Affiche le tour actuel + laisse le bot jouer si cliqué

    void on_pushRecommencer_clicked(); // Reset entier, comme si ca venait d'etre ouvert

    void on_pushQuitter_clicked(); // Ferme la fenetre

    void on_pushQuitterOriginal_clicked();

    void on_slideMusic_valueChanged(int value);


private:
    Ui::Uno *ui;
    void Shuffle(); // Creer le deck, le melange et dstribue les cartes
    void playEffect(QString); // Joue un son une seule fois
    void setBackgroundMusic(QString); // Gere l'audio
    void setScreen(); // Gere le Gui
    void goBotgo(); // Gere le bot
    void setTimer(); // A FAIRE, bouton Uno quand last card
    void refreshInfo(); // Gere le tableau de status de la partie
    bool respectDesRegles(int posCarte); // Verifie que les cartes misent respecte les regles
    void checkLastPlayed(); // Check la derniere carte, notamment les bool dans unocard.h + check si joueur victorieux
    void victoire(); // Balance tout ce qui est post victoire d'un joueur


    unocard Deck[108];
    unocard Defausse[108];
    unocard LastPlayed;
    QVector<unocard> Joueur1;
    QVector<unocard> Joueur2;

    bool whichTurn=true; // True au joueur, False au bot
    bool whoWined=true; // True au joueur, False au bot

    int whereDeck=107; // Track le deck
    int whereDefausse=0; // Track la defausse

    void mousePressEvent(QMouseEvent * event); // Gere la souris au moment d'un click
    //void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event); // Gere la souris au moment du relachement de la souris
    bool BoutonSourie = false;
    QLabel *trieeLbl; // Not useful
    QLabel *melangeLbl; // Not useful
    QLabel *child;

    QScreen * screenUno;

    QTimer timer;
    int secondePasse=0;
    void timerTick();

    /* https://www.soundsnap.com/tags/card_game
    https://www.storyblocks.com/audio/search/card+game+sound+effect*/
};

#endif // UNO_H
