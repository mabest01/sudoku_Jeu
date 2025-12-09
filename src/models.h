#ifndef MODELES_H
#define MODELES_H

#include <stdbool.h>

#define MAX_UTILISATEURS 100
#define N 9

typedef enum {
  DIFFICULTE_FACILE = 0,
  DIFFICULTE_MOYEN = 1,
  DIFFICULTE_DIFFICILE = 2
} Difficulte;

typedef struct {
  char nom_utilisateur[32];
  char mot_de_passe_hache[65]; // Chaîne hexadécimale
  int meilleur_score_facile;   // Temps en secondes
  int meilleur_score_moyen;
  int meilleur_score_difficile;
} Utilisateur;

typedef struct {
  char nom_utilisateur[32];
  Difficulte difficulte;
  int grille[N][N];   // État actuel
  int solution[N][N]; // Solution
  int initial[N][N];  // État initial (pour savoir quelles cases sont fixes)
  int notes[N][N];    // Masque de bits pour les notes (1-9)
  int temps_ecoule;   // Secondes
  int erreurs_commises;
  bool est_termine;
} EtatJeu;

#endif
