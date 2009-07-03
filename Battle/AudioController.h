#ifndef _AUDIOCONTROLLER_H
#define _AUDIOCONTROLLER_H

#include "SDL/SDL_mixer.h"

#define MUSICFILES 2

#define MUSIC_TITLE 0
#define MUSIC_BATTLE 1

#define SOUNDFILES 12

#define SND_SELECT 0
#define SND_PAUSE 1
#define SND_SELECT_CHARACTER 2

#define SND_JUMP 3
#define SND_SHOOT 4
#define SND_HIT 5
#define SND_BOUNCE 6
#define SND_EXPLODE 7

#define SND_ITEM 8

#define SND_YOULOSE 9

#define SND_COUNTDOWN 10
#define SND_GO 11

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

	int sound_volume;
	int music_volume;
private:
	Mix_Music * music[MUSICFILES];
	Mix_Chunk * sound[SOUNDFILES];


	static const char * music_files[];
	static const char * sound_files[];
	static const int soundvolume[];
};

#endif
