#ifndef UNOCARD_H
#define UNOCARD_H
#include "QLabel"

class unocard
{
public:
    unocard();
    int nombre; // +2 en 10, reverse en 11, skip en 12, +4 en 13, joker en 14
    int couleur; // rouge, jaune, bleu, vert, NOIR

    bool IsAReverse =false;
    bool IsASkip=false;
    bool IsA2=false;
    bool IsA4=false;
    bool IsAJoker=false;

    QString nomCouleur;
    QString cheminImage; // Va etre utile pour afficher l'image, couleur et nom
    QPixmap pixmap; // Garde en memoire le chemin vers image en Pixmap [on peux setPixmap() sur un label]
    QLabel * label; // Va charger l'image grace a cheminImage, utilise le .qrc

    void setupImage(bool,int,int); // Si true, met la carte de dos

};

#endif // UNOCARD_H
