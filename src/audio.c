#include "audio.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
/* Prevent include reordering */
#include <mmsystem.h>

void audio_play_loop(const char *filename) {
  char command[256];
  sprintf(command, "open \"%s\" type mpegvideo alias bgm", filename);
  mciSendString(command, NULL, 0, NULL);
  mciSendString("play bgm repeat", NULL, 0, NULL);
}

void audio_play_effect(SoundEffect effect) {
  switch (effect) {
  case SFX_CLICK:
    PlaySound("CCSelect", NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
    break;
  case SFX_ERROR:
    PlaySound("SystemHand", NULL, SND_ALIAS | SND_ASYNC);
    break;
  case SFX_VICTORY:
    PlaySound("SystemAsterisk", NULL, SND_ALIAS | SND_ASYNC);
    break;
  case SFX_GAMEOVER:
    PlaySound("SystemExclamation", NULL, SND_ALIAS | SND_ASYNC);
    break;
  }
}

void audio_stop() { mciSendString("close bgm", NULL, 0, NULL); }
#else
void audio_play_loop(const char *filename) {
  printf("Audio playback not supported on this platform.\n");
}
void audio_play_effect(SoundEffect effect) { (void)effect; }
void audio_stop() {}
#endif
