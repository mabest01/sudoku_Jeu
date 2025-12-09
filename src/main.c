#include "auth.h"
#include "gui.h"
#include "sudoku.h"


int main(int argc, char **argv) {
  jeu_initialiser_logique();
  auth_initialiser();
  gui_initialiser(argc, argv);
  return 0;
}
