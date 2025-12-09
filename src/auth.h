#ifndef AUTH_H
#define AUTH_H

#include "models.h"
#include <stdbool.h>


void auth_initialiser();
Utilisateur *auth_connexion(const char *nom_utilisateur,
                            const char *mot_de_passe);
bool auth_inscription(const char *nom_utilisateur, const char *mot_de_passe);
Utilisateur *auth_obtenir_utilisateur_courant();
void auth_deconnexion();
Utilisateur *auth_obtenir_tous_utilisateurs(int *nombre);

#endif
