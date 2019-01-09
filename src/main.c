#include "board.h"
#include "screen.h"
#include "fskip.h"
#include "hiscore.h"
#include "sound.h"

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef __vita__
#include <vitasdk.h>
#endif

 /* Stolen from the mailing list */
       /* Creates a new mouse cursor from an XPM */

       /* XPM */
static const char *arrow[] = {
         /* width height num_colors chars_per_pixel */
	"    32    32        3            1",
         /* colors */
	"X c #000000",
	". c #ffffff",
	"  c None",
         /* pixels */
	"X                               ",
	"XX                              ",
	"X.X                             ",
	"X..X                            ",
	"X...X                           ",
	"X....X                          ",
	"X.....X                         ",
	"X......X                        ",
	"X.......X                       ",
	"X........X                      ",
	"X.....XXXXX                     ",
	"X..X..X                         ",
	"X.X X..X                        ",
	"XX  X..X                        ",
	"X    X..X                       ",
	"     X..X                       ",
	"      X..X                      ",
	"      X..X                      ",
	"       XX                       ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row ) {
		for ( col=0; col<32; ++col ) {
			if ( col % 8 ) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
			case 'X':
				data[i] |= 0x01;
				//k[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

void stoupper(char *s) {
	int i=0;
	while(s[i]!=0) {
		s[i]=toupper(s[i]);
		i++;
	}
}

typedef enum RETCODE {
	RT_VOID=0,
	RT_GAMEOVER,
	RT_CANCEL
}RETCODE;

SDL_Cursor *mouse_curs;

SDL_Joystick *joy;

RETCODE run_game_mainloop(GAMETYPE gametype,int wide) {
	BOARD *board;
	SDL_Event event;
	int ox=0,oy=0;
	int omx=-1,omy=-1,mouse_x,mouse_y,finger_x,finger_y,num_fingers=0;
	SDL_TouchID touchid;

	if (wide) 
		board=create_board(gametype,9+4,7,MAXGEMCOL-1);
	else
		board=create_board(gametype,9,7,MAXGEMCOL-1);

	// TODO: the first run almost never give yellow, this is bad!
	fill_board(board);fill_board(board);
	play_music(SID_MUSIC1);
	while (1){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT: 
				free_board(board);
				return RT_CANCEL;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					free_board(board);
					return RT_CANCEL;
					break;
// 				case SDLK_g:
// 					board->timer=1;
// 					break;
// #ifdef PANDORA
// 				case SDLK_c:
// 					if (mouse_curs==NULL)
// 						mouse_curs=init_system_cursor(arrow);
// 					SDL_SetCursor(mouse_curs);
// 					SDL_ShowCursor(SDL_ENABLE);
// 					break;
// #endif
// 				case SDLK_UP:
// 					if (board->cursy>0)
// 						board->cursy--;
// 					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_x = SCREEN_WIDTH / (float)draw_w * event.button.x;
				mouse_y = SCREEN_HEIGHT / (float)draw_h * event.button.y;
				omx=omy=-1;
				screenspace2gemspace(board,mouse_x,mouse_y,&board->cursx,&board->cursy);
				if (board->cursx !=-1 && board->cursy!=-1) {
					omx=mouse_x;
					omy=mouse_y;
				}
				//printf("Click %d,%d -> %d %d\n",mouse_x,mouse_y,board->cursx,board->cursy);
				break;
			case SDL_MOUSEMOTION:
				if (event.motion.state==SDL_PRESSED && omx!=-1 && omy!=-1) {
					mouse_x = SCREEN_WIDTH / (float)draw_w * event.button.x;
					mouse_y = SCREEN_HEIGHT / (float)draw_h * event.button.y;
					screenspace2gemspace(board,mouse_x,mouse_y,&ox,&oy);
					if (ox!=board->cursx || oy!=board->cursy) {
						//printf("Mouse Motion %d,%d -> %d %d\n",mouse_x,mouse_y,ox,oy);
						if (ox!=-1 && oy!=-1 && board->block_input==0) {
							omx=omy=-1;
							play_sound(SID_SWAP);
							begin_swap(board,ox,oy,board->cursx,board->cursy);
						}
					}
				}
				break;
			case SDL_FINGERDOWN:
				touchid = SDL_GetTouchDevice(0);
				//if (touchid != 0)
					num_fingers = SDL_GetNumTouchFingers(touchid);

				if (num_fingers == 1) {
					finger_x = draw_w * event.tfinger.x;
					finger_y = draw_h * event.tfinger.y;
					omx=omy=-1;
					screenspace2gemspace(board,finger_x,finger_y,&board->cursx,&board->cursy);
					if (board->cursx !=-1 && board->cursy!=-1) {
						omx=finger_x;
						omy=finger_y;
					}
					//printf("Finger Touch %d,%d -> %d %d\n",finger_x,finger_y,board->cursx,board->cursy);
				}
				break;
			case SDL_FINGERMOTION:
				touchid = SDL_GetTouchDevice(0);
				//if (touchid != 0)
					num_fingers = SDL_GetNumTouchFingers(touchid);

				if (num_fingers == 1 && omx!=-1 && omy!=-1) {
					finger_x = draw_w * event.tfinger.x;
					finger_y = draw_h * event.tfinger.y;
					screenspace2gemspace(board,finger_x,finger_y,&ox,&oy);
					if (ox!=board->cursx || oy!=board->cursy) {
						//printf("Finger Motion %d,%d -> %d %d\n",finger_x,finger_y,ox,oy);
						if (ox!=-1 && oy!=-1 && board->block_input==0) {
							omx=omy=-1;
							play_sound(SID_SWAP);
							begin_swap(board,ox,oy,board->cursx,board->cursy);
						}
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				if (event.jbutton.button==11) {
					free_board(board);
					return RT_CANCEL;
				}
				break;
			}
		}

		draw_all(board);
		if (board->combo_cnt>0) {
			board->combo_cnt--;
			if (board->combo_cnt==0) {
				board->combo--;
				if (board->combo==0) board->combo=1;
				else board->combo_cnt=110-(board->combo*10);
				//board->combo=1;
			}
		}
		if (could_this_be_the_end(board)!=0) {
			/* TODO: Save score in hiscore here */
			if (ishiscore(gametype,board->score)) {
#ifdef PANDORA
				char *string=getenv("USER");
				stoupper(string);
#elif __SWITCH__
				// TODO
				char *string="SWITCH PLAYER";
#elif __vita__
				// TODO
				char *string="SWITCH PLAYER";
#else
				char string[32];
				play_music(SID_MUSIC3);
				input_username(string,32);
				printf("String =%s\n",string);
#endif
				addhiscore(gametype,string,board->score);
			}
			free_board(board);
			return RT_GAMEOVER;
		}
		SDL_BlitSurface(buffer,NULL,screen,NULL);
		void* pixels;
		int pitch;
		SDL_RenderClear(renderer);
		SDL_LockTexture(texture,NULL,&pixels,&pitch);
		memcpy(pixels,screen->pixels,screen->pitch*screen->h);
		SDL_UnlockTexture(texture);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		frame_skip();//SDL_Delay(13);
	}

}

void intro_loop(void) {
	SDL_Event event;
	int counter=60*5;
	while (counter>0){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_FINGERDOWN:
			case SDL_JOYBUTTONDOWN:
				return;
				break;
			}
		}
		draw_intro();
		flip_screen();
		counter--;
	}


}

void show_highscore(void) {
	SDL_Event event;
	int counter=60*5;
	int difficulty=0;
	while (difficulty<4){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_FINGERDOWN:
			case SDL_JOYBUTTONDOWN:
				return;
				break;
			}
		}
		draw_scoreboard(difficulty|GM_LINE);
		flip_screen();
		counter--;
		if (counter==0) {
			difficulty++;
			counter=60*5;
		}
	}
}

static SDL_Rect butt_r={112,67,176,35};

int do_button(SDL_Rect *r,char *text,int mx,int my,int button) {
	draw_button(r,text);
	if (mx>r->x && mx<r->x+r->w && my>r->y && my<r->y+r->h && button!=0)
		return 1;
	return 0;
}

int main_menu(void) {
	SDL_Event event;
	int gametype=GM_LINE;
	int menulevel=2;
	int mx=0,my=0,b=0,num_fingers=0;
	int cnt=0;
	SDL_TouchID touchid;

	while (menulevel!=0){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				return -1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mx = SCREEN_WIDTH / (float)draw_w * event.button.x;
				my = SCREEN_HEIGHT / (float)draw_h * event.button.y;
				b=1;
				break;
			case SDL_MOUSEBUTTONUP:
				b=0;
				break;
			case SDL_FINGERDOWN:
				touchid = SDL_GetTouchDevice(0);
				//if (touchid != 0)
					num_fingers = SDL_GetNumTouchFingers(touchid);

				if (num_fingers == 1) {
					mx = draw_w * event.tfinger.x;
					my = draw_h * event.tfinger.y;
					b=1;
				}
				break;
			case SDL_FINGERUP:
				b=0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					return -1;
					break;
				}
				break;
			case SDL_JOYBUTTONDOWN:
				if (event.jbutton.button==11) {
					return -1;
				}
				break;
			}
		}
		draw_main_menu();
		switch (menulevel) {
		case 2:
			//difficulty
			butt_r.y=67;
			if (do_button(&butt_r,"EASY",mx,my,b)) {
				menulevel=0;gametype|=GM_EASY;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"NORMAL",mx,my,b)) {
				menulevel=0;gametype|=GM_NORMAL;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"HARD",mx,my,b)) {
				menulevel=0;gametype|=GM_HARD;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"LEGEND!",mx,my,b)) {
				menulevel=0;gametype|=GM_LEGEND;
			}			
			break;
		case 1:
			// Pattern type
			//TODO
			break;
		}
		cnt++;
		if (cnt>60*20) {
			show_highscore();
			cnt=0;
		}
		flip_screen();
	}
	return gametype;
}

int main(int argc,char *argv[]) {
	int rc,gt;

#ifdef __SWITCH__
	//socketInitializeDefault();
	//nxlinkStdio();
	romfsInit();
#endif
	rc=SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK);
	if(rc!=0) {
		printf("Error while SDL_Init\n");
		exit(1);
	}

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}
	create_board(INLINE,9,7,MAXGEMCOL-1);
	init_rand(time(NULL));
	init_screen();
	load_assets();
	init_sound();
	load_sound();
	inithiscore();
	loadhiscore("sc.dat");

	mouse_curs=NULL;
#ifndef PANDORA
	init_frame_skip();
	reset_frame_skip();
#endif
	play_music(SID_MUSIC2);
	intro_loop();
	while ((gt=main_menu())!=-1) {
		printf("Gametype=%d\n",gt);
		rc=run_game_mainloop(gt,0);
		switch (rc) {
		case RT_CANCEL:
			break;
		case RT_GAMEOVER:
			break;
		}
		play_music(SID_MUSIC2);
	}
	savehiscore("sc.dat");

    if (SDL_JoystickGetAttached(joy)) {
        SDL_JoystickClose(joy);
    }

	deinit_sound();
	SDL_Quit();

#ifdef __SWITCH__
	romfsExit();
	//socketExit();
#endif

	return 0;
}
