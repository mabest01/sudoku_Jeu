#include "gui.h"
#include "audio.h"
#include "auth.h"
#include "storage.h"
#include "sudoku.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GtkWidget *pile_principale;
static GtkWidget *fenetre_solution;
static GtkWidget *fenetre;
static GtkCssProvider *fournisseur_css;

// Widgets Connexion
static GtkWidget *entree_connexion_utilisateur;
static GtkWidget *entree_connexion_mdp;
static GtkWidget *label_statut_connexion;

// Widgets Tableau de Bord
static GtkWidget *label_bienvenue;
static GtkWidget *label_highscore_facile;
static GtkWidget *label_highscore_moyen;
static GtkWidget *label_highscore_difficile;

// Widgets Jeu
static GtkWidget *conteneur_grille_jeu;
static GtkWidget *label_chrono;
static GtkWidget *label_erreurs;
static GtkWidget *entrees_jeu[9][9];
static EtatJeu jeu_courant;
static guint id_chrono = 0;

// --- Fonctions Auxiliaires ---

static void charger_css() {
  fournisseur_css = gtk_css_provider_new();
  gtk_css_provider_load_from_path(fournisseur_css, "style.css", NULL);
  gtk_style_context_add_provider_for_screen(
      gdk_screen_get_default(), GTK_STYLE_PROVIDER(fournisseur_css),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void basculer_vers_page(const char *nom_page) {
  gtk_stack_set_visible_child_name(GTK_STACK(pile_principale), nom_page);
}

static void rafraichir_tableau_de_bord() {
  Utilisateur *u = auth_obtenir_utilisateur_courant();
  if (u) {
    char buf[64];
    sprintf(buf, "🟢 Facile : %d pts", u->meilleur_score_facile);
    gtk_label_set_text(GTK_LABEL(label_highscore_facile), buf);
    sprintf(buf, "🟡 Moyen : %d pts", u->meilleur_score_moyen);
    gtk_label_set_text(GTK_LABEL(label_highscore_moyen), buf);
    sprintf(buf, "🔴 Difficile : %d pts", u->meilleur_score_difficile);
    gtk_label_set_text(GTK_LABEL(label_highscore_difficile), buf);
  }
}

// --- Écran de Connexion ---

static void sur_clic_connexion(GtkWidget *widget, gpointer data) {
  const char *user =
      gtk_entry_get_text(GTK_ENTRY(entree_connexion_utilisateur));
  const char *pass = gtk_entry_get_text(GTK_ENTRY(entree_connexion_mdp));

  if (strlen(user) < 3 || strlen(pass) < 3) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(label_statut_connexion);
    gtk_style_context_add_class(ctx, "label-erreur");
    gtk_style_context_remove_class(ctx, "label-succes");
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "⚠️ Nom/Mdp trop court (min 3)");
    return;
  }

  if (auth_connexion(user, pass)) {
    // Mise à jour Tableau de Bord
    char bienvenue[64];
    sprintf(bienvenue, "👋 Bienvenue, %s !", user);
    gtk_label_set_text(GTK_LABEL(label_bienvenue), bienvenue);

    // Mise à jour Highscores
    rafraichir_tableau_de_bord();

    GtkStyleContext *ctx = gtk_widget_get_style_context(label_statut_connexion);
    gtk_style_context_remove_class(ctx, "label-erreur");
    gtk_style_context_add_class(ctx, "label-succes");
    gtk_label_set_text(GTK_LABEL(label_statut_connexion), "");

    basculer_vers_page("tableau_de_bord");
  } else {
    GtkStyleContext *ctx = gtk_widget_get_style_context(label_statut_connexion);
    gtk_style_context_add_class(ctx, "label-erreur");
    gtk_style_context_remove_class(ctx, "label-succes");
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "⚠️ Identifiants incorrects");
  }
}

static void sur_clic_inscription(GtkWidget *widget, gpointer data) {
  const char *user =
      gtk_entry_get_text(GTK_ENTRY(entree_connexion_utilisateur));
  const char *pass = gtk_entry_get_text(GTK_ENTRY(entree_connexion_mdp));

  if (auth_inscription(user, pass)) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(label_statut_connexion);
    gtk_style_context_add_class(ctx, "label-succes");
    gtk_style_context_remove_class(ctx, "label-erreur");
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "✅ Compte créé ! Veuillez vous connecter.");
  } else {
    GtkStyleContext *ctx = gtk_widget_get_style_context(label_statut_connexion);
    gtk_style_context_add_class(ctx, "label-erreur");
    gtk_style_context_remove_class(ctx, "label-succes");
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "⚠️ Ce nom d'utilisateur existe déjà");
  }
}

static GtkWidget *creer_page_connexion() {
  GtkWidget *boite = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_widget_set_halign(boite, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(boite, GTK_ALIGN_CENTER);

  GtkWidget *titre = gtk_label_new("👑 Sudoku EILCO 👑");
  gtk_style_context_add_class(gtk_widget_get_style_context(titre), "titre");
  gtk_box_pack_start(GTK_BOX(boite), titre, FALSE, FALSE, 20);

  entree_connexion_utilisateur = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entree_connexion_utilisateur),
                                 "👤 Nom d'utilisateur");
  gtk_box_pack_start(GTK_BOX(boite), entree_connexion_utilisateur, FALSE, FALSE,
                     0);

  entree_connexion_mdp = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entree_connexion_mdp),
                                 "🔒 Mot de passe");
  gtk_entry_set_visibility(GTK_ENTRY(entree_connexion_mdp), FALSE);
  gtk_box_pack_start(GTK_BOX(boite), entree_connexion_mdp, FALSE, FALSE, 0);

  GtkWidget *boite_boutons = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(boite_boutons),
                            GTK_BUTTONBOX_CENTER);

  GtkWidget *btn_connexion = gtk_button_new_with_label("🔑 Connexion");
  g_signal_connect(btn_connexion, "clicked", G_CALLBACK(sur_clic_connexion),
                   NULL);
  gtk_container_add(GTK_CONTAINER(boite_boutons), btn_connexion);

  GtkWidget *btn_inscription = gtk_button_new_with_label("📝 Inscription");
  g_signal_connect(btn_inscription, "clicked", G_CALLBACK(sur_clic_inscription),
                   NULL);
  gtk_container_add(GTK_CONTAINER(boite_boutons), btn_inscription);

  gtk_box_pack_start(GTK_BOX(boite), boite_boutons, FALSE, FALSE, 10);

  label_statut_connexion = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(boite), label_statut_connexion, FALSE, FALSE, 0);

  return boite;
}

// --- Écran Tableau de Bord ---

static gboolean sur_tick_chrono(gpointer data) {
  if (jeu_courant.temps_initial == 0) {
    // Mode Facile : Pas de limite de temps
    gtk_label_set_text(GTK_LABEL(label_chrono), "⏱️ Temps : ∞");
    return TRUE;
  }

  if (jeu_courant.temps_restant > 0) {
    jeu_courant.temps_restant--;
    char buf[32];
    sprintf(buf, "⏱️ Temps : %02d:%02d", jeu_courant.temps_restant / 60,
            jeu_courant.temps_restant % 60);
    gtk_label_set_text(GTK_LABEL(label_chrono), buf);
  } else {
    // Temps écoulé -> Game Over
    gtk_label_set_text(GTK_LABEL(label_erreurs), "💀 TEMPS ÉCOULÉ !");
    gtk_style_context_add_class(gtk_widget_get_style_context(label_erreurs),
                                "label-erreur");
    // Désactiver la grille
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        gtk_widget_set_sensitive(GTK_WIDGET(entrees_jeu[i][j]), FALSE);
    return FALSE; // Arrêter le timer
  }
  return TRUE;
}

static void mettre_a_jour_vies() {
  char buf[64];
  if (jeu_courant.vies_restantes == -1) {
    sprintf(buf, "❤️ Vies : ∞");
  } else {
    strcpy(buf, "❤️ Vies : ");
    for (int i = 0; i < jeu_courant.vies_restantes; i++) {
      strcat(buf, "❤️");
    }
  }
  gtk_label_set_text(GTK_LABEL(label_erreurs), buf);
}

static void sur_demarrage_nouvelle_partie(GtkWidget *widget, gpointer data) {
  int diff = GPOINTER_TO_INT(data);
  jeu_generer(&jeu_courant, (Difficulte)diff);
  strcpy(jeu_courant.nom_utilisateur,
         auth_obtenir_utilisateur_courant()->nom_utilisateur);

  // Rafraîchir l'interface de la grille
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      GtkEntry *entree = GTK_ENTRY(entrees_jeu[i][j]);
      GtkStyleContext *ctx = gtk_widget_get_style_context(GTK_WIDGET(entree));

      gtk_style_context_remove_class(ctx, "case-fixe");
      gtk_style_context_remove_class(ctx, "case-utilisateur");
      gtk_style_context_remove_class(ctx, "correct");
      gtk_style_context_remove_class(ctx, "incorrect");

      if (jeu_courant.initial[i][j] != 0) {
        char buf[2];
        sprintf(buf, "%d", jeu_courant.initial[i][j]);
        gtk_entry_set_text(entree, buf);
        gtk_widget_set_sensitive(GTK_WIDGET(entree), FALSE);
        gtk_style_context_add_class(ctx, "case-fixe");
      } else {
        gtk_entry_set_text(entree, "");
        gtk_widget_set_sensitive(GTK_WIDGET(entree), TRUE);
        gtk_style_context_add_class(ctx, "case-utilisateur");
      }
    }
  }

  // Réinitialiser UI
  gtk_label_set_text(GTK_LABEL(label_erreurs), "");
  gtk_style_context_remove_class(gtk_widget_get_style_context(label_erreurs),
                                 "label-succes");
  gtk_style_context_remove_class(gtk_widget_get_style_context(label_erreurs),
                                 "label-erreur");
  mettre_a_jour_vies();

  // Réinitialiser le chrono
  if (id_chrono > 0)
    g_source_remove(id_chrono);

  // Callback simple pour le chrono
  id_chrono = g_timeout_add_seconds(1, sur_tick_chrono, NULL);
  // Appel immédiat pour afficher le temps initial
  sur_tick_chrono(NULL);

  basculer_vers_page("jeu");
}

static void sur_clic_charger_partie(GtkWidget *widget, gpointer data) {
  Utilisateur *u = auth_obtenir_utilisateur_courant();
  if (charger_jeu(u->nom_utilisateur, &jeu_courant)) {
    // On relance comme une nouvelle partie mais on écrase la grille
    // Attention : il faut préserver le temps restant et les vies du chargement
    // jeu_generer est appelé dans sur_demarrage... donc on ne peut pas
    // l'utiliser directement On va copier manuellement la logique d'init UI

    // Init UI Grille
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        GtkEntry *entree = GTK_ENTRY(entrees_jeu[i][j]);
        GtkStyleContext *ctx = gtk_widget_get_style_context(GTK_WIDGET(entree));

        gtk_style_context_remove_class(ctx, "case-fixe");
        gtk_style_context_remove_class(ctx, "case-utilisateur");
        gtk_style_context_remove_class(ctx, "correct");
        gtk_style_context_remove_class(ctx, "incorrect");

        if (jeu_courant.initial[i][j] != 0) {
          char buf[2];
          sprintf(buf, "%d", jeu_courant.initial[i][j]);
          gtk_entry_set_text(entree, buf);
          gtk_widget_set_sensitive(GTK_WIDGET(entree), FALSE);
          gtk_style_context_add_class(ctx, "case-fixe");
        } else {
          if (jeu_courant.grille[i][j] != 0) {
            char buf[2];
            sprintf(buf, "%d", jeu_courant.grille[i][j]);
            gtk_entry_set_text(entree, buf);
          } else {
            gtk_entry_set_text(entree, "");
          }
          gtk_widget_set_sensitive(GTK_WIDGET(entree), TRUE);
          gtk_style_context_add_class(ctx, "case-utilisateur");
        }
      }
    }

    mettre_a_jour_vies();

    if (id_chrono > 0)
      g_source_remove(id_chrono);
    id_chrono = g_timeout_add_seconds(1, sur_tick_chrono, NULL);
    sur_tick_chrono(NULL);

    basculer_vers_page("jeu");
  }
}

static void sur_clic_deconnexion(GtkWidget *widget, gpointer data) {
  auth_deconnexion();
  basculer_vers_page("connexion");
}

static GtkWidget *creer_page_tableau_de_bord() {
  GtkWidget *boite = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_halign(boite, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(boite, GTK_ALIGN_CENTER);

  label_bienvenue = gtk_label_new("Bienvenue !");
  gtk_style_context_add_class(gtk_widget_get_style_context(label_bienvenue),
                              "titre");
  gtk_box_pack_start(GTK_BOX(boite), label_bienvenue, FALSE, FALSE, 0);

  // Highscores
  GtkWidget *cadre_hs = gtk_frame_new("🏆 Meilleurs Scores (Points)");
  GtkWidget *boite_hs = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(cadre_hs), boite_hs);
  gtk_container_set_border_width(GTK_CONTAINER(boite_hs), 10);

  label_highscore_facile = gtk_label_new("🟢 Facile : -");
  label_highscore_moyen = gtk_label_new("🟡 Moyen : -");
  label_highscore_difficile = gtk_label_new("🔴 Difficile : -");

  gtk_box_pack_start(GTK_BOX(boite_hs), label_highscore_facile, FALSE, FALSE,
                     0);
  gtk_box_pack_start(GTK_BOX(boite_hs), label_highscore_moyen, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(boite_hs), label_highscore_difficile, FALSE, FALSE,
                     0);

  gtk_box_pack_start(GTK_BOX(boite), cadre_hs, FALSE, FALSE, 10);

  // Boutons
  GtkWidget *btn_facile =
      gtk_button_new_with_label("🟢 Facile (∞ Vies, Pas de temps)");
  g_signal_connect(btn_facile, "clicked",
                   G_CALLBACK(sur_demarrage_nouvelle_partie),
                   GINT_TO_POINTER(DIFFICULTE_FACILE));
  gtk_box_pack_start(GTK_BOX(boite), btn_facile, FALSE, FALSE, 5);

  GtkWidget *btn_moyen = gtk_button_new_with_label("🟡 Moyen (5 Vies, 8 min)");
  g_signal_connect(btn_moyen, "clicked",
                   G_CALLBACK(sur_demarrage_nouvelle_partie),
                   GINT_TO_POINTER(DIFFICULTE_MOYEN));
  gtk_box_pack_start(GTK_BOX(boite), btn_moyen, FALSE, FALSE, 5);

  GtkWidget *btn_difficile =
      gtk_button_new_with_label("🔴 Difficile (3 Vies, 3 min)");
  g_signal_connect(btn_difficile, "clicked",
                   G_CALLBACK(sur_demarrage_nouvelle_partie),
                   GINT_TO_POINTER(DIFFICULTE_DIFFICILE));
  gtk_box_pack_start(GTK_BOX(boite), btn_difficile, FALSE, FALSE, 5);

  GtkWidget *btn_charger = gtk_button_new_with_label("💾 Charger une Partie");
  g_signal_connect(btn_charger, "clicked", G_CALLBACK(sur_clic_charger_partie),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite), btn_charger, FALSE, FALSE, 5);

  GtkWidget *btn_deconnexion = gtk_button_new_with_label("🚪 Déconnexion");
  g_signal_connect(btn_deconnexion, "clicked", G_CALLBACK(sur_clic_deconnexion),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite), btn_deconnexion, FALSE, FALSE, 20);

  return boite;
}

// --- Écran de Jeu ---

static void sur_changement_case(GtkEditable *editable, gpointer data) {
  int ligne = -1, col = -1;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (GTK_WIDGET(editable) == entrees_jeu[i][j]) {
        ligne = i;
        col = j;
        break;
      }
    }
  }

  if (ligne == -1)
    return;

  const char *texte = gtk_entry_get_text(GTK_ENTRY(editable));
  if (strlen(texte) == 0)
    return;

  int val = atoi(texte);
  if (val < 1 || val > 9) {
    gtk_entry_set_text(GTK_ENTRY(editable), ""); // Invalide
    return;
  }

  // Vérifier le coup
  GtkStyleContext *ctx = gtk_widget_get_style_context(GTK_WIDGET(editable));
  if (jeu_verifier_coup(&jeu_courant, ligne, col, val)) {
    gtk_style_context_add_class(ctx, "correct");
    gtk_style_context_remove_class(ctx, "incorrect");

    if (jeu_est_termine(&jeu_courant)) {
      // Victoire !
      // Calcul du score : Base (1000) + (TempsRestant * 10) + (ViesRestantes *
      // 500)
      int score = 1000;
      if (jeu_courant.temps_initial > 0) {
        score += jeu_courant.temps_restant * 10;
      }
      if (jeu_courant.vies_restantes > 0) {
        score += jeu_courant.vies_restantes * 500;
      }

      Utilisateur *u = auth_obtenir_utilisateur_courant();
      if (jeu_courant.difficulte == DIFFICULTE_FACILE &&
          score > u->meilleur_score_facile)
        u->meilleur_score_facile = score;
      if (jeu_courant.difficulte == DIFFICULTE_MOYEN &&
          score > u->meilleur_score_moyen)
        u->meilleur_score_moyen = score;
      if (jeu_courant.difficulte == DIFFICULTE_DIFFICILE &&
          score > u->meilleur_score_difficile)
        u->meilleur_score_difficile = score;
      int nb_users;
      Utilisateur *tous_users = auth_obtenir_tous_utilisateurs(&nb_users);
      sauvegarder_utilisateurs(tous_users, nb_users);

      if (id_chrono > 0)
        g_source_remove(id_chrono);

      char msg[128];
      sprintf(msg, "🎉 VICTOIRE ! Score : %d", score);
      gtk_label_set_text(GTK_LABEL(label_erreurs), msg);
      gtk_style_context_add_class(gtk_widget_get_style_context(label_erreurs),
                                  "label-succes");
      audio_play_effect(SFX_VICTORY);
    }
  } else {
    gtk_style_context_add_class(ctx, "incorrect");
    gtk_style_context_remove_class(ctx, "correct");
    audio_play_effect(SFX_ERROR);

    if (jeu_courant.vies_restantes != -1) {
      jeu_courant.vies_restantes--;
      mettre_a_jour_vies();

      if (jeu_courant.vies_restantes <= 0) {
        gtk_label_set_text(GTK_LABEL(label_erreurs),
                           "💀 GAME OVER (Plus de vies)");
        gtk_style_context_add_class(gtk_widget_get_style_context(label_erreurs),
                                    "label-erreur");
        audio_play_effect(SFX_GAMEOVER);
        // Désactiver grille
        for (int i = 0; i < 9; i++)
          for (int j = 0; j < 9; j++)
            gtk_widget_set_sensitive(GTK_WIDGET(entrees_jeu[i][j]), FALSE);
        if (id_chrono > 0)
          g_source_remove(id_chrono);
      }
    }
  }

  // Sauvegarde auto
  sauvegarder_jeu(&jeu_courant);
}

static void afficher_solution_fenetre() {
  if (fenetre_solution == NULL) {
    fenetre_solution = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fenetre_solution), "Solution");
    g_signal_connect(fenetre_solution, "delete-event",
                     G_CALLBACK(gtk_widget_hide_on_delete), NULL);
  }

  GtkWidget *grille_solution = gtk_grid_new();
  gtk_widget_set_halign(grille_solution, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(grille_solution, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_top(grille_solution, 20);
  gtk_widget_set_margin_bottom(grille_solution, 20);
  gtk_widget_set_margin_start(grille_solution, 20);
  gtk_widget_set_margin_end(grille_solution, 20);
  gtk_grid_set_row_spacing(GTK_GRID(grille_solution), 2);
  gtk_grid_set_column_spacing(GTK_GRID(grille_solution), 2);

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      char buf[2];
      sprintf(buf, "%d", jeu_courant.solution[i][j]);
      GtkWidget *label = gtk_label_new(buf);

      GtkStyleContext *ctx = gtk_widget_get_style_context(label);
      gtk_style_context_add_class(ctx, "case-sudoku");

      GtkWidget *frame = gtk_frame_new(NULL);
      gtk_container_add(GTK_CONTAINER(frame), label);
      gtk_widget_set_size_request(frame, 30, 30);

      int haut = (i % 3 == 0 && i != 0) ? 4 : 0;
      int gauche = (j % 3 == 0 && j != 0) ? 4 : 0;
      gtk_widget_set_margin_top(frame, haut);
      gtk_widget_set_margin_start(frame, gauche);

      gtk_grid_attach(GTK_GRID(grille_solution), frame, j, i, 1, 1);
    }
  }

  GList *children = gtk_container_get_children(GTK_CONTAINER(fenetre_solution));
  if (children) {
    gtk_container_remove(GTK_CONTAINER(fenetre_solution),
                         GTK_WIDGET(children->data));
    g_list_free(children);
  }

  gtk_container_add(GTK_CONTAINER(fenetre_solution), grille_solution);
  gtk_widget_show_all(fenetre_solution);
}

static void sur_clic_quitter_jeu(GtkWidget *widget, gpointer data) {
  if (id_chrono > 0)
    g_source_remove(id_chrono);

  sauvegarder_jeu(&jeu_courant);
  rafraichir_tableau_de_bord();
  basculer_vers_page("tableau_de_bord");
}

static void sur_clic_remplir_auto(GtkWidget *widget, gpointer data) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (jeu_courant.initial[i][j] == 0) {
        char buf[2];
        sprintf(buf, "%d", jeu_courant.solution[i][j]);

        // Block signal to prevent spamming checks/saves
        g_signal_handlers_block_by_func(entrees_jeu[i][j],
                                        (gpointer)sur_changement_case, NULL);
        gtk_entry_set_text(GTK_ENTRY(entrees_jeu[i][j]), buf);
        g_signal_handlers_unblock_by_func(entrees_jeu[i][j],
                                          (gpointer)sur_changement_case, NULL);

        // Manually update internal state
        jeu_courant.grille[i][j] = jeu_courant.solution[i][j];

        // Update visual style
        GtkStyleContext *ctx = gtk_widget_get_style_context(entrees_jeu[i][j]);
        gtk_style_context_remove_class(ctx, "incorrect");
        gtk_style_context_add_class(ctx, "correct");
      }
    }
  }
  // Trigger victory check manually
  sur_changement_case(GTK_EDITABLE(entrees_jeu[0][0]), NULL);
}

static void sur_clic_resoudre(GtkWidget *widget, gpointer data) {
  afficher_solution_fenetre();
}

static GtkWidget *creer_page_jeu() {
  GtkWidget *boite = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  // En-tête
  GtkWidget *en_tete = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  label_chrono = gtk_label_new("⏱️ Temps : --:--");
  label_erreurs = gtk_label_new("❤️ Vies : ---");
  gtk_box_pack_start(GTK_BOX(en_tete), label_chrono, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(en_tete), label_erreurs, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(boite), en_tete, FALSE, FALSE, 10);

  // Grille
  GtkWidget *grille = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grille), 2);
  gtk_grid_set_column_spacing(GTK_GRID(grille), 2);
  gtk_widget_set_halign(grille, GTK_ALIGN_CENTER);

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      entrees_jeu[i][j] = gtk_entry_new();
      gtk_entry_set_width_chars(GTK_ENTRY(entrees_jeu[i][j]), 2);
      gtk_entry_set_alignment(GTK_ENTRY(entrees_jeu[i][j]), 0.5);
      gtk_entry_set_max_length(GTK_ENTRY(entrees_jeu[i][j]), 1);

      GtkStyleContext *ctx = gtk_widget_get_style_context(entrees_jeu[i][j]);
      gtk_style_context_add_class(ctx, "case-sudoku");

      // Marges pour les blocs 3x3
      int haut = (i % 3 == 0 && i != 0) ? 4 : 0;
      int gauche = (j % 3 == 0 && j != 0) ? 4 : 0;
      gtk_widget_set_margin_top(entrees_jeu[i][j], haut);
      gtk_widget_set_margin_start(entrees_jeu[i][j], gauche);

      g_signal_connect(entrees_jeu[i][j], "changed",
                       G_CALLBACK(sur_changement_case), NULL);

      gtk_grid_attach(GTK_GRID(grille), entrees_jeu[i][j], j, i, 1, 1);
    }
  }
  gtk_box_pack_start(GTK_BOX(boite), grille, TRUE, TRUE, 0);

  // Pied de page
  GtkWidget *boite_boutons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_widget_set_halign(boite_boutons, GTK_ALIGN_CENTER);

  GtkWidget *btn_resoudre = gtk_button_new_with_label("🧩 Résoudre (Test)");
  g_signal_connect(btn_resoudre, "clicked", G_CALLBACK(sur_clic_resoudre),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite_boutons), btn_resoudre, FALSE, FALSE, 0);

  GtkWidget *btn_remplir = gtk_button_new_with_label("⚡ Remplir");
  g_signal_connect(btn_remplir, "clicked", G_CALLBACK(sur_clic_remplir_auto),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite_boutons), btn_remplir, FALSE, FALSE, 0);

  GtkWidget *btn_quitter =
      gtk_button_new_with_label("💾 Sauvegarder & Quitter");
  g_signal_connect(btn_quitter, "clicked", G_CALLBACK(sur_clic_quitter_jeu),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite_boutons), btn_quitter, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(boite), boite_boutons, FALSE, FALSE, 10);

  return boite;
}

// --- Init Principale ---

void gui_initialiser(int argc, char **argv) {
  gtk_init(&argc, &argv);
  charger_css();

  fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  pile_principale = gtk_stack_new();
  gtk_stack_set_transition_type(GTK_STACK(pile_principale),
                                GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_connexion(),
                      "connexion");
  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_tableau_de_bord(),
                      "tableau_de_bord");
  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_jeu(), "jeu");

  gtk_container_add(GTK_CONTAINER(fenetre), pile_principale);

  g_signal_connect(fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(fenetre);
  gtk_main();
}
