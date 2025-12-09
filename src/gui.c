#include "gui.h"
#include "auth.h"
#include "storage.h"
#include "sudoku.h"
#include <gtk/gtk.h>
#include <stdio.h>

// Widgets UI Globaux
static GtkWidget *pile_principale;
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

// --- Écran de Connexion ---

static void sur_clic_connexion(GtkWidget *widget, gpointer data) {
  const char *user =
      gtk_entry_get_text(GTK_ENTRY(entree_connexion_utilisateur));
  const char *pass = gtk_entry_get_text(GTK_ENTRY(entree_connexion_mdp));

  if (auth_connexion(user, pass)) {
    // Mise à jour Tableau de Bord
    char bienvenue[64];
    sprintf(bienvenue, "👋 Bienvenue, %s !", user);
    gtk_label_set_text(GTK_LABEL(label_bienvenue), bienvenue);

    // Mise à jour Highscores
    Utilisateur *u = auth_obtenir_utilisateur_courant();
    char buf[64];
    sprintf(buf, "🟢 Facile : %ds",
            u->meilleur_score_facile == 999999 ? 0 : u->meilleur_score_facile);
    gtk_label_set_text(GTK_LABEL(label_highscore_facile), buf);
    sprintf(buf, "🟡 Moyen : %ds",
            u->meilleur_score_moyen == 999999 ? 0 : u->meilleur_score_moyen);
    gtk_label_set_text(GTK_LABEL(label_highscore_moyen), buf);
    sprintf(buf, "🔴 Difficile : %ds",
            u->meilleur_score_difficile == 999999
                ? 0
                : u->meilleur_score_difficile);
    gtk_label_set_text(GTK_LABEL(label_highscore_difficile), buf);

    basculer_vers_page("tableau_de_bord");
  } else {
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "❌ Nom d'utilisateur ou mot de passe invalide");
  }
}

static void sur_clic_inscription(GtkWidget *widget, gpointer data) {
  const char *user =
      gtk_entry_get_text(GTK_ENTRY(entree_connexion_utilisateur));
  const char *pass = gtk_entry_get_text(GTK_ENTRY(entree_connexion_mdp));

  if (strlen(user) < 3 || strlen(pass) < 3) {
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "⚠️ Nom/Mdp trop court");
    return;
  }

  if (auth_inscription(user, pass)) {
    gtk_label_set_text(GTK_LABEL(label_statut_connexion),
                       "✅ Compte créé ! Veuillez vous connecter.");
  } else {
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
  gtk_style_context_add_class(
      gtk_widget_get_style_context(label_statut_connexion), "label-erreur");
  gtk_box_pack_start(GTK_BOX(boite), label_statut_connexion, FALSE, FALSE, 0);

  return boite;
}

// --- Écran Tableau de Bord ---

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

  // Réinitialiser le chrono
  if (id_chrono > 0)
    g_source_remove(id_chrono);

  // Callback simple pour le chrono
  id_chrono = g_timeout_add_seconds(
      1, (GSourceFunc)({
        gboolean cb(gpointer d) {
          jeu_courant.temps_ecoule++;
          char buf[32];
          sprintf(buf, "⏱️ Temps : %02d:%02d", jeu_courant.temps_ecoule / 60,
                  jeu_courant.temps_ecoule % 60);
          gtk_label_set_text(GTK_LABEL(label_chrono), buf);

          return TRUE;
        }
        cb;
      }),
      NULL);

  basculer_vers_page("jeu");
}

static void sur_clic_charger_partie(GtkWidget *widget, gpointer data) {
  Utilisateur *u = auth_obtenir_utilisateur_courant();
  if (charger_jeu(u->nom_utilisateur, &jeu_courant)) {
    sur_demarrage_nouvelle_partie(NULL,
                                  GINT_TO_POINTER(jeu_courant.difficulte));

    // Écraser avec les valeurs sauvegardées
    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        if (jeu_courant.grille[i][j] != 0) {
          char buf[2];
          sprintf(buf, "%d", jeu_courant.grille[i][j]);
          gtk_entry_set_text(GTK_ENTRY(entrees_jeu[i][j]), buf);
        }
      }
    }
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
  GtkWidget *cadre_hs = gtk_frame_new("🏆 Meilleurs Scores");
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
      gtk_button_new_with_label("🟢 Nouvelle Partie (Facile)");
  g_signal_connect(btn_facile, "clicked",
                   G_CALLBACK(sur_demarrage_nouvelle_partie),
                   GINT_TO_POINTER(DIFFICULTE_FACILE));
  gtk_box_pack_start(GTK_BOX(boite), btn_facile, FALSE, FALSE, 5);

  GtkWidget *btn_moyen =
      gtk_button_new_with_label("🟡 Nouvelle Partie (Moyen)");
  g_signal_connect(btn_moyen, "clicked",
                   G_CALLBACK(sur_demarrage_nouvelle_partie),
                   GINT_TO_POINTER(DIFFICULTE_MOYEN));
  gtk_box_pack_start(GTK_BOX(boite), btn_moyen, FALSE, FALSE, 5);

  GtkWidget *btn_difficile =
      gtk_button_new_with_label("🔴 Nouvelle Partie (Difficile)");
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
      Utilisateur *u = auth_obtenir_utilisateur_courant();
      if (jeu_courant.difficulte == DIFFICULTE_FACILE &&
          jeu_courant.temps_ecoule < u->meilleur_score_facile)
        u->meilleur_score_facile = jeu_courant.temps_ecoule;
      if (jeu_courant.difficulte == DIFFICULTE_MOYEN &&
          jeu_courant.temps_ecoule < u->meilleur_score_moyen)
        u->meilleur_score_moyen = jeu_courant.temps_ecoule;
      if (jeu_courant.difficulte == DIFFICULTE_DIFFICILE &&
          jeu_courant.temps_ecoule < u->meilleur_score_difficile)
        u->meilleur_score_difficile = jeu_courant.temps_ecoule;
      sauvegarder_utilisateurs(auth_obtenir_tous_utilisateurs(NULL), 0);

      gtk_label_set_text(GTK_LABEL(label_erreurs), "🎉 VICTOIRE ! 🎉");
      gtk_style_context_add_class(gtk_widget_get_style_context(label_erreurs),
                                  "label-succes");
    }
  } else {
    gtk_style_context_add_class(ctx, "incorrect");
    gtk_style_context_remove_class(ctx, "correct");

    char buf[32];
    sprintf(buf, "❌ Erreurs : %d", jeu_courant.erreurs_commises);
    gtk_label_set_text(GTK_LABEL(label_erreurs), buf);

    // Vérifier condition de défaite
    if (jeu_courant.difficulte == DIFFICULTE_DIFFICILE &&
        jeu_courant.erreurs_commises >= 3) {
      gtk_label_set_text(GTK_LABEL(label_erreurs),
                         "💀 GAME OVER (Trop d'erreurs)");
    }
  }

  // Sauvegarde auto
  sauvegarder_jeu(&jeu_courant);
}

static void sur_clic_quitter_jeu(GtkWidget *widget, gpointer data) {
  if (id_chrono > 0)
    g_source_remove(id_chrono);
  basculer_vers_page("tableau_de_bord");
}

static GtkWidget *creer_page_jeu() {
  GtkWidget *boite = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  // En-tête
  GtkWidget *en_tete = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  label_chrono = gtk_label_new("⏱️ Temps : 00:00");
  label_erreurs = gtk_label_new("❌ Erreurs : 0");
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
  GtkWidget *btn_quitter =
      gtk_button_new_with_label("💾 Sauvegarder & Quitter");
  g_signal_connect(btn_quitter, "clicked", G_CALLBACK(sur_clic_quitter_jeu),
                   NULL);
  gtk_box_pack_start(GTK_BOX(boite), btn_quitter, FALSE, FALSE, 10);

  return boite;
}

// --- Init Principale ---

void gui_initialiser(int argc, char **argv) {
  gtk_init(&argc, &argv);
  charger_css();

  fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(fenetre), "👑 Sudoku EILCO 👑");
  gtk_window_set_default_size(GTK_WINDOW(fenetre), 500, 600);
  g_signal_connect(fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  pile_principale = gtk_stack_new();
  gtk_stack_set_transition_type(GTK_STACK(pile_principale),
                                GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_connexion(),
                      "connexion");
  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_tableau_de_bord(),
                      "tableau_de_bord");
  gtk_stack_add_named(GTK_STACK(pile_principale), creer_page_jeu(), "jeu");

  gtk_container_add(GTK_CONTAINER(fenetre), pile_principale);

  gtk_widget_show_all(fenetre);
  gtk_main();
}
