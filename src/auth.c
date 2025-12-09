#include "auth.h"
#include "sha256.h"
#include "storage.h"
#include <stdio.h>
#include <string.h>


static Utilisateur utilisateurs[MAX_UTILISATEURS];
static int nombre_utilisateurs = 0;
static Utilisateur *utilisateur_courant = NULL;

void auth_initialiser() {
  nombre_utilisateurs = charger_utilisateurs(utilisateurs);
}

static void hacher_mot_de_passe(const char *mot_de_passe, char *sortie) {
  ContexteSHA256 ctx;
  uint8_t hachage[32];
  sha256_initialiser(&ctx);
  sha256_mettre_a_jour(&ctx, (uint8_t *)mot_de_passe, strlen(mot_de_passe));
  sha256_finaliser(&ctx, hachage);

  for (int i = 0; i < 32; i++) {
    sprintf(sortie + (i * 2), "%02x", hachage[i]);
  }
  sortie[64] = 0;
}

Utilisateur *auth_connexion(const char *nom_utilisateur,
                            const char *mot_de_passe) {
  char hachage[65];
  hacher_mot_de_passe(mot_de_passe, hachage);

  for (int i = 0; i < nombre_utilisateurs; i++) {
    if (strcmp(utilisateurs[i].nom_utilisateur, nom_utilisateur) == 0 &&
        strcmp(utilisateurs[i].mot_de_passe_hache, hachage) == 0) {
      utilisateur_courant = &utilisateurs[i];
      return utilisateur_courant;
    }
  }
  return NULL;
}

bool auth_inscription(const char *nom_utilisateur, const char *mot_de_passe) {
  if (nombre_utilisateurs >= MAX_UTILISATEURS)
    return false;

  // Vérifier si existe
  for (int i = 0; i < nombre_utilisateurs; i++) {
    if (strcmp(utilisateurs[i].nom_utilisateur, nom_utilisateur) == 0)
      return false;
  }

  Utilisateur u;
  strcpy(u.nom_utilisateur, nom_utilisateur);
  hacher_mot_de_passe(mot_de_passe, u.mot_de_passe_hache);
  u.meilleur_score_facile = 999999;
  u.meilleur_score_moyen = 999999;
  u.meilleur_score_difficile = 999999;

  utilisateurs[nombre_utilisateurs++] = u;
  sauvegarder_utilisateurs(utilisateurs, nombre_utilisateurs);
  return true;
}

Utilisateur *auth_obtenir_utilisateur_courant() { return utilisateur_courant; }

void auth_deconnexion() { utilisateur_courant = NULL; }

Utilisateur *auth_obtenir_tous_utilisateurs(int *nombre) {
  *nombre = nombre_utilisateurs;
  return utilisateurs;
}
