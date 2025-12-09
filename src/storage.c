#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FICHIER_UTILISATEURS "utilisateurs.dat"

// --- Stockage Utilisateur ---

void sauvegarder_utilisateurs(Utilisateur utilisateurs[], int nombre) {
  FILE *f = fopen(FICHIER_UTILISATEURS, "wb");
  if (f) {
    fwrite(&nombre, sizeof(int), 1, f);
    fwrite(utilisateurs, sizeof(Utilisateur), nombre, f);
    fclose(f);
  }
}

int charger_utilisateurs(Utilisateur utilisateurs[]) {
  FILE *f = fopen(FICHIER_UTILISATEURS, "rb");
  if (!f)
    return 0;

  int nombre = 0;
  fread(&nombre, sizeof(int), 1, f);
  if (nombre > MAX_UTILISATEURS)
    nombre = MAX_UTILISATEURS;

  fread(utilisateurs, sizeof(Utilisateur), nombre, f);
  fclose(f);
  return nombre;
}

// --- Stockage Jeu ---

void sauvegarder_jeu(EtatJeu *jeu) {
  char nom_fichier[64];
  sprintf(nom_fichier, "jeu_%s.dat", jeu->nom_utilisateur);

  FILE *f = fopen(nom_fichier, "wb");
  if (f) {
    fwrite(jeu, sizeof(EtatJeu), 1, f);
    fclose(f);
  }
}

bool charger_jeu(const char *nom_utilisateur, EtatJeu *jeu) {
  char nom_fichier[64];
  sprintf(nom_fichier, "jeu_%s.dat", nom_utilisateur);

  FILE *f = fopen(nom_fichier, "rb");
  if (!f)
    return false;

  fread(jeu, sizeof(EtatJeu), 1, f);
  fclose(f);
  return true;
}
