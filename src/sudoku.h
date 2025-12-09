#ifndef SUDOKU_H
#define SUDOKU_H

#include "models.h"

void jeu_initialiser_logique();
void jeu_generer(EtatJeu *jeu, Difficulte diff);
bool jeu_verifier_coup(EtatJeu *jeu, int ligne, int col, int num);
bool jeu_est_termine(EtatJeu *jeu);

#endif
