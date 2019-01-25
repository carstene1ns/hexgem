#include "SDL_mixer.h"
#include "sound.h"

static Mix_Chunk 	*mc_swap,*mc_fall,*mc_wrong,
*mc_combo1,*mc_combo2,*mc_combo3,*mc_gameover;
static Mix_Music 	*music1,*music2,*music3;

void init_sound(void) {
	Uint16 audio_format = AUDIO_S16; 
	int audio_channels = 2;
	int ret;

	Mix_Init(MIX_INIT_MP3);
#ifdef __SWITCH__
	ret = Mix_OpenAudioDevice(48000, audio_format, audio_channels, 4096, NULL, 0);
#else
	int audio_rate = 44100;
	int audio_buffers = 1024;
	ret = Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
#endif
	if(ret) {
		printf("Unable to open audio!\n");
		return;
	}
}
void deinit_sound(void) {
	stop_music();
	Mix_CloseAudio();
	Mix_Quit();
}
void load_sound(void) {
#ifdef __SWITCH__
	mc_swap = Mix_LoadWAV("romfs:/swap.wav");
	mc_wrong = Mix_LoadWAV("romfs:/badmove.wav");
	mc_fall = Mix_LoadWAV("romfs:/fall.wav");
	mc_combo1 = Mix_LoadWAV("romfs:/combo1.wav");
	mc_combo2 = Mix_LoadWAV("romfs:/combo2.wav");
	mc_combo3 = Mix_LoadWAV("romfs:/combo3.wav");
	music1 = Mix_LoadMUS("romfs:/Grey_Sector_v0_85.mp3");
	music2 = Mix_LoadMUS("romfs:/Rusted_Soul.mp3");
	music3 = Mix_LoadMUS("romfs:/The_Time_You_Spend_v1_1.mp3");
	mc_gameover = Mix_LoadWAV("romfs:/gameover.wav");
#elif defined __vita__
	mc_swap = Mix_LoadWAV("app0:assets/swap.wav");
	mc_wrong = Mix_LoadWAV("app0:assets/badmove.wav");
	mc_fall = Mix_LoadWAV("app0:assets/fall.wav");
	mc_combo1 = Mix_LoadWAV("app0:assets/combo1.wav");
	mc_combo2 = Mix_LoadWAV("app0:assets/combo2.wav");
	mc_combo3 = Mix_LoadWAV("app0:assets/combo3.wav");
	music1 = Mix_LoadMUS("app0:assets/Grey_Sector_v0_85.mp3");
	music2 = Mix_LoadMUS("app0:assets/Rusted_Soul.mp3");
	music3 = Mix_LoadMUS("app0:assets/The_Time_You_Spend_v1_1.mp3");
	mc_gameover = Mix_LoadWAV("app0:assets/gameover.wav");
#else
	mc_swap = Mix_LoadWAV("assets/swap.wav");
	mc_wrong = Mix_LoadWAV("assets/badmove.wav");
	mc_fall = Mix_LoadWAV("assets/fall.wav");
	mc_combo1 = Mix_LoadWAV("assets/combo1.wav");
	mc_combo2 = Mix_LoadWAV("assets/combo2.wav");
	mc_combo3 = Mix_LoadWAV("assets/combo3.wav");
	music1 = Mix_LoadMUS("assets/Grey_Sector_v0_85.mp3");
	music2 = Mix_LoadMUS("assets/Rusted_Soul.mp3");
	music3 = Mix_LoadMUS("assets/The_Time_You_Spend_v1_1.mp3");
	mc_gameover = Mix_LoadWAV("assets/gameover.wav");
#endif
}
void play_sound(SOUNDID id) {
	switch(id) {
	case SID_SWAP:
		Mix_PlayChannel(-1, mc_swap, 0);
		break;
	case SID_FALL:
		Mix_PlayChannel(-1, mc_fall, 0);
		break;
	case SID_WRONG:
		Mix_PlayChannel(-1, mc_wrong, 0);
		break;
	case SID_COMBO1:
		Mix_PlayChannel(-1, mc_combo1, 0);
		break;
	case SID_COMBO2:
		Mix_PlayChannel(-1, mc_combo2, 0);
		break;
	case SID_COMBO3:
		Mix_PlayChannel(-1, mc_combo3, 0);
		break;
	case SID_GAMEOVER:
		Mix_PlayChannel(-1, mc_gameover, 0);
		break;
		default:
		break;
	}
}
void play_music(SOUNDID id) {
	stop_music();
	switch(id) {
	case SID_MUSIC1:
		Mix_PlayMusic(music1, -1);
		break;
	case SID_MUSIC2:
		Mix_PlayMusic(music2, -1);
		break;
	case SID_MUSIC3:
		Mix_PlayMusic(music3, -1);
		break;
		default:
		break;
	}
}
void fadout_music(int msec) {
	Mix_FadeOutMusic(msec);
}
void stop_music(void) {
	Mix_RewindMusic();
	Mix_HaltMusic();
}
