/*
 * (C) Gražvydas "notaz" Ignotas, 2012
 *
 * This work is licensed under the terms of any of these licenses
 * (at your option):
 *  - GNU GPL, version 2 or later.
 *  - GNU LGPL, version 2.1 or later.
 *  - MAME license.
 * See the COPYING file in the top-level directory.
 */

#include <stdio.h>
#include <SDL.h>
#include "input.h"
#include "in_sdl.h"

#define IN_SDL_PREFIX "sdl:"
/* should be machine word for best performace */
typedef unsigned long keybits_t;
#define KEYBITS_WORD_BITS (sizeof(keybits_t) * 8)

struct in_sdl_state {
	const in_drv_t *drv;
	SDL_Joystick *joy;
	int joy_id;
	int axis_keydown[2];
	int hat_keydown[2];	
#if 1
	keybits_t keystate[SDL_NUM_SCANCODES / KEYBITS_WORD_BITS + 1];
#else
	keybits_t keystate[SDLK_LAST / KEYBITS_WORD_BITS + 1];
#endif
};

static void (*ext_event_handler)(void *event);

#if 1
static const char * const in_sdl_keys[SDL_NUM_SCANCODES] = {

	[SDL_SCANCODE_BACKSPACE] = "backspace",
	[SDL_SCANCODE_TAB] = "tab",
	[SDL_SCANCODE_CLEAR] = "clear",
	[SDL_SCANCODE_RETURN] = "return",
	[SDL_SCANCODE_PAUSE] = "pause",
	[SDL_SCANCODE_ESCAPE] = "escape",
	[SDL_SCANCODE_SPACE] = "space",
	[SDL_SCANCODE_KP_EXCLAM]  = "!",
//	[SDL_SCANCODE_QUOTEDBL]  = "\"",
	[SDL_SCANCODE_KP_HASH]  = "#",
//	[SDL_SCANCODE_DOLLAR]  = "$",
	[SDL_SCANCODE_KP_AMPERSAND]  = "&",
	[SDL_SCANCODE_APOSTROPHE] = "'",
	[SDL_SCANCODE_KP_LEFTPAREN] = "(",
	[SDL_SCANCODE_KP_RIGHTPAREN] = ")",
	[SDL_SCANCODE_KP_MULTIPLY] = "*",
	[SDL_SCANCODE_KP_PLUS] = "+",
	[SDL_SCANCODE_COMMA] = ",",
	[SDL_SCANCODE_MINUS] = "-",
	[SDL_SCANCODE_PERIOD] = ".",
	[SDL_SCANCODE_SLASH] = "/",
	[SDL_SCANCODE_0] = "0",
	[SDL_SCANCODE_1] = "1",
	[SDL_SCANCODE_2] = "2",
	[SDL_SCANCODE_3] = "3",
	[SDL_SCANCODE_4] = "4",
	[SDL_SCANCODE_5] = "5",
	[SDL_SCANCODE_6] = "6",
	[SDL_SCANCODE_7] = "7",
	[SDL_SCANCODE_8] = "8",
	[SDL_SCANCODE_9] = "9",
	[SDL_SCANCODE_KP_COLON] = ":",
	[SDL_SCANCODE_SEMICOLON] = ";",
	[SDL_SCANCODE_KP_LESS] = "<",
	[SDL_SCANCODE_EQUALS] = "=",
	[SDL_SCANCODE_KP_GREATER] = ">",
//	[SDL_SCANCODE_QUESTION] = "?",
	[SDL_SCANCODE_KP_AT] = "@",
	[SDL_SCANCODE_LEFTBRACKET] = "[",
	[SDL_SCANCODE_BACKSLASH] = "\\",
	[SDL_SCANCODE_RIGHTBRACKET] = "]",
	[SDL_SCANCODE_KP_POWER] = "^",
//	[SDL_SCANCODE_UNDERSCORE] = "_",
	[SDL_SCANCODE_GRAVE] = "`",
	[SDL_SCANCODE_A] = "a",
	[SDL_SCANCODE_B] = "b",
	[SDL_SCANCODE_C] = "c",
	[SDL_SCANCODE_D] = "d",
	[SDL_SCANCODE_E] = "e",
	[SDL_SCANCODE_F] = "f",
	[SDL_SCANCODE_G] = "g",
	[SDL_SCANCODE_H] = "h",
	[SDL_SCANCODE_I] = "i",
	[SDL_SCANCODE_J] = "j",
	[SDL_SCANCODE_K] = "k",
	[SDL_SCANCODE_L] = "l",
	[SDL_SCANCODE_M] = "m",
	[SDL_SCANCODE_N] = "n",
	[SDL_SCANCODE_O] = "o",
	[SDL_SCANCODE_P] = "p",
	[SDL_SCANCODE_Q] = "q",
	[SDL_SCANCODE_R] = "r",
	[SDL_SCANCODE_S] = "s",
	[SDL_SCANCODE_T] = "t",
	[SDL_SCANCODE_U] = "u",
	[SDL_SCANCODE_V] = "v",
	[SDL_SCANCODE_W] = "w",
	[SDL_SCANCODE_X] = "x",
	[SDL_SCANCODE_Y] = "y",
	[SDL_SCANCODE_Z] = "z",
	[SDL_SCANCODE_DELETE] = "delete",

	[SDL_SCANCODE_KP_0] = "[0]",
	[SDL_SCANCODE_KP_1] = "[1]",
	[SDL_SCANCODE_KP_2] = "[2]",
	[SDL_SCANCODE_KP_3] = "[3]",
	[SDL_SCANCODE_KP_4] = "[4]",
	[SDL_SCANCODE_KP_5] = "[5]",
	[SDL_SCANCODE_KP_6] = "[6]",
	[SDL_SCANCODE_KP_7] = "[7]",
	[SDL_SCANCODE_KP_8] = "[8]",
	[SDL_SCANCODE_KP_9] = "[9]",
	[SDL_SCANCODE_KP_PERIOD] = "[.]",
	[SDL_SCANCODE_KP_DIVIDE] = "[/]",
	[SDL_SCANCODE_KP_MULTIPLY] = "[*]",
	[SDL_SCANCODE_KP_MINUS] = "[-]",
	[SDL_SCANCODE_KP_PLUS] = "[+]",
	[SDL_SCANCODE_KP_ENTER] = "enter",
	[SDL_SCANCODE_KP_EQUALS] = "equals",

	[SDL_SCANCODE_UP] = "up",
	[SDL_SCANCODE_DOWN] = "down",
	[SDL_SCANCODE_RIGHT] = "right",
	[SDL_SCANCODE_LEFT] = "left",
	
	[SDL_SCANCODE_INSERT] = "insert",
	[SDL_SCANCODE_HOME] = "home",
	[SDL_SCANCODE_END] = "end",
	[SDL_SCANCODE_PAGEUP] = "page up",
	[SDL_SCANCODE_PAGEDOWN] = "page down",

	[SDL_SCANCODE_F1] = "f1",
	[SDL_SCANCODE_F2] = "f2",
	[SDL_SCANCODE_F3] = "f3",
	[SDL_SCANCODE_F4] = "f4",
	[SDL_SCANCODE_F5] = "f5",
	[SDL_SCANCODE_F6] = "f6",
	[SDL_SCANCODE_F7] = "f7",
	[SDL_SCANCODE_F8] = "f8",
	[SDL_SCANCODE_F9] = "f9",
	[SDL_SCANCODE_F10] = "f10",
	[SDL_SCANCODE_F11] = "f11",
	[SDL_SCANCODE_F12] = "f12",
	[SDL_SCANCODE_F13] = "f13",
	[SDL_SCANCODE_F14] = "f14",
	[SDL_SCANCODE_F15] = "f15",

	[SDL_SCANCODE_NUMLOCKCLEAR] = "numlock",
	[SDL_SCANCODE_CAPSLOCK] = "caps lock",
	[SDL_SCANCODE_SCROLLLOCK] = "scroll lock",
	[SDL_SCANCODE_RSHIFT] = "right shift",
	[SDL_SCANCODE_LSHIFT] = "left shift",
	[SDL_SCANCODE_RCTRL] = "right ctrl",
	[SDL_SCANCODE_LCTRL] = "left ctrl",
	[SDL_SCANCODE_RALT] = "right alt",
	[SDL_SCANCODE_LALT] = "left alt",
//	[SDLK_RMETA] = "right meta",
//	[SDLK_LMETA] = "left meta",
//	[SDLK_LSUPER] = "left super",	/* "Windows" keys */
//	[SDLK_RSUPER] = "right super",	
	[SDL_SCANCODE_MODE] = "alt gr",
//	[SDLK_COMPOSE] = "compose",
};
#else
static const char * const in_sdl_keys[SDLK_LAST] = {
	[SDLK_BACKSPACE] = "backspace",
	[SDLK_TAB] = "tab",
	[SDLK_CLEAR] = "clear",
	[SDLK_RETURN] = "return",
	[SDLK_PAUSE] = "pause",
	[SDLK_ESCAPE] = "escape",
	[SDLK_SPACE] = "space",
	[SDLK_EXCLAIM]  = "!",
	[SDLK_QUOTEDBL]  = "\"",
	[SDLK_HASH]  = "#",
	[SDLK_DOLLAR]  = "$",
	[SDLK_AMPERSAND]  = "&",
	[SDLK_QUOTE] = "'",
	[SDLK_LEFTPAREN] = "(",
	[SDLK_RIGHTPAREN] = ")",
	[SDLK_ASTERISK] = "*",
	[SDLK_PLUS] = "+",
	[SDLK_COMMA] = ",",
	[SDLK_MINUS] = "-",
	[SDLK_PERIOD] = ".",
	[SDLK_SLASH] = "/",
	[SDLK_0] = "0",
	[SDLK_1] = "1",
	[SDLK_2] = "2",
	[SDLK_3] = "3",
	[SDLK_4] = "4",
	[SDLK_5] = "5",
	[SDLK_6] = "6",
	[SDLK_7] = "7",
	[SDLK_8] = "8",
	[SDLK_9] = "9",
	[SDLK_COLON] = ":",
	[SDLK_SEMICOLON] = ";",
	[SDLK_LESS] = "<",
	[SDLK_EQUALS] = "=",
	[SDLK_GREATER] = ">",
	[SDLK_QUESTION] = "?",
	[SDLK_AT] = "@",
	[SDLK_LEFTBRACKET] = "[",
	[SDLK_BACKSLASH] = "\\",
	[SDLK_RIGHTBRACKET] = "]",
	[SDLK_CARET] = "^",
	[SDLK_UNDERSCORE] = "_",
	[SDLK_BACKQUOTE] = "`",
	[SDLK_a] = "a",
	[SDLK_b] = "b",
	[SDLK_c] = "c",
	[SDLK_d] = "d",
	[SDLK_e] = "e",
	[SDLK_f] = "f",
	[SDLK_g] = "g",
	[SDLK_h] = "h",
	[SDLK_i] = "i",
	[SDLK_j] = "j",
	[SDLK_k] = "k",
	[SDLK_l] = "l",
	[SDLK_m] = "m",
	[SDLK_n] = "n",
	[SDLK_o] = "o",
	[SDLK_p] = "p",
	[SDLK_q] = "q",
	[SDLK_r] = "r",
	[SDLK_s] = "s",
	[SDLK_t] = "t",
	[SDLK_u] = "u",
	[SDLK_v] = "v",
	[SDLK_w] = "w",
	[SDLK_x] = "x",
	[SDLK_y] = "y",
	[SDLK_z] = "z",
	[SDLK_DELETE] = "delete",

	[SDLK_KP0] = "[0]",
	[SDLK_KP1] = "[1]",
	[SDLK_KP2] = "[2]",
	[SDLK_KP3] = "[3]",
	[SDLK_KP4] = "[4]",
	[SDLK_KP5] = "[5]",
	[SDLK_KP6] = "[6]",
	[SDLK_KP7] = "[7]",
	[SDLK_KP8] = "[8]",
	[SDLK_KP9] = "[9]",
	[SDLK_KP_PERIOD] = "[.]",
	[SDLK_KP_DIVIDE] = "[/]",
	[SDLK_KP_MULTIPLY] = "[*]",
	[SDLK_KP_MINUS] = "[-]",
	[SDLK_KP_PLUS] = "[+]",
	[SDLK_KP_ENTER] = "enter",
	[SDLK_KP_EQUALS] = "equals",

	[SDLK_UP] = "up",
	[SDLK_DOWN] = "down",
	[SDLK_RIGHT] = "right",
	[SDLK_LEFT] = "left",
	[SDLK_INSERT] = "insert",
	[SDLK_HOME] = "home",
	[SDLK_END] = "end",
	[SDLK_PAGEUP] = "page up",
	[SDLK_PAGEDOWN] = "page down",

	[SDLK_F1] = "f1",
	[SDLK_F2] = "f2",
	[SDLK_F3] = "f3",
	[SDLK_F4] = "f4",
	[SDLK_F5] = "f5",
	[SDLK_F6] = "f6",
	[SDLK_F7] = "f7",
	[SDLK_F8] = "f8",
	[SDLK_F9] = "f9",
	[SDLK_F10] = "f10",
	[SDLK_F11] = "f11",
	[SDLK_F12] = "f12",
	[SDLK_F13] = "f13",
	[SDLK_F14] = "f14",
	[SDLK_F15] = "f15",

	[SDLK_NUMLOCK] = "numlock",
	[SDLK_CAPSLOCK] = "caps lock",
	[SDLK_SCROLLOCK] = "scroll lock",
	[SDLK_RSHIFT] = "right shift",
	[SDLK_LSHIFT] = "left shift",
	[SDLK_RCTRL] = "right ctrl",
	[SDLK_LCTRL] = "left ctrl",
	[SDLK_RALT] = "right alt",
	[SDLK_LALT] = "left alt",
	[SDLK_RMETA] = "right meta",
	[SDLK_LMETA] = "left meta",
	[SDLK_LSUPER] = "left super",	/* "Windows" keys */
	[SDLK_RSUPER] = "right super",	
	[SDLK_MODE] = "alt gr",
	[SDLK_COMPOSE] = "compose",
};
#endif

#define JOYBUTTON(x) (1<<(x))

unsigned long uljoybuttontables[] = {
	JOYBUTTON(0), // 0xA0
	JOYBUTTON(1),
	JOYBUTTON(2),
	JOYBUTTON(3),
	JOYBUTTON(4),
	JOYBUTTON(5),
	JOYBUTTON(6),
	JOYBUTTON(7),
	JOYBUTTON(8),
	JOYBUTTON(9),
	JOYBUTTON(10),
	JOYBUTTON(11),
	JOYBUTTON(12),
	JOYBUTTON(13),
	JOYBUTTON(14),
	JOYBUTTON(15),
	JOYBUTTON(10) | JOYBUTTON(15), // 0xb0
	JOYBUTTON(10) | JOYBUTTON(2),
	JOYBUTTON(10) | JOYBUTTON(4),
	JOYBUTTON(10) | JOYBUTTON(5),
	JOYBUTTON(10) | JOYBUTTON(0)

/*
	JOYBUTTON(7) | JOYBUTTON(6), // start + select (0xA8)
	JOYBUTTON(7) | JOYBUTTON(5),
	JOYBUTTON(7) | JOYBUTTON(4),
	JOYBUTTON(7) | JOYBUTTON(3),
	JOYBUTTON(7) | JOYBUTTON(2),
	JOYBUTTON(7) | JOYBUTTON(1),
	JOYBUTTON(7) | JOYBUTTON(0),
	JOYBUTTON(6) | JOYBUTTON(5), // select + R (0xAF)
	JOYBUTTON(6) | JOYBUTTON(4), // select + L (0xB0)
	JOYBUTTON(6) | JOYBUTTON(3), // select + Y (0xB1)
	JOYBUTTON(6) | JOYBUTTON(2), // select + X (0xB2) 
	JOYBUTTON(6) | JOYBUTTON(1), // select + B
	JOYBUTTON(6) | JOYBUTTON(0), // select + A
	JOYBUTTON(5) | JOYBUTTON(4),
	JOYBUTTON(5) | JOYBUTTON(3),
	JOYBUTTON(5) | JOYBUTTON(2),
	JOYBUTTON(5) | JOYBUTTON(1),
	JOYBUTTON(5) | JOYBUTTON(0),
	JOYBUTTON(4) | JOYBUTTON(3),
	JOYBUTTON(4) | JOYBUTTON(2),
	JOYBUTTON(4) | JOYBUTTON(1),
	JOYBUTTON(4) | JOYBUTTON(0),
	JOYBUTTON(3) | JOYBUTTON(2),
	JOYBUTTON(3) | JOYBUTTON(1),
	JOYBUTTON(3) | JOYBUTTON(0),
	JOYBUTTON(2) | JOYBUTTON(1),
	JOYBUTTON(2) | JOYBUTTON(0),
    JOYBUTTON(1) | JOYBUTTON(0)*/};
	
static void in_sdl_probe(const in_drv_t *drv)
{
	const struct in_pdata *pdata = drv->pdata;
	const char * const * key_names = in_sdl_keys;
	struct in_sdl_state *state;
	SDL_Joystick *joy;
	int i, joycount;
	char name[256];

	if (pdata->key_names)
		key_names = pdata->key_names;

	state = calloc(1, sizeof(*state));
	if (state == NULL) {
		fprintf(stderr, "in_sdl: OOM\n");
		return;
	}

	state->drv = drv;
#if 1
	in_register(IN_SDL_PREFIX "keys", -1, state, SDL_NUM_SCANCODES,
		key_names, 0);
#else
	in_register(IN_SDL_PREFIX "keys", -1, state, SDLK_LAST,
		key_names, 0);
#endif

	/* joysticks go here too */
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	joycount = SDL_NumJoysticks();
	for (i = 0; i < joycount; i++) {
		joy = SDL_JoystickOpen(i);
		if (joy == NULL)
			continue;

		state = calloc(1, sizeof(*state));
		if (state == NULL) {
			fprintf(stderr, "in_sdl: OOM\n");
			break;
		}
		state->joy = joy;
		state->joy_id = i;
		state->drv = drv;

#if 1
		snprintf(name, sizeof(name), IN_SDL_PREFIX "%s", SDL_JoystickName(joy));
		in_register(name, -1, state, SDL_NUM_SCANCODES, key_names, 0);
#else
		snprintf(name, sizeof(name), IN_SDL_PREFIX "%s", SDL_JoystickName(i));
		in_register(name, -1, state, SDLK_LAST, key_names, 0);
#endif
	}

	if (joycount > 0)
		SDL_JoystickEventState(SDL_ENABLE);
}

static void in_sdl_free(void *drv_data)
{
	struct in_sdl_state *state = drv_data;

	if (state != NULL) {
		if (state->joy != NULL)
			SDL_JoystickClose(state->joy);
		free(state);
	}
}

static const char * const *
in_sdl_get_key_names(const in_drv_t *drv, int *count)
{
	const struct in_pdata *pdata = drv->pdata;
#if 1
	*count = SDL_NUM_SCANCODES;
#else
	*count = SDLK_LAST;
#endif
	if (pdata->key_names)
		return pdata->key_names;
	return in_sdl_keys;
}

/* could use SDL_GetKeyState, but this gives better packing */
static void update_keystate(keybits_t *keystate, int sym, int is_down)
{
	keybits_t *ks_word, mask;

	mask = 1;
	mask <<= sym & (KEYBITS_WORD_BITS - 1);
	ks_word = keystate + sym / KEYBITS_WORD_BITS;
	if (is_down)
		*ks_word |= mask;
	else
		*ks_word &= ~mask;
}

static int handle_event(struct in_sdl_state *state, SDL_Event *event,
	int *kc_out, int *down_out)
{
	if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
		return -1;

#if 1
	// for SDL2.0
	update_keystate(state->keystate, event->key.keysym.scancode, event->type == SDL_KEYDOWN);
#else
	update_keystate(state->keystate, event->key.keysym.sym,
		event->type == SDL_KEYDOWN);
#endif
	if (kc_out != NULL)
#if 1
		// for SDL2.0
		*kc_out = event->key.keysym.scancode;
#else
		*kc_out = event->key.keysym.sym;
#endif
	if (down_out != NULL)
		*down_out = event->type == SDL_KEYDOWN;

	return 1;
}

static int handle_joy_event(struct in_sdl_state *state, SDL_Event *event,
	int *kc_out, int *down_out)
{
	int kc = -1, down = 0, ret = 0;
	int i=0;
	static unsigned long uljoybuttonstates = 0;
	static unsigned long ulaxis[2]= {0,0};


	
	/* TODO: remaining axis */
	switch (event->type) {
	case SDL_JOYAXISMOTION:
		if (event->jaxis.which != state->joy_id)
			return -2;
		if (event->jaxis.axis > 1)
			break;
		if (-16384 <= event->jaxis.value && event->jaxis.value <= 16384) {
			kc = state->axis_keydown[event->jaxis.axis];
			state->axis_keydown[event->jaxis.axis] = 0;
			ret = 1;
		}
		else if (event->jaxis.value < -16384) {
			kc = state->axis_keydown[event->jaxis.axis];
			if (kc)
				update_keystate(state->keystate, kc, 0);
#if 1
			kc = event->jaxis.axis ? SDL_SCANCODE_UP : SDL_SCANCODE_LEFT;
#else
			kc = event->jaxis.axis ? SDLK_UP : SDLK_LEFT;
#endif			
			state->axis_keydown[event->jaxis.axis] = kc;
			down = 1;
			ret = 1;
		}
		else if (event->jaxis.value > 16384) {
			kc = state->axis_keydown[event->jaxis.axis];
			if (kc)
				update_keystate(state->keystate, kc, 0);
#if 1
			kc = event->jaxis.axis ? SDL_SCANCODE_DOWN : SDL_SCANCODE_RIGHT;
#else				
			kc = event->jaxis.axis ? SDLK_DOWN : SDLK_RIGHT;
#endif
			state->axis_keydown[event->jaxis.axis] = kc;
			down = 1;
			ret = 1;
		}
		break;
	case SDL_JOYHATMOTION:
		if (event->jhat.which != state->joy_id)
		return -2;
		if (event->jhat.value == SDL_HAT_CENTERED) {
			if ( (state->hat_keydown[0] != 0) && (state->hat_keydown[1] != 0))
			{				
				kc = state->hat_keydown[0];
				update_keystate(state->keystate, kc, 0);
				state->hat_keydown[0] = 0;

				kc = state->hat_keydown[1];
				state->hat_keydown[1] = 0;
							
				ret = 1;				
			}
			else
			{
				if (state->hat_keydown[0] != 0)	
				{
					kc = state->hat_keydown[0];
					state->hat_keydown[0] = 0;
					ret = 1;
				}
				else if (state->hat_keydown[1] != 0)
				{
					kc = state->hat_keydown[1];
					state->hat_keydown[1] =0;
					ret = 1;	
				}			
			}
		}
		else 
		{
			if (event->jhat.value & SDL_HAT_LEFT || event->jhat.value & SDL_HAT_RIGHT) {
				kc = state->hat_keydown[0];
				if (kc)
					update_keystate(state->keystate, kc, 0);
				kc = (event->jhat.value & SDL_HAT_LEFT) ? SDL_SCANCODE_LEFT : SDL_SCANCODE_RIGHT;
				state->hat_keydown[0] = kc;
				down = 1;
				ret = 1;
			}
			else
			{
				if (state->hat_keydown[0] != 0)
				{
					update_keystate(state->keystate, state->hat_keydown[0], 0);
					state->hat_keydown[0] = 0;	
				}				
			}
			
			if (event->jhat.value & SDL_HAT_UP || event->jhat.value & SDL_HAT_DOWN) {
				kc = state->hat_keydown[1];
				if (kc)
					update_keystate(state->keystate, kc, 0);
				kc = (event->jhat.value & SDL_HAT_UP) ? SDL_SCANCODE_UP : SDL_SCANCODE_DOWN;
				state->hat_keydown[1] = kc;

				down = 1;
				ret = 1;
			}
			else
			{
				if (state->hat_keydown[1] != 0)
				{
					update_keystate(state->keystate, state->hat_keydown[1], 0);
					state->hat_keydown[1] = 0;	
				}
			}
		}		break;
	case SDL_JOYBUTTONDOWN:
		if (event->jbutton.which != state->joy_id)
			return -2;
		
		uljoybuttonstates |= JOYBUTTON(event->jbutton.button);
		for(i=0; i<sizeof(uljoybuttontables)/sizeof(unsigned long); i++)
		{
			if (uljoybuttonstates == uljoybuttontables[i])
			{
				kc = i + SDLK_WORLD_0;
				down = 1;
				ret = 1;
				break;
			}
		}
		if (i>=sizeof(uljoybuttontables)/sizeof(unsigned long))
		{
			kc = (int)event->jbutton.button + SDLK_WORLD_0;
			down = 1;
			ret = 1;
		}

		
		//printf("SDL_JOYBUTTONDOWN: uljoybuttonstates = 0x%lx, kc = 0x%x, jbutton.state = %d\r\n", uljoybuttonstates, kc, event->jbutton.state);
		break;
	case SDL_JOYBUTTONUP:
		if (event->jbutton.which != state->joy_id)
			return -2;
		
		if (uljoybuttonstates)
		{
			for(i=0; i<sizeof(uljoybuttontables)/sizeof(unsigned long); i++)
			{
				if (uljoybuttonstates == uljoybuttontables[i])
				{
					kc = i + SDLK_WORLD_0;
					break;
				}
			}
			if (i>=sizeof(uljoybuttontables)/sizeof(unsigned long))
			{
				kc = (int)event->jbutton.button + SDLK_WORLD_0;
			}	
			
			uljoybuttonstates &= ~JOYBUTTON(event->jbutton.button);
		}
		else
		{
			kc = (int)event->jbutton.button + SDLK_WORLD_0;
		}
		
		//printf("SDL_JOYBUTTONUP: uljoybuttonstates = 0x%lx, kc = 0x%x, jbutton.state = %d\r\n", uljoybuttonstates, kc, event->jbutton.state);
		ret = 1;
		break;
	default:
		return -1;
	}

	if (ret)
		update_keystate(state->keystate, kc, down);

	
	if (kc_out != NULL)
		*kc_out = kc;
	if (down_out != NULL)
		*down_out = down;

	return ret;
}

#define JOY_EVENTS (SDL_JOYAXISMOTIONMASK | SDL_JOYBALLMOTIONMASK | SDL_JOYHATMOTIONMASK \
		    | SDL_JOYBUTTONDOWNMASK | SDL_JOYBUTTONUPMASK)

static int collect_events(struct in_sdl_state *state, int *one_kc, int *one_down)
{
	SDL_Event events[4];
#if 0
	Uint32 mask = state->joy ? JOY_EVENTS : (SDL_ALLEVENTS & ~JOY_EVENTS);
#endif
	int count, maxcount;
	int i, ret, retval = 0;
	int num_events, num_peeped_events;
	SDL_Event *event;

	maxcount = (one_kc != NULL) ? 1 : sizeof(events) / sizeof(events[0]);

	SDL_PumpEvents();

#if 1
	if (state->joy)
		num_events = SDL_PeepEvents(events, maxcount, SDL_PEEKEVENT, SDL_JOYAXISMOTION, SDL_LASTEVENT);
	else 
		num_events = SDL_PeepEvents(events, maxcount, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_MOUSEWHEEL);
#else
	num_events = SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, mask);
#endif
#if 0
// 상수값 참고
    SDL_JOYAXISMOTION  = 0x600, /**< Joystick axis motion */
    SDL_JOYBALLMOTION,          /**< Joystick trackball motion */
    SDL_JOYHATMOTION,           /**< Joystick hat position change */
    SDL_JOYBUTTONDOWN,          /**< Joystick button pressed */
    SDL_JOYBUTTONUP,  
	
    SDL_KEYDOWN        = 0x300, /**< Key pressed */
    SDL_KEYUP,                  /**< Key released */	
#endif

	for (num_peeped_events = 0; num_peeped_events < num_events; num_peeped_events += count) {
#if 1
		if (state->joy)
			count = SDL_PeepEvents(events, maxcount, SDL_GETEVENT, SDL_JOYAXISMOTION, SDL_LASTEVENT);
		else
			count = SDL_PeepEvents(events, maxcount, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_MOUSEWHEEL);

#else
		count = SDL_PeepEvents(events, maxcount, SDL_GETEVENT, mask);
#endif
		if (count <= 0)
			break;
		for (i = 0; i < count; i++) {
			event = &events[i];
			if (state->joy)
				ret = handle_joy_event(state,
					event, one_kc, one_down);
			else
				ret = handle_event(state,
					event, one_kc, one_down);
			if (ret < 0) {
				switch (ret) {
					case -2:
						SDL_PushEvent(event);
						break;
					default:
						if (ext_event_handler != NULL)
							ext_event_handler(event);
						break;
				}
				continue;
			}

			retval |= ret;
			if (one_kc != NULL && ret)
			{
				// don't lose events other devices might want to handle
				for (i++; i < count; i++)
					SDL_PushEvent(&events[i]);
				goto out;
			}
		}
	}

out:
	return retval;
}

static int in_sdl_update(void *drv_data, const int *binds, int *result)
{
	struct in_sdl_state *state = drv_data;
	keybits_t mask;
	int i, sym, bit, b;

	collect_events(state, NULL, NULL);

#if 1
	for (i = 0; i < SDL_NUM_SCANCODES / KEYBITS_WORD_BITS + 1; i++) {
#else
	for (i = 0; i < SDLK_LAST / KEYBITS_WORD_BITS + 1; i++) {
#endif		
		mask = state->keystate[i];
		if (mask == 0)
			continue;
		for (bit = 0; mask != 0; bit++, mask >>= 1) {
			if ((mask & 1) == 0)
				continue;
			sym = i * KEYBITS_WORD_BITS + bit;

			for (b = 0; b < IN_BINDTYPE_COUNT; b++)
				result[b] |= binds[IN_BIND_OFFS(sym, b)];
		}
	}

	return 0;
}

static int in_sdl_update_keycode(void *drv_data, int *is_down)
{
	struct in_sdl_state *state = drv_data;
	int ret_kc = -1, ret_down = 0;

	collect_events(state, &ret_kc, &ret_down);

	if (is_down != NULL)
		*is_down = ret_down;

	return ret_kc;
}

static int in_sdl_menu_translate(void *drv_data, int keycode, char *charcode)
{
	struct in_sdl_state *state = drv_data;
	const struct in_pdata *pdata = state->drv->pdata;
	const char * const * key_names = in_sdl_keys;
	const struct menu_keymap *map;
	int map_len;
	int ret = 0;
	int i;

	if (pdata->key_names)
		key_names = pdata->key_names;

	if (state->joy) {
		map = pdata->joy_map;
		map_len = pdata->jmap_size;
	} else {
		map = pdata->key_map;
		map_len = pdata->kmap_size;
	}

	if (keycode < 0)
	{
		/* menu -> kc */
		keycode = -keycode;
		for (i = 0; i < map_len; i++)
			if (map[i].pbtn == keycode)
				return map[i].key;
	}
	else
	{
		for (i = 0; i < map_len; i++) {
			if (map[i].key == keycode) {
				ret = map[i].pbtn;
				break;
			}
		}

#if 1
		if (charcode != NULL && (unsigned int)keycode < SDL_NUM_SCANCODES &&
#else
		if (charcode != NULL && (unsigned int)keycode < SDLK_LAST &&
#endif		
		    key_names[keycode] != NULL && key_names[keycode][1] == 0)
		{
			ret |= PBTN_CHAR;
			*charcode = key_names[keycode][0];
		}
	}

	return ret;
}

static const in_drv_t in_sdl_drv = {
	.prefix         = IN_SDL_PREFIX,
	.probe          = in_sdl_probe,
	.free           = in_sdl_free,
	.get_key_names  = in_sdl_get_key_names,
	.update         = in_sdl_update,
	.update_keycode = in_sdl_update_keycode,
	.menu_translate = in_sdl_menu_translate,
};

int in_sdl_init(const struct in_pdata *pdata, void (*handler)(void *event))
{
	if (!pdata) {
		fprintf(stderr, "in_sdl: Missing input platform data\n");
		return -1;
	}

	in_register_driver(&in_sdl_drv, pdata->defbinds, pdata);
	ext_event_handler = handler;
	return 0;
}
