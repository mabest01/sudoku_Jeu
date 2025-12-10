#include "sudoku.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Logique Sudoku (Backtracking) ---

static bool est_sur(int grille[N][N], int ligne, int col, int num) {
  for (int x = 0; x < N; x++)
    if (grille[ligne][x] == num)
      return false;
  for (int x = 0; x < N; x++)
    if (grille[x][col] == num)
      return false;
  int debutLigne = ligne - ligne % 3, debutCol = col - col % 3;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (grille[i + debutLigne][j + debutCol] == num)
        return false;
  return true;
}

static bool resoudre(int grille[N][N]) {
  int ligne, col;
  bool vide = false;
  for (ligne = 0; ligne < N; ligne++) {
    for (col = 0; col < N; col++) {
      if (grille[ligne][col] == 0) {
        vide = true;
        break;
      }
    }
    if (vide)
      break;
  }
  if (!vide)
    return true;

  int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  // Mélanger
  for (int i = 0; i < 9; i++) {
    int r = rand() % 9;
    int t = nums[i];
    nums[i] = nums[r];
    nums[r] = t;
  }

  for (int i = 0; i < 9; i++) {
    if (est_sur(grille, ligne, col, nums[i])) {
      grille[ligne][col] = nums[i];
      if (resoudre(grille))
        return true;
      grille[ligne][col] = 0;
    }
  }
  return false;
}

void jeu_initialiser_logique() { srand(time(NULL)); }

void jeu_generer(EtatJeu *jeu, Difficulte diff) {
  // Effacer
  memset(jeu->grille, 0, sizeof(jeu->grille));
  memset(jeu->solution, 0, sizeof(jeu->solution));
  memset(jeu->notes, 0, sizeof(jeu->notes));

  // Générer grille complète
  // Remplir les boîtes diagonales (indépendantes)
  for (int i = 0; i < N; i = i + 3) {
    int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    // Shuffle
    for (int k = 0; k < 9; k++) {
      int r = rand() % 9;
      int temp = nums[k];
      nums[k] = nums[r];
      nums[r] = temp;
    }
    // Remplir la boîte 3x3
    int idx = 0;
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        jeu->grille[i + r][i + c] = nums[idx++];
      }
    }
  }
  resoudre(jeu->grille);

  // Copier vers solution
  memcpy(jeu->solution, jeu->grille, sizeof(jeu->grille));

  // Retirer des chiffres selon la difficulté
  int remplies;
  switch (diff) {
  case DIFFICULTE_FACILE:
    remplies = 50;
    break;
  case DIFFICULTE_MOYEN:
    remplies = 35;
    break;
  case DIFFICULTE_DIFFICILE:
    remplies = 10;
    break;
  default:
    remplies = 30;
  }

  int a_retirer = 81 - remplies;
  while (a_retirer > 0) {
    int r = rand() % N;
    int c = rand() % N;
    if (jeu->grille[r][c] != 0) {
      jeu->grille[r][c] = 0;
      a_retirer--;
    }
  }

  // Définir état initial
  memcpy(jeu->initial, jeu->grille, sizeof(jeu->grille));

  jeu->difficulte = diff;
  jeu->erreurs_commises = 0;
  jeu->est_termine = false;

  switch (diff) {
  case DIFFICULTE_FACILE:
    jeu->temps_initial = 0; // Infini / Pas de temps
    jeu->temps_restant = 0;
    jeu->vies_restantes = -1; // Infini
    break;
  case DIFFICULTE_MOYEN:
    jeu->temps_initial = 8 * 60;
    jeu->temps_restant = 8 * 60;
    jeu->vies_restantes = 5;
    break;
  case DIFFICULTE_DIFFICILE:
    jeu->temps_initial = 3 * 60;
    jeu->temps_restant = 3 * 60;
    jeu->vies_restantes = 3;
    break;
  }
}

bool jeu_verifier_coup(EtatJeu *jeu, int ligne, int col, int num) {
  if (jeu->solution[ligne][col] == num) {
    jeu->grille[ligne][col] = num;
    return true;
  } else {
    jeu->erreurs_commises++;
    return false;
  }
}

bool jeu_est_termine(EtatJeu *jeu) {
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (jeu->grille[i][j] != jeu->solution[i][j])
        return false;
  return true;
}
