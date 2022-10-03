#include "uno.h"
#include "ui_uno.h"

//QMediaPlaylist* playlist = new QMediaPlaylist;
//QMediaPlayer *audio = new QMediaPlayer;

QSoundEffect effetUnoSolo;
QSoundEffect backgroundMusicUnoSolo;

Uno::Uno(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uno)
{
    ui->setupUi(this);

    setScreen(); // Initialise tout ce qui est en rapport avec le visuel
    Shuffle(); // Initialise le deck, le melange, et distribue les cartes
    setBackgroundMusic("daddydaddydo"); // Initialise la playlist et lance le 1er son
    setTimer();
    effetUnoSolo.setLoopCount(1);
    backgroundMusicUnoSolo.setLoopCount(QSoundEffect::Infinite);

    //QSound::play("mysounds/bells.wav"); // Joue un son 1 fois seulement

    /*
    QMovie* leGif = new QMovie(":/Autre/testgif");
    ui->label_TestGif->setMovie(leGif);
    leGif->setScaledSize(QSize(80,80));
    leGif->start();
    */

}

Uno::~Uno() // Ne semble pas très fonctionnel, il n'est jamais call
{
    backgroundMusicUnoSolo.stop();
    effetUnoSolo.stop();
    qDebug() << "Deconstructeur de UNO";
    delete ui;
    //emit gettingClosed(1);
}

void Uno::setBackgroundMusic(QString nom)
{
    /*    playlist->insertMedia(QUrl::fromLocalFile("D:\\Musique\\Cube_world_boss_fight.wav")); Chemin absolu
    //playlist->addMedia(QUrl::fromLocalFile("./zalut/Cube_world_boss_fight.wav")); Chemin relatif
    // LE CHEMIN EST RELATIF AU .EXE QUI EST GENERE ET NON LE CODE SOURCE
    playlist->removeMedia(0,100); // Histoire d'etre sur
    playlist->insertMedia(1,QUrl("qrc:/Song/putinwalk")); // qrc:/Song/putinwalk ne fonctionne pas dans la release mais dans l'IDE
    playlist->insertMedia(2,QUrl("qrc:/Song/cubeworld"));
    playlist->insertMedia(3,QUrl("qrc:/Song/gaster"));
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop); //Fait en sorte que tant qu'on ne change pas a la main la musique, elle loop
    connect(audio, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64))); //Initialise l'audio
    audio->setVolume(100);
    audio->setPlaylist(playlist);
    audio->play();
    //playlist->setCurrentIndex(INT); //Changer la position de la plylist, commence à 0     */
    backgroundMusicUnoSolo.stop();
    QString chemin = ":/Song/"+nom;
    backgroundMusicUnoSolo.setSource(QUrl::fromLocalFile(chemin));
    backgroundMusicUnoSolo.play();
}

void Uno::setScreen()
{
    screenUno = QGuiApplication::primaryScreen();
    //ui->labelLEDEBUG->setText(QString::number(screen->size().width())+QString::number(screen->size().height()));
    resize(screenUno->availableSize().width(),screenUno->availableSize().height()); // Bon bah va falloir TOUT refaire EN RELATIF ici eheh IWantDead

    QPixmap fondecran(QPixmap(":/Uno/Table0")); // Fond Ecran
    fondecran = fondecran.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window ,fondecran);
    //palette.setBrush(QPalette::Background, fondecran);
    this->setPalette(palette); // Fin Fond Ecran

    Uno::setWindowTitle("El Uno de Mathieu"); // Titre de la fenetre
    ui->horizontalJoueur1->setAlignment(Qt::AlignLeft); // TOUT GARDER A GAUCHE SOUS PEINE D'EXPLOSION DU SYSTEME
    ui->horizontalJoueur2->setAlignment(Qt::AlignRight); // Same raison
    ui->pushInfo->setStyleSheet("background-color: rgb(200, 200, 200);border: none;border-radius: 15px;"); // Arrondie les bords des boutons
    ui->pushQuitterOriginal->setStyleSheet("background-color: rgb(200, 200, 200);border: none;border-radius: 15px;");

    QTimer::singleShot(0, this, SLOT(showFullScreen())); // Plein ecran
    //showFullScreen(); // Plein ecran aussi?




    ui->gridMilieu->setAlignment(Qt::AlignLeft);
    ui->pushRouge->setVisible(false);
    ui->pushRouge->setStyleSheet("background-color: rgb(255, 0, 0); border: none;border-radius: 15px;");
    ui->pushRouge->update();
    ui->pushJaune->setVisible(false);
    ui->pushJaune->setStyleSheet("background-color: rgb(255, 255, 0); border: none;border-radius: 15px;");
    ui->pushJaune->update();
    ui->pushBleu->setVisible(false);
    ui->pushBleu->setStyleSheet("background-color: rgb(0, 0, 255); border: none;border-radius: 15px;");
    ui->pushBleu->update();
    ui->pushVert->setVisible(false);
    ui->pushVert->setStyleSheet("background-color: rgb(0, 255, 0); border: none;border-radius: 15px;");
    ui->pushVert->update();
    ui->pushQuitter->setVisible(false);
    ui->pushRecommencer->setVisible(false);
    ui->label_Fin->setVisible(false);

    // C'est partie pour tout changer en relatif à l'écran
    float w = screenUno->availableSize().width();
    float h = screenUno->availableSize().height();

    QString os = QGuiApplication::platformName();

    static unsigned short yo=0;
    if ((os=="android" or os=="ios" or screenUno->size().height()<1080) and yo==0) // Potentiellement inutile puisque c'est de base petit
    {
        QFont font;
        font.setPointSize(ui->label_CarteMain1->font().pointSize()*(5.0/7.0));
        ui->label_Fin->setFont(font);
        ui->label_CarteMain1->setFont(font);
        ui->label_CarteMain2->setFont(font);
        ui->label_NumberLast->setFont(font);
        ui->label_CouleurLast->setFont(font);
        ui->label_CompteurDeck->setFont(font);
        ui->label_CompteurDefausse->setFont(font);
        ui->pushQuitterOriginal->setFont(font);
        ui->pushInfo->setFont(font);
        ui->pushQuitter->setFont(font);
        ui->pushRecommencer->setFont(font);
        yo++;
    }
    // Calculs avec comme base la taille 1920*1080, s'adapter aux écrans

    ui->horizontalLayoutWidget->setGeometry(w*(11.0/32.0),h*(79.0/108.0),w*(41.0/64.0),h*(13.0/54.0)); // Joueur 1
    ui->horizontalLayoutWidget_2->setGeometry(w*(1.0/48.0),h*(1.0/27.0),w*(41.0/64.0),h*(13.0/54.0)); // Joueur 2
    ui->gridLayoutWidget->setGeometry(w*(35.0/48.0),h*(11.0/108.0),w*(25.0/96.0),h*(59.0/108.0)); // Grid info
    ui->gridLayoutWidget_2->setGeometry(w*(17.0/48.0),h*(43.0/108.0),w*(53.0/192.0),h*(13.0/54.0)); // Grid Milieu
    ui->gridLayoutWidget_3->setGeometry(w*(7.0/96.0),h*(65.0/108.0),w*(5.0/48.0),h*(1.0/12.0)); // Grid Music
    ui->label_Fin->setGeometry(w*(11.0/48.0),h*(4.0/27.0),w*(101.0/192.0),h*(35.0/108.0));
    ui->pushInfo->setGeometry(w*(13.0/192.0),h*(13.0/18.0),w*(7.0/48.0),h*(5.0/54.0));
    ui->pushQuitterOriginal->setGeometry(w*(13.0/192.0),h*(23.0/27.0),w*(7.0/48.0),h*(5.0/54.0));
    ui->pushRecommencer->setGeometry(w*(49.0/192.0),h*(71.0/108.0),w*(23.0/96.0),h*(1.0/6.0));
    ui->pushQuitter->setGeometry(w*(97.0/192.0),h*(71.0/108.0),w*(23.0/96.0),h*(1.0/6.0));

    ui->label_LastPlayed->setFixedSize(ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
    ui->label_Deck->setFixedSize(ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());

    ui->label_Fin->setWordWrap(true);
    ui->label_CarteMain1->setWordWrap(true);
    ui->label_CarteMain2->setWordWrap(true);
    ui->label_NumberLast->setWordWrap(true);
    ui->label_CouleurLast->setWordWrap(true);
    ui->label_CompteurDeck->setWordWrap(true);
    ui->label_CompteurDefausse->setWordWrap(true);

}

void Uno::setTimer()
{
    timer.stop();
    timer.setInterval(1000);
}

void Uno::refreshInfo()
{
    ui->label_CarteMain1->setText("Tu as "+QString::number(Joueur1.length())+" cartes en main.");
    ui->label_CarteMain2->setText("Il a "+QString::number(Joueur2.length())+" cartes en main.");
    QString tempCouleur;
    QString tempNum;
    tempNum = QString::number(LastPlayed.nombre);
    if (LastPlayed.couleur==1)tempCouleur="Rouge";
    if (LastPlayed.couleur==2)tempCouleur="Jaune";
    if (LastPlayed.couleur==3)tempCouleur="Bleu";
    if (LastPlayed.couleur==4)tempCouleur="Vert";
    if (tempNum=="10") tempNum="+2";
    if (tempNum=="11") tempNum="Reverse";
    if (tempNum=="12") tempNum="Skip";
    if (tempNum=="13") tempNum="+4";
    if (tempNum=="14") tempNum="Joker";

    ui->label_CouleurLast->setText("La couleur actuelle est "+tempCouleur);
    ui->label_NumberLast->setText("Le numéro actuel est : "+tempNum);
    ui->label_CompteurDeck->setText("Il y a "+QString::number(whereDeck)+" cartes dans le deck");
    ui->label_CompteurDefausse->setText("Il y a "+QString::number(whereDefausse)+" cartes dans la defausse");

    if (whichTurn==true) // Si c'est le tour de J1
    {
        ui->pushInfo->setEnabled(false);
        if (ui->pushRouge->isVisible())
        {
            qWarning("En attente couleur par Joueur1");
            ui->pushInfo->setText("En attente de couleur");
        }
        else
        {
            qWarning("Attente action Joueur1");
            ui->pushInfo->setText("C'est ton tour");
        }
    }
    else // Si c'est le tour de J2
    {
        if (ui->pushRouge->isVisible())
        {
            qWarning("En attente couleur par Joueur1");
            ui->pushInfo->setText("En attente de couleur");
        }
        else
        {
            qWarning("Laisser le bot jouer : activé");
            ui->pushInfo->setEnabled(true);
            ui->pushInfo->setText("Laisser le bot jouer");
        }
    }


}


void Uno::Shuffle()
{
    playEffect("shuffle");
    int compteurDeck=0;
    int couleurActuel=1;
    int nombreActuel=0;
    QString Couleur="R";

    while (couleurActuel<=4 and compteurDeck<=100)
    {

        if (compteurDeck==25 or compteurDeck==50 or compteurDeck==75)
        {
            if (Couleur=="B") Couleur="V";
            if (Couleur=="J") Couleur="B";
            if (Couleur=="R") Couleur="J";
            couleurActuel++;
            nombreActuel=0;
        }

        if (nombreActuel<13)
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=nombreActuel;
            Deck[compteurDeck].cheminImage=":/Uno/"+Deck[compteurDeck].nomCouleur+QString::number(nombreActuel);
            if (Deck[compteurDeck].nombre==10) Deck[compteurDeck].IsA2=true;
            if (Deck[compteurDeck].nombre==11) Deck[compteurDeck].IsAReverse=true;
            if (Deck[compteurDeck].nombre==12) Deck[compteurDeck].IsASkip=true;
            Deck[compteurDeck].label = new QLabel;
            Deck[compteurDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
        }
        else
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=nombreActuel-12;
            Deck[compteurDeck].cheminImage=":/Uno/"+Deck[compteurDeck].nomCouleur+QString::number(nombreActuel-12);
            if (Deck[compteurDeck].nombre==10) Deck[compteurDeck].IsA2=true;
            if (Deck[compteurDeck].nombre==11) Deck[compteurDeck].IsAReverse=true;
            if (Deck[compteurDeck].nombre==12) Deck[compteurDeck].IsASkip=true;
            Deck[compteurDeck].label = new QLabel;
            Deck[compteurDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
        }
        nombreActuel++;
        compteurDeck++;

    }
    compteurDeck--;
    while (compteurDeck<=107)
    {
        Couleur="N";
        couleurActuel=5;
        if (compteurDeck<=103)
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=13;
            Deck[compteurDeck].IsA4=true;
            Deck[compteurDeck].cheminImage=":/Uno/N+4";
            Deck[compteurDeck].label = new QLabel;
            Deck[compteurDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
        }
        else
        {
            Deck[compteurDeck].couleur=couleurActuel;
            Deck[compteurDeck].nomCouleur=Couleur;
            Deck[compteurDeck].nombre=14;
            Deck[compteurDeck].IsAJoker=true;
            Deck[compteurDeck].cheminImage=":/Uno/NJoker";
            Deck[compteurDeck].label = new QLabel;
            Deck[compteurDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
        }
        compteurDeck++;
    }

    int i =0;
    bool tabDone[108];
    unocard tempo[108];
    while (i<108)
    {
        tabDone[i]=false;
        tempo[i]=Deck[i];
        i++;
    }
    i=0;
    while (i<108) // Melange les cartes de manière aléatoire
    {
        int aleatoire= rand()%108;
        if (tabDone[aleatoire]==false)
        {
            Deck[i]=tempo[aleatoire];
            tabDone[aleatoire]=true;
            i++;
        }

    }

    whereDeck=107;
    whereDefausse=0;

    while (Joueur1.length()>0) // Utile lorsque l'on reset le jeu
    {
        delete Joueur1[0].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
        qWarning("Vidage J1");
        Joueur1.remove(0);
    }
    while (Joueur2.length()>0) // Utile lorsque l'on reset le jeu
    {
        delete Joueur2[0].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
        qWarning("Vidage J2");
        Joueur2.remove(0);
    }

    for (int i=0;i<7;i++) // Distribue les cartes au J1
    {
        Joueur1.push_back(Deck[whereDeck]);
        whereDeck--;
        ui->horizontalJoueur1->addWidget(Joueur1[i].label);
    }

    for (int i=0;i<7;i++) // Distribue les cartes au J2
    {
        Joueur2.push_back(Deck[whereDeck]);
        whereDeck--;
        Joueur2[i].setupImage(true,ui->horizontalLayoutWidget_2->size().width()/7.0,ui->horizontalLayoutWidget_2->size().height());
        ui->horizontalJoueur2->addWidget(Joueur2[i].label);
    }

    // Initialiser le LastPlayed et affichage de la carte Deck
    LastPlayed=Deck[whereDeck];
    Defausse[whereDefausse]=Deck[whereDeck];
    whereDeck--;
    whereDefausse++;

    ui->label_Deck->setScaledContents(true);
    //ui->label_Deck->setFixedSize(176,256);
    ui->label_Deck->setPixmap(QPixmap(":/Uno/Back"));

    ui->label_LastPlayed->setScaledContents(true);
    //ui->label_LastPlayed->setFixedSize(176,256);
    ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);

    if (LastPlayed.couleur==5)
    {
        LastPlayed.couleur= (rand()%4)+1;
    }
    LastPlayed.IsA2=false;
    LastPlayed.IsA4=false;
    LastPlayed.IsASkip=false;
    LastPlayed.IsAReverse=false;
    LastPlayed.IsAJoker=false;
    refreshInfo();
}

void Uno::playEffect(QString nom)
{
    effetUnoSolo.stop();
    QString chemin = ":/Effect/"+nom;
    effetUnoSolo.setSource(QUrl::fromLocalFile(chemin));
    effetUnoSolo.setVolume(0.5);
    effetUnoSolo.play();
}


void Uno::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::RightButton)
       {
           qWarning("CLIC DROIT ENFONCED");
       }
    if(event->button() == Qt::LeftButton)
    {
        QLabel *temp = static_cast<QLabel*>(childAt(event->pos())); // Detecte le Label qui est sur la souris
        child = temp;

        int tailleActuelleCarte = ui->horizontalLayoutWidget->width()/Joueur1.length();
        if (tailleActuelleCarte>ui->horizontalLayoutWidget->width()/7) tailleActuelleCarte=ui->horizontalLayoutWidget->width()/7;

        if (whereDeck==1)
        {
            while (whereDefausse>1) // On reverifie le deck
            {
                Deck[whereDeck]=Defausse[whereDefausse];
                Deck[whereDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
                whereDeck++;
                whereDefausse--;
            }
        }

        if (event->x() > ui->horizontalLayoutWidget->x() and event->x() < ui->horizontalLayoutWidget->x()+(tailleActuelleCarte*Joueur1.length()) and whichTurn==true) // CARTES JOUEUR, s'adapte pour ne pas crash l'application.
        {
            if (event->y() > ui->horizontalLayoutWidget->y() and event->y() < ui->horizontalLayoutWidget->y() + ui->horizontalLayoutWidget->height())
            {
                int positionCarteSouris=((event->x()-ui->horizontalLayoutWidget->x()-(Joueur1.length()/tailleActuelleCarte))/tailleActuelleCarte);
                // Obtient la position de la carte entre 0 et length()-1
                qDebug() << positionCarteSouris;

                if (respectDesRegles(positionCarteSouris)) // Si la carte selectionné respecte les règles
                {
                    playEffect("place");
                    qWarning("Une carte est joué par Joueur1");
                    Defausse[whereDefausse]=Joueur1[positionCarteSouris];
                    whereDefausse++;
                    LastPlayed=Joueur1[positionCarteSouris];
                    ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
                    Joueur1.remove(positionCarteSouris);
                    child->close();
                    whichTurn=false;
                    checkLastPlayed();
                }

            }
        }

        if (event->x() > screenUno->size().width()*(17.0/48.0) and event->x() < screenUno->size().width()*(17.0/48.0)+ui->label_Deck->width() and whichTurn==true) // Position souris = pioche
        {
            if (event->y() > screenUno->size().height()*(43.0/108.0) and event->y() < screenUno->size().height()*(43.0/108.0)+ui->label_Deck->height())
            {
                playEffect("draw");
                qWarning("Une carte est pioché par Joueur1");
                Joueur1.push_back(Deck[whereDeck]);
                ui->horizontalJoueur1->addWidget(Joueur1.back().label);
                whereDeck--;
                whichTurn=false;
                checkLastPlayed();
            }
        }

        if (whereDeck==0) // Check que le deck ne soit pas vide
        {
            while (whereDefausse>=0)
            {
                Deck[whereDeck]=Defausse[whereDefausse];
                Deck[whereDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
                whereDeck++;
                whereDefausse--;
            }
        }

        BoutonSourie = true;
        qWarning("CLIC GAUCHE ENFONCED");
        //ui->label_2->setText(QString::number(event->x()-658-(Joueur1.length()/tailleActuelleCarte)));
        //ui->label_3->setText(QString::number((event->x()-658-(Joueur1.length()/tailleActuelleCarte))/tailleActuelleCarte));
        refreshInfo();
    }
}

void Uno::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
       {
           qWarning("CLIC DROIT RELACHED");
       }
    if(event->button() == Qt::LeftButton)
           {

        BoutonSourie = false;

        qWarning("CLIC GAUCHE RELACHED");
    }
}


/*void Uno::mouseMoveEvent(QMouseEvent* event)
{
    BoutonSourie=false; // Empeche toute action de ce sous programme, servira plus tard.
    if(BoutonSourie)
    {
        qDebug() << QString::number(event->pos().x())<<","<<QString::number(event->pos().y());



            if (!child)
            {
                return;
            }
            else
            {
                //setMouseTracking(true);
                //child->move(event->x()-child->width()/2,event->y()-child->height()/2);
            }

    }

}*/

bool Uno::respectDesRegles(int posCarte) // Si la carte que l'on souhaite poser soit conforme aux règles
{
    if (whichTurn==true) // Si tour J1
    {
        if((LastPlayed.nombre) == (Joueur1[posCarte].nombre)) // Check le nombre
        {
            return true;
        }

        if((LastPlayed.couleur) == (Joueur1[posCarte].couleur)) // Check la couleur
        {
            return true;
        }

        if(Joueur1[posCarte].couleur == 5) // Il faut choisir la couleur du joker / +4
        {
            ui->pushRouge->setVisible(true);
            ui->pushJaune->setVisible(true);
            ui->pushBleu->setVisible(true);
            ui->pushVert->setVisible(true);
            return true;
        }
    }
    else // Si tour J2
    {
        if((LastPlayed.nombre) == (Joueur2[posCarte].nombre)) // Check le nombre
        {
            return true;
        }

        if((LastPlayed.couleur) == (Joueur2[posCarte].couleur)) // Check la couleur
        {
            return true;
        }

        if(Joueur2[posCarte].couleur == 5) // Il faut choisir la couleur du joker / +4
        {
            return true;
        }
    }


    return false;
}

void Uno::checkLastPlayed()
{
    if (LastPlayed.nombre>=0 and LastPlayed.nombre<=9)
    {
        LastPlayed.IsA2=false;
        LastPlayed.IsA4=false;
        LastPlayed.IsASkip=false;
        LastPlayed.IsAReverse=false;
        LastPlayed.IsAJoker=false;
    }

    qWarning("Entrée checkLastPlayed");
    bool tempTurn=whichTurn; // Variable temporaire qui va ensuite changer le tour
    if (whichTurn==true)
    {
        qWarning("checkLastPlayed, le Joueur1 se prend :");
        if (LastPlayed.IsA2)
        {
            qWarning("Un +2");
            for (int i=0; i<2; i++)
            {
                Joueur1.push_back(Deck[whereDeck]);
                whereDeck--;
                ui->horizontalJoueur1->addWidget(Joueur1[Joueur1.length()-1].label);
            }
            tempTurn=false; // En 1v1, si on se prend +2, on perd son tour

        }
        if (LastPlayed.IsA4)
        {
            qWarning("Un +4");
            for (int i=0; i<4; i++)
            {
                playEffect("gasp");
                Joueur1.push_back(Deck[whereDeck]);
                whereDeck--;
                ui->horizontalJoueur1->addWidget(Joueur1[Joueur1.length()-1].label);
            }
            tempTurn=false; // En 1v1, si on se prend un +4, on perd son tour

        }
        if (LastPlayed.IsAReverse or LastPlayed.IsASkip)
        {
            qWarning("Un skip ou reverse");
            tempTurn=false; // En 1v1, si on se prend un skip ou uun reverse, on perd son tour
        }

    }
    if (whichTurn==false)
    {
        qWarning("checkLastPlayed, le Joueur2 se prend :");
        if (LastPlayed.IsA2==true)
        {
            qWarning("Un +2");
            for (int i=0; i<2; i++)
            {
                Joueur2.push_back(Deck[whereDeck]);
                whereDeck--;
                Joueur2[Joueur2.length()-1].setupImage(true,ui->horizontalLayoutWidget_2->size().width()/7.0,ui->horizontalLayoutWidget_2->size().height());
                ui->horizontalJoueur2->addWidget(Joueur2[Joueur2.length()-1].label);
            }
            tempTurn=true; // En 1v1, si on se prend un +2, on perd son tour


        }
        if (LastPlayed.IsA4==true)
        {
            qWarning("Un +4");
            for (int i=0; i<4; i++)
            {
                playEffect("gasp");
                Joueur2.push_back(Deck[whereDeck]);
                whereDeck--;
                Joueur2[Joueur2.length()-1].setupImage(true,ui->horizontalLayoutWidget_2->size().width()/7.0,ui->horizontalLayoutWidget_2->size().height());
                ui->horizontalJoueur2->addWidget(Joueur2[Joueur2.length()-1].label);
            }
            tempTurn=true; // En 1v1, si on se prend un +4, on perd son tour


        }
        if (LastPlayed.IsAReverse==true or LastPlayed.IsASkip==true)
        {
            qWarning("Un skip ou reverse");
            tempTurn=true; // En 1v1, si on se prend un skip ou un reverse, on perd son tour
        }

    }
    whichTurn=tempTurn;
    qWarning("Sortie checkLastPlayed");
    LastPlayed.IsA2=false;
    LastPlayed.IsA4=false;
    LastPlayed.IsASkip=false;
    LastPlayed.IsAReverse=false;
    LastPlayed.IsAJoker=false;
    // Check la quantité de carte des joueurs.
    if (Joueur1.length()<=1 or Joueur2.length()<=1)
    {
        //if (Joueur2.length()==1) playEffect("uno"); // Le bot dis direct UNO!
        QString temp = backgroundMusicUnoSolo.source().toString(); // file::/Song/putinwalk
        if (temp=="file::/Song/daddydaddydo") setBackgroundMusic("millionnaireHeartbeat");
        qWarning ("ONE CARD LEFT");
        // Faire le bouton Uno

    }

    if (Joueur1.length()>=3 and Joueur2.length()>=3)
    {
        QString temp = backgroundMusicUnoSolo.source().toString(); // file::/Song/millionnaireHeartbeat
        if (temp=="file::/Song/millionnaireHeartbeat") setBackgroundMusic("daddydaddydo");
    }

    if (Joueur1.length()==0)
    {
        whoWined=true;
        victoire();
    }

    if (Joueur2.length()==0)
    {
        whoWined=false;
        victoire();
    }
}

void Uno::victoire()
{
    //audio->stop();
    backgroundMusicUnoSolo.stop();
    ui->horizontalLayoutWidget->setVisible(false);
    ui->horizontalLayoutWidget_2->setVisible(false);
    ui->gridLayoutWidget->setVisible(false);
    ui->gridLayoutWidget_2->setVisible(false);
    ui->pushInfo->setVisible(false);
    ui->pushRecommencer->setVisible(true);
    ui->pushQuitter->setVisible(true);
    ui->label_Fin->setVisible(true);
    ui->pushQuitterOriginal->setVisible(false);
    ui->label_Musique->setVisible(false);
    ui->slideMusic->setVisible(false);
    if (whoWined==true)
    {
        ui->label_Fin->setText("VICTOIRE, tu es si fort au jeu vidéo");
        playEffect("epicwin");
    }
    else
    {
        ui->label_Fin->setText("DEFAITE, tu es si mauvais, abandonne");
        playEffect("sadviolin");
    }
}

void Uno::goBotgo() // Simple procedure pour le bot, si il peux poser une carte, il le fait
{
    if (whereDeck==1)
    {
        while (whereDefausse>1) // On reverifie le deck
        {
            Deck[whereDeck]=Defausse[whereDefausse];
            Deck[whereDeck].setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
            whereDeck++;
            whereDefausse--;
        }
    }

    bool aJoue=false;
    for (int i=0;i<Joueur2.length();i++)
    {
        if (aJoue==false and respectDesRegles(i)) // Si il n'a pas encore joué et qu'il respecte les règles
        {
            playEffect("place");
            aJoue=true;
            Defausse[whereDefausse]=Joueur2[i];
            whereDefausse++;
            LastPlayed=Joueur2[i];
            LastPlayed.setupImage(false,ui->label_LastPlayed->width(),ui->label_LastPlayed->height());
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
            delete Joueur2[i].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
            Joueur2.remove(i);
            if (LastPlayed.couleur==5) // Si la carte qu'il a mis est noire, on genere une couleur de facon aléatoire
            {
                LastPlayed.couleur= (rand()%4)+1;
            }
            qWarning ("Une carte est joué par Joueur2");
        }
    }
    if (aJoue==false) // Il pioche une carte si il ne peux pas jouer
    {
        playEffect("draw");
        qWarning("Une carte est pioché par Joueur2");
        Joueur2.push_back(Deck[whereDeck]);
        Joueur2.last().setupImage(true,ui->horizontalLayoutWidget_2->size().width()/7.0,ui->horizontalLayoutWidget_2->size().height());
        ui->horizontalJoueur2->addWidget(Joueur2.last().label);
        whereDeck--;       
    }
}


void Uno::on_pushRouge_clicked()
{
    qWarning("Joueur 1 a choisi rouge");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=1;
    refreshInfo();
}

void Uno::on_pushJaune_clicked()
{
    qWarning("Joueur 1 a choisi jaune");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=2;
    refreshInfo();
}

void Uno::on_pushBleu_clicked()
{
    qWarning("Joueur 1 a choisi bleu");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=3;
    refreshInfo();
}

void Uno::on_pushVert_clicked()
{
    qWarning("Joueur 1 a choisi vert");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=4;
    refreshInfo();
}

void Uno::on_pushInfo_clicked() // On autorise le bot a jouer, refresh du bouton
{
    checkLastPlayed();
    goBotgo();
    qWarning("Le bot a fini de jouer");
    ui->pushInfo->setEnabled(false);
    ui->pushInfo->setText("C'est ton tour");
    whichTurn=true; // Le joueur1 va pouvoir jouer
    checkLastPlayed();
    refreshInfo();
}


void Uno::on_pushRecommencer_clicked()
{
    ui->horizontalLayoutWidget->setVisible(true);
    ui->horizontalLayoutWidget_2->setVisible(true);
    ui->gridLayoutWidget->setVisible(true);
    ui->gridLayoutWidget_2->setVisible(true);
    ui->pushInfo->setVisible(true);
    ui->pushQuitter->setVisible(false);
    ui->pushRecommencer->setVisible(false);
    ui->label_Fin->setVisible(false);
    ui->pushQuitterOriginal->setVisible(true);
    ui->slideMusic->setVisible(true);
    ui->label_Musique->setVisible(true);
    whereDefausse=0;
    for (int i=0;i<108;i++)
    {
        Deck[i].IsA2=false;
        Deck[i].IsA4=false;
        Deck[i].IsASkip=false;
        Deck[i].IsAJoker=false;
        Deck[i].IsAReverse=false;
    }
    Shuffle();
    setScreen();
    refreshInfo();
    whichTurn=true;
    ui->pushInfo->setEnabled(false);
    ui->pushInfo->setText("C'est ton tour");
    /*  playlist->setCurrentIndex(1);
    audio->setVolume(50);
    audio->play();  */
    setBackgroundMusic("daddydaddydo");
}

void Uno::on_pushQuitter_clicked()
{
    qDebug() << "on_pushQuitter_clicked";
    //audio->stop();
    //disconnect(audio, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    backgroundMusicUnoSolo.stop();
    effetUnoSolo.stop();
    close();
    emit gettingClosed(1);
}

void Uno::on_pushQuitterOriginal_clicked()
{
    qDebug() << "on_pushQuitterOriginal_clicked";
    backgroundMusicUnoSolo.stop();
    effetUnoSolo.stop();
    close();
    emit gettingClosed(1);
}

void Uno::on_slideMusic_valueChanged(int value)
{
    float dab = float(value)/float(100);
    backgroundMusicUnoSolo.setVolume(dab);
    effetUnoSolo.setVolume(dab);
    ui->label_Musique->setText("Volume : "+QString::number(dab));
}
