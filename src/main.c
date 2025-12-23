#include "audio.h"
#include "auth.h"
#include "gui.h"
#include "sudoku.h"

int main(int argc, char **argv) {
  jeu_initialiser_logique();
  auth_initialiser();
  audio_play_loop("music.mp3");
  gui_initialiser(argc, argv);
  audio_stop();
  return 0;
}
