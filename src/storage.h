#ifndef STOCKAGE_H
#define STOCKAGE_H

#include "models.h"

void sauvegarder_utilisateurs(Utilisateur utilisateurs[], int nombre);
int charger_utilisateurs(Utilisateur utilisateurs[]);
void sauvegarder_jeu(EtatJeu *jeu);
bool charger_jeu(const char *nom_utilisateur, EtatJeu *jeu);

#endif
