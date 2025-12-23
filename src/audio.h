#ifndef AUDIO_H
#define AUDIO_H

typedef enum { SFX_CLICK, SFX_ERROR, SFX_VICTORY, SFX_GAMEOVER } SoundEffect;

void audio_play_loop(const char *filename);
void audio_play_effect(SoundEffect effect);
void audio_stop();

#endif
