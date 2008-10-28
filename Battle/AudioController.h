#ifndef _AUDIOCONTROLLER_H
#define _AUDIOCONTROLLER_H

#include "SDL/SDL_mixer.h"

#define MUSIC_TITLE 0
#define MUSIC_BATTLE 1

#define SND_SELECT 0
#define SND_PAUSE 1

#define SND_JUMP 10
#define SND_SHOOT 11
#define SND_HIT 12
#define SND_BOUNCE 13

#define SND_YOULOSE 18

#define SND_COUNTDOWN 20
#define SND_GO 21

class AudioController {
public:
	AudioController();
	~AudioController();
	bool open_audio();
	void close_audio();
	void load_files();
	void close_files();

	void play_music(int music);
	void stop_music();
	void pause_music();
	void unpause_music();

	void play(int sound);
private:
	Mix_Music * mus_title;
	Mix_Music * mus_battle;

	Mix_Chunk * select;
	Mix_Chunk * pause;
	
	Mix_Chunk * jump;
	Mix_Chunk * shoot;
	Mix_Chunk * hit;
	Mix_Chunk * bounce;

	Mix_Chunk * youlose;

	Mix_Chunk * countdown;
	Mix_Chunk * go;

	void play_sound(Mix_Chunk * sound);
	void play_sound(Mix_Chunk * sound, int volume);
};

#endif
