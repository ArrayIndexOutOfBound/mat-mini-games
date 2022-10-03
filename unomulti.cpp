#include "unomulti.h"
#include "ui_unomulti.h"

QSoundEffect effetUnoMulti;
QSoundEffect backgroundMusicUnoMulti;


UnoMulti::UnoMulti(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnoMulti)
{
    ui->setupUi(this);

    setScreen(); // Initialise tout ce qui est en rapport avec le visuel, EXTENSION CAR CONNECTION
    //ui->pushInfo->setVisible(false);
    //ui->label_Deck->setVisible(false); Ca lui ferait peter un plomb
    //ui->label_CompteurDeck->setVisible(false);    Ils n'existent plus dans cette temporalité
    //ui->label_CompteurDefausse->setVisible(false);
    //ui->label_LastPlayed->setVisible(false); Ca lui ferait peter un plomb
    Shuffle(); // Initialise le deck, mais organisé pour garder une copies des cartes
    // setBackgroundMusic("putinwalk"); //Initialise la playlist et lance le 1er son
    // setTimer();
    effetUnoMulti.setLoopCount(1);
    backgroundMusicUnoMulti.setLoopCount(QSoundEffect::Infinite);
    sock = new QTcpSocket(this);
    sock->setSocketOption(QAbstractSocket::LowDelayOption,1);

    connect(sock, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SLOT(onSocketError(QAbstractSocket::SocketError)));

}

UnoMulti::~UnoMulti()
{
    backgroundMusicUnoMulti.stop();
    effetUnoMulti.stop();
    qDebug() << "Deconstructeur de UNO_MULTI";
    delete ui;
    emit gettingClosed(2);
}

void UnoMulti::onConnected()
{
    sock->write("iii"); // Faire un truc pour connaitre son ID/Numéro Joueur
    sock->flush();
    sock->flush();
    sock->flush();
    sock->waitForBytesWritten();

    //ui->pushInfo->setVisible(true);
    ui->label_CarteMain1->setVisible(true);
    ui->label_CarteMain2->setVisible(true);
    ui->label_NumberLast->setVisible(true);
    ui->label_CouleurLast->setVisible(true);
    //ui->label_CompteurDeck->setVisible(true);     Ils n'existent plus dans cette temporalité
    //ui->label_CompteurDefausse->setVisible(true);
    ui->label_whichTurn->setVisible(true);
    ui->labelHelpIp->setVisible(false);
    ui->lineIP->setVisible(false);
    ui->linePORT->setVisible(false);
    ui->pushConnecter->setVisible(false);
    setBackgroundMusic("putinwalk");

    sock->write("Je viens de me connecter");
    sock->flush();
    sock->waitForBytesWritten();
}

void UnoMulti::onReadyRead()
{
    // EN GROS IL VA LIRE AUSSI CE QUE LE SERV ENVOIE
    QByteArray qba = sock->readAll();
    QString reception = qba.toStdString().c_str();

    qDebug() << "reception : " << reception;
    if(reception[0].toLatin1()=='S' and reception[1].toLatin1()==':') // Cas spécial : réception depuis la commande line du serveur, pour débug
    {
        qDebug() << reception.mid(2,reception.length());
        return; // On exit, pas besoin du reste
    }


    QString info1 = reception.mid(0,1);
    QString info2 = reception.mid(1,1);
    QString info3;
    if (reception.length()==3) info3 = reception.mid(2,1);
    else info3=reception.mid(2,2);
    /*
        Structure Deck :
        Rouge = 0-9,+2,reverse,skip ; 0-9,+2,reverse,skip
        Jaune, Bleu, Vert, 4*(+4), 4*Joker
    */
    qDebug() << "info1 : " << info1;
    qDebug() << "info2 : " << info2;
    qDebug() << "info3 : " << info3;
    // Info 1 : 1 = définition joueur 1 ; 2 = définition joueur 2 ; 5 = je rejoue (+2,+4)
    // a = adversaire pioche ; b = adversaire a joué ; c = encore mon tour ; d = pioche carte soit meme
    // Info 2 : 50 = encore mon tour ; 51 = encore tour adversaire ; d[1,2,3,4,5] = couleur carte pioché
    // Info 3 : d[1,2,3,4,5][IdCarte] = carte que l'on pioche

    if (info1=="5") // Suite à un +2 ou +4
    {
        if (whoAmI==1)
        {
            if (info2=="0") whichTurn=true;
            else whichTurn=false;
        }
        else
        {
            if (info2=="1") whichTurn=true;
            else whichTurn=false;
        }
        refreshInfo();
        return;
    }

    if (info1=="1" or info1=="2") // Définition du joueur 1 ou deux
    {
        if (info1=="1")
        {
            whoAmI=1;
            sock->write("Je suis le joueur 1");
            sock->waitForBytesWritten();
            if (info2=="0") whichTurn=true;
            else whichTurn=false;
        }
        if (info1=="2")
        {
            whoAmI=2;
            sock->write("Je suis le joueur 2");
            sock->waitForBytesWritten();
            if (info2=="1") whichTurn=true;
            else whichTurn=false;
        }
        refreshInfo();
        return;
    }

    if (info1=="a") // Adverse a pioché
    {
        Joueur2.push_back(Deck[0]);
        Joueur2.last().label = new QLabel;
        Joueur2.last().setupImage(true,ui->horizontalLayoutWidget_2->size().width()/7.0,ui->horizontalLayoutWidget_2->size().height());
        ui->horizontalJoueur2->addWidget(Joueur2.last().label);
        whichTurn=true; // C'est ton tour du coup
        if (Joueur1.length()==7 and Joueur2.length()==7) debutGame=true; // Hisoire de bien faire victoire()
        refreshInfo();

        return;
    }

    if(info1=="b") // Adversaire a joué
    {
        delete Joueur2[0].label; // Supprime bien le label a la place de removeWidget, a faire avant le remove
        Joueur2.remove(0);
        whichTurn=true;
        refreshInfo();
        return;
    }

    if(info1=="c")
    {
        whichTurn=true;
        refreshInfo();
        return;
    }

    if (info1=="d") // J'ai demandé une carte a piocher, reception ici
    {
        //playEffect("draw");
        if(info2=="1") // Rouge
        {
            Joueur1.push_back(Deck[info3.toInt()]);
            Joueur1.last().label = new QLabel;
            Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
            ui->horizontalJoueur1->addWidget(Joueur1.last().label);
        }
        if(info2=="2") // Jaune
        {
            Joueur1.push_back(Deck[info3.toInt()+25]);
            Joueur1.last().label = new QLabel;
            Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
            ui->horizontalJoueur1->addWidget(Joueur1.last().label);
        }
        if(info2=="3") // Bleu
        {
            Joueur1.push_back(Deck[info3.toInt()+50]);
            Joueur1.last().label = new QLabel;
            Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
            ui->horizontalJoueur1->addWidget(Joueur1.last().label);
        }
        if(info2=="4") // Vert
        {
            Joueur1.push_back(Deck[info3.toInt()+75]);
            Joueur1.last().label = new QLabel;
            Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
            ui->horizontalJoueur1->addWidget(Joueur1.last().label);
        }
        if(info2=="5") // Noir
        {
            if(info3=="13")
            {
                Joueur1.push_back(Deck[102]); // +4
                Joueur1.last().label = new QLabel;
                Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
                ui->horizontalJoueur1->addWidget(Joueur1.last().label);
            }
            if(info3=="14")
            {
                Joueur1.push_back(Deck[106]); // Joker
                Joueur1.last().label = new QLabel;
                Joueur1.last().setupImage(false,ui->horizontalLayoutWidget->size().width()/7.0,ui->horizontalLayoutWidget->size().height());
                ui->horizontalJoueur1->addWidget(Joueur1.last().label);
            }
        }
        if (Joueur1.length()==7 and Joueur2.length()==7) debutGame=true; // Oui
        refreshInfo();
        return;
    }

    if (info1=="z")
    {
        if(info3=="13") // On check d'abord si c'est un +4 ou Joker
        {
            LastPlayed=Deck[102]; // +4
            LastPlayed.couleur=info2.toInt();
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }
        if(info3=="14")
        {
            LastPlayed=Deck[106]; // Joker
            LastPlayed.couleur=info2.toInt();
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }

        if(info2=="1") // Rouge
        {
            LastPlayed=Deck[info3.toInt()];
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }
        if(info2=="2") // Jaune
        {
            LastPlayed=Deck[info3.toInt()+25];
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }
        if(info2=="3") // Bleu
        {
            LastPlayed=Deck[info3.toInt()+50];
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }
        if(info2=="4") // Vert
        {
            LastPlayed=Deck[info3.toInt()+75];
            ui->label_LastPlayed->setScaledContents(true);
            //ui->label_LastPlayed->setFixedSize(176,256);
            ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
        }
        refreshInfo();
        return;
    }
    if (info1=="w") // MAJ de la couleur du LastPlayed en cas de +4 et Joker
    {
        LastPlayed.couleur=info2.toInt();
        refreshInfo();
        return;
    }
}

void UnoMulti::onDisconnected()
{
    backgroundMusicUnoMulti.stop();
    effetUnoMulti.stop();
    qWarning("Pre delete window");
    close();
    qWarning("Post delete window");
}

void UnoMulti::on_pushConnecter_clicked()
{
    sock->connectToHost(ui->lineIP->text(), ui->linePORT->text().toInt());
    //sock->disconnectFromHost();
    //sock->write(ui->leEmission->text().toStdString().c_str());
    qDebug() << ui->lineIP->text() << ui->linePORT->text();
}

void UnoMulti::onSocketError(QAbstractSocket::SocketError error)
{
    switch (error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "Connexion refusée par le serveur !";
        break;
    case QAbstractSocket::NetworkError:
        qDebug() << "Coupure de liaison réseau !";
        break;
    default:
        qDebug() << "Erreur réseau !";
        break;
    } // sw
}

void UnoMulti::Shuffle()
{
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

}

void UnoMulti::setBackgroundMusic(QString nom)
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
    backgroundMusicUnoMulti.stop();
    QString chemin = ":/Song/"+nom;
    backgroundMusicUnoMulti.setSource(QUrl::fromLocalFile(chemin));
    backgroundMusicUnoMulti.play();
}

void UnoMulti::setScreen()
{
    screenUnoMulti = QGuiApplication::primaryScreen();
    //ui->labelLEDEBUG->setText(QString::number(screen->size().width())+QString::number(screen->size().height()));
    resize(screenUnoMulti->availableSize().width(),screenUnoMulti->availableSize().height());

    QPixmap fondecran(QPixmap(":/Uno/Table0")); // Fond Ecran
    fondecran = fondecran.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window ,fondecran);
    //palette.setBrush(QPalette::Background, fondecran);
    this->setPalette(palette); // Fin setEcran

    UnoMulti::setWindowTitle("Uno version multi"); // Titre de la fenetre
    ui->horizontalJoueur1->setAlignment(Qt::AlignLeft); // TOUT GARDER A GAUCHE SOUS PEINE D'EXPLOSION DU SYSTEME
    ui->horizontalJoueur2->setAlignment(Qt::AlignRight); // Same raison
    //ui->pushInfo->setStyleSheet("background-color: rgb(200, 200, 200);border: none;border-radius: 15px;"); // Arrondie les bords des boutons
    ui->pushQuitterOriginal->setStyleSheet("background-color: rgb(200, 200, 200);border: none;border-radius: 15px;");
    QTimer::singleShot(0, this, SLOT(showFullScreen())); // Plein ecran

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
    ui->label_CarteMain1->setVisible(false);
    ui->label_CarteMain2->setVisible(false);
    ui->label_NumberLast->setVisible(false);
    ui->label_CouleurLast->setVisible(false);
    ui->label_whichTurn->setVisible(false);

    // C'est partie pour tout changer en relatif à l'écran
    float w = screenUnoMulti->availableSize().width();
    float h = screenUnoMulti->availableSize().height();

    QString os = QGuiApplication::platformName();

    static unsigned short yo=0;
    if ((os=="android" or os=="ios" or screenUnoMulti->size().height()<1080) and yo==0) // Potentiellement inutile puisque c'est de base petit
    {
        QFont font;
        font.setPointSize(ui->label_CarteMain1->font().pointSize()*(5.0/7.0));
        ui->label_Fin->setFont(font);
        ui->label_CarteMain1->setFont(font);
        ui->label_CarteMain2->setFont(font);
        ui->label_NumberLast->setFont(font);
        ui->label_CouleurLast->setFont(font);
        ui->pushQuitterOriginal->setFont(font);
        ui->pushQuitter->setFont(font);
        ui->pushRecommencer->setFont(font);
        ui->label_whichTurn->setFont(font);
        yo++;
    }

    // Calculs avec comme base la taille 1920*1080, s'adapter aux écrans

    ui->horizontalLayoutWidget->setGeometry(w*(11.0/32.0),h*(79.0/108.0),w*(41.0/64.0),h*(13.0/54.0)); // Joueur 1
    ui->horizontalLayoutWidget_2->setGeometry(w*(1.0/48.0),h*(1.0/27.0),w*(41.0/64.0),h*(13.0/54.0)); // Joueur 2
    ui->gridLayoutWidget->setGeometry(w*(35.0/48.0),h*(11.0/108.0),w*(25.0/96.0),h*(59.0/108.0)); // Grid info
    ui->gridLayoutWidget_2->setGeometry(w*(17.0/48.0),h*(43.0/108.0),w*(53.0/192.0),h*(13.0/54.0)); // Grid Milieu
    ui->gridLayoutWidget_3->setGeometry(w*(1.0/96.0),h*(25.0/54.0),w*(43.0/192.0),h*(25.0/108.0)); // Grid CONNECTION
    ui->gridLayoutWidget_4->setGeometry(w*(13.0/192.0),h*(20.0/27.0),w*(7.0/48.0),h*(5.0/54.0)); // Grid MUSIC
    ui->label_Fin->setGeometry(w*(11.0/48.0),h*(4.0/27.0),w*(101.0/192.0),h*(35.0/108.0));
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
    ui->label_whichTurn->setWordWrap(true);

    ui->label_Deck->setScaledContents(true);
    ui->label_Deck->setPixmap(QPixmap(":/Uno/Back"));
    ui->label_LastPlayed->setScaledContents(true);
}

void UnoMulti::refreshInfo()
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
    //ui->label_CompteurDeck->setText("Il y a "+QString::number(whereDeck)+" cartes dans le deck");     Ils n'existent plus dans cette temporalité
    //ui->label_CompteurDefausse->setText("Il y a "+QString::number(whereDefausse)+" cartes dans la defausse");

    if(whichTurn==true) ui->label_whichTurn->setText("C'est ton tour.");
    else ui->label_whichTurn->setText("C'est le tour de ton adversaire");

    if (Joueur1.length()==0 and debutGame) victoire(true);
    if (Joueur2.length()==0 and debutGame) victoire(false);
}

void UnoMulti::playEffect(QString nom)
{
    effetUnoMulti.stop();
    QString chemin = ":/Effect/"+nom;
    effetUnoMulti.setSource(QUrl::fromLocalFile(chemin));
    effetUnoMulti.setVolume(0.5);
    effetUnoMulti.play();
}

void UnoMulti::mousePressEvent(QMouseEvent * event) // A MODIFIER
{
    if(event->button() == Qt::RightButton)
       {
           //qWarning("CLIC DROIT ENFONCED");
       }
    if(event->button() == Qt::LeftButton)
    {
        QLabel *temp = static_cast<QLabel*>(childAt(event->pos())); // Detecte le Label qui est sur la souris
        child = temp;

        int tailleActuelleCarte = ui->horizontalLayoutWidget->width()/Joueur1.length();
        if (tailleActuelleCarte>ui->horizontalLayoutWidget->width()/7) tailleActuelleCarte=ui->horizontalLayoutWidget->width()/7;

        if (event->x() > ui->horizontalLayoutWidget->x() and event->x() < ui->horizontalLayoutWidget->x()+(tailleActuelleCarte*Joueur1.length()) and whichTurn==true) // CARTES JOUEUR, s'adapte pour ne pas crash l'application.
        {
            if (event->y() > ui->horizontalLayoutWidget->y() and event->y() < ui->horizontalLayoutWidget->y() + ui->horizontalLayoutWidget->height())
            {
                int positionCarteSouris=((event->x()-ui->horizontalLayoutWidget->x()-(Joueur1.length()/tailleActuelleCarte))/tailleActuelleCarte);
                // Obtient la position de la carte entre 0 et length()-1

                if (respectDesRegles(positionCarteSouris)) // Si la carte selectionné respecte les règles
                {
                    playEffect("place");
                    LastPlayed=Joueur1[positionCarteSouris];
                    ui->label_LastPlayed->setPixmap(LastPlayed.pixmap);
                    Joueur1.remove(positionCarteSouris);
                    child->close();
                    QString temp = QString::number(whoAmI)+"b"+QString::number(positionCarteSouris);
                    sock->write(temp.toLatin1());
                    if (LastPlayed.couleur==5)
                    {
                        ui->pushRouge->setVisible(true);
                        ui->pushJaune->setVisible(true);
                        ui->pushBleu->setVisible(true);
                        ui->pushVert->setVisible(true);
                    }
                    // else Attendre la couleur dans les pushs
                    whichTurn=false;
                }
            }
        }

        if (event->x() > screenUnoMulti->size().width()*(17.0/48.0) and event->x() < screenUnoMulti->size().width()*(17.0/48.0)+ui->label_Deck->width() and whichTurn==true) // Position souris = pioche
        {
            if (event->y() > screenUnoMulti->size().height()*(43.0/108.0) and event->y() < screenUnoMulti->size().height()*(43.0/108.0)+ui->label_Deck->height())
            {
                playEffect("draw");
                QString temp = QString::number(whoAmI) + "a";
                sock->write(temp.toLatin1());
                whichTurn=false;
            }
        }

        BoutonSourie = true;
        //qWarning("CLIC GAUCHE ENFONCED");
        //ui->label_2->setText(QString::number(event->position().x()-658-(Joueur1.length()/tailleActuelleCarte)));
        //ui->label_3->setText(QString::number((event->position().x()-658-(Joueur1.length()/tailleActuelleCarte))/tailleActuelleCarte));
        refreshInfo();
    }
}

void UnoMulti::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
       {
           //qWarning("CLIC DROIT RELACHED");
       }
    if(event->button() == Qt::LeftButton)
           {

        BoutonSourie = false;

        //qWarning("CLIC GAUCHE RELACHED");
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
                //child->move(event->position().x()-child->width()/2,event->position().y()-child->height()/2);
            }

    }

}*/

void UnoMulti::on_pushRouge_clicked() // MODIFIER LE COMPORTEMENT DES BOUTONS
{
    qWarning("Joueur 1 a choisi rouge");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=1;
    QString temp = QString::number(whoAmI)+"l1";
    sock->write(temp.toLatin1());
    refreshInfo();
}

void UnoMulti::on_pushJaune_clicked()
{
    qWarning("Joueur 1 a choisi jaune");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=2;
    QString temp = QString::number(whoAmI)+"l2";
    sock->write(temp.toLatin1());
    refreshInfo();
}

void UnoMulti::on_pushBleu_clicked()
{
    qWarning("Joueur 1 a choisi bleu");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=3;
    QString temp = QString::number(whoAmI)+"l3";
    sock->write(temp.toLatin1());
    refreshInfo();
}

void UnoMulti::on_pushVert_clicked()
{
    qWarning("Joueur 1 a choisi vert");
    ui->pushRouge->setVisible(false);
    ui->pushJaune->setVisible(false);
    ui->pushBleu->setVisible(false);
    ui->pushVert->setVisible(false);
    LastPlayed.couleur=4;
    QString temp = QString::number(whoAmI)+"l4";
    sock->write(temp.toLatin1());
    refreshInfo();
}

void UnoMulti::on_pushQuitter_clicked()
{
    //audio->stop();
    //disconnect(audio, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    backgroundMusicUnoMulti.stop();
    effetUnoMulti.stop();
    close();
    emit gettingClosed(2);
}

void UnoMulti::on_pushQuitterOriginal_clicked() // AJOUTER LA DECONNECTION AU SERV POUR LA PROCHAINE FENETRE
{
    backgroundMusicUnoMulti.stop();
    effetUnoMulti.stop();
    close();
    emit gettingClosed(2);
}

void UnoMulti::on_slideMusic_valueChanged(int value)
{
    float dab = float(value)/float(100);
    backgroundMusicUnoMulti.setVolume(dab);
    effetUnoMulti.setVolume(dab);
    ui->labelMusic->setText("Volume : "+QString::number(dab));
}

bool UnoMulti::respectDesRegles(int posCarte) // Si la carte que l'on souhaite poser soit conforme aux règles
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
    return false;
}

void UnoMulti::victoire(bool whoWined)
{
    backgroundMusicUnoMulti.stop();
    ui->horizontalLayoutWidget->setVisible(false);
    ui->horizontalLayoutWidget_2->setVisible(false);
    ui->gridLayoutWidget->setVisible(false);
    ui->gridLayoutWidget_2->setVisible(false);
    ui->pushRecommencer->setVisible(true);
    ui->pushQuitter->setVisible(true);
    ui->label_Fin->setVisible(true);
    ui->pushQuitterOriginal->setVisible(false);
    ui->labelMusic->setVisible(false);
    ui->slideMusic->setVisible(false);

    if (whoWined)
    {
        ui->label_Fin->setWordWrap(true);
        ui->label_Fin->setText("VICTOIRE, tu es si fort au jeu vidéo. \nIl faut quitter cette fenetre et la réouvrir pour relancer une game.");
        playEffect("epicwin");
    }
    else
    {
        ui->label_Fin->setWordWrap(true);
        ui->label_Fin->setText("DEFAITE, tu es si mauvais, abandonne. \nIl faut quitter cette fenetre et la réouvrir pour relancer une game.");
        playEffect("sadviolin");
    }
}
