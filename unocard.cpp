#include "unocard.h"

unocard::unocard()
{

}

void unocard::setupImage(bool isBack, int largeur, int hauteur)
{
    if (isBack)
    {
        pixmap=QPixmap(":/Uno/Back");
        label->setScaledContents(true);
        label->setPixmap(pixmap);
        label->setFixedSize(largeur, hauteur);
    }
    else
    {
        pixmap=QPixmap(cheminImage);
        label->setScaledContents(true);
        label->setPixmap(pixmap);
        label->setFixedSize(largeur,hauteur);
    }
}
