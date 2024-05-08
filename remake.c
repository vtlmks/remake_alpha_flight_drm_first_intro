#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <loader.h>
#include <remake.h>

#include "data/music.h"
#include "data/background.h"
#include "data/testfont14.h"

#include "resampler.c"

#define ARRAY_SIZE(_Array) (sizeof(_Array) / sizeof(_Array[0]))


uint32_t copper_blue_stripes_start_index = 0;
uint32_t copper_blue_stripes[] = {
	0x0000ff00, 0x000000ff, 0x0000ee00, 0x000000ff, 0x0000dd00, 0x000000ff, 0x0000cc00,
	0x000000ff, 0x0000bb00, 0x000000ff, 0x0000aa00, 0x000000ff, 0x00009900, 0x000000ff,
	0x00008800, 0x000000ff, 0x00007700, 0x000000ff, 0x00006600, 0x000000ff, 0x00005500,
	0x000000ff, 0x00004400, 0x000000ff, 0x00003300, 0x000000ff, 0x00002200, 0x000000ff,
	0x00001100
};

uint32_t large_copper_bar_sine_offset = 0;
uint32_t large_copper_bars[7][7] = {		// 7 color bars, 5 pixels high each
	{ 0x0055aaff, 0x0088ddff, 0x00aaffff, 0x0088ddff, 0x0066aaff, 0x004477ff, 0x002244ff, },
	{ 0x008888ff, 0x00ccbbff, 0x00ffddff, 0x00ddbbff, 0x00aa88ff, 0x007755ff, 0x003322ff, },
	{ 0x008800ff, 0x00cc00ff, 0x00ff00ff, 0x00dd00ff, 0x00aa00ff, 0x007700ff, 0x003300ff, },
	{ 0x880000ff, 0xcc0000ff, 0xff0000ff, 0xdd0000ff, 0xaa0000ff, 0x770000ff, 0x330000ff, },
	{ 0x885500ff, 0xcc9900ff, 0xffcc00ff, 0xddaa00ff, 0xaa7700ff, 0x774400ff, 0x331100ff, },
	{ 0x888800ff, 0xcccc00ff, 0xffff00ff, 0xdddd00ff, 0xaaaa00ff, 0x777700ff, 0x333300ff, },
	{ 0x888888ff, 0xccccccff, 0xffffffff, 0xddddddff, 0xaaaaaaff, 0x777777ff, 0x333333ff, },
};

uint32_t small_copper_bars_sine_offset = 0;
uint32_t small_copper_bars[7][5] = {		// 7 copper bars, 5 pixels high each
	{ 0x0077ccff, 0x00aaffff, 0x0088ddff, 0x0066aaff, 0x004466ff, },
	{ 0x00ccbbff, 0x00ffddff, 0x00ccbbff, 0x008877ff, 0x004433ff, },
	{ 0x00cc00ff, 0x00ff00ff, 0x00cc00ff, 0x008800ff, 0x004400ff, },
	{ 0xcc0000ff, 0xff0000ff, 0xcc0000ff, 0x880000ff, 0x440000ff, },
	{ 0xcc9900ff, 0xffcc00ff, 0xddaa00ff, 0x996600ff, 0x442200ff, },
	{ 0xcccc00ff, 0xffff00ff, 0xcccc00ff, 0x888800ff, 0x444400ff, },
	{ 0xccccccff, 0xffffffff, 0xccccccff, 0x888888ff, 0x444444ff, },
};

uint32_t scroll_colors[85] = {
	0x0000ffff, 0x222222ff, 0x0033ffff, 0x444444ff, 0x0055ffff, 0x666666ff, 0x0077ffff, 0x888888ff,
	0x0099ffff, 0xaaaaaaff, 0x00bbffff, 0xccccccff, 0x00ddffff, 0xeeeeeeff, 0x00ffffff, 0xffffffff,
	0x00ffddff, 0xeeeeeeff, 0x00ffbbff, 0xddddddff, 0x00ff99ff, 0xaaaaaaff, 0x00ff77ff, 0x888888ff,
	0x00ff55ff, 0x666666ff, 0x00ff33ff, 0x444444ff, 0x00ff00ff, 0x444400ff, 0x33ff00ff, 0x666600ff,
	0x55ff00ff, 0x888800ff, 0x77ff00ff, 0xaaaa00ff, 0x99ff00ff, 0xcccc00ff, 0xbbff00ff, 0xeeee00ff,
	0xddff00ff, 0xffff00ff, 0xffff00ff, 0xeeee00ff, 0xffdd00ff, 0xcccc00ff, 0xffbb00ff, 0xaaaa00ff,
	0xff9900ff, 0x888800ff, 0xff7700ff, 0x666600ff, 0xff5500ff, 0x444400ff, 0xff3300ff, 0x004444ff,
	0xff0000ff, 0x006666ff, 0xff0033ff, 0x008888ff, 0xff0055ff, 0x00aaaaff, 0xff0077ff, 0x00ccccff,
	0xff0099ff, 0x00eeeeff, 0xff00bbff, 0x00ffffff, 0xff00ddff, 0x00ffffff, 0xff00ffff, 0x00eeeeff,
	0xdd00ffff, 0x00ccccff, 0xbb00ffff, 0x00aaaaff, 0x9900ffff, 0x008888ff, 0x7700ffff, 0x006666ff,
	0x5500ffff, 0x005555ff, 0x3300ffff, 0x004444ff, 0x0000ffff,
};

uint8_t half_sine_bounce[] = {
	0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x12, 0x14, 0x18, 0x1a,
	0x1e, 0x22, 0x26, 0x28, 0x2e, 0x32, 0x36, 0x3c, 0x40, 0x46, 0x4c, 0x52, 0x58, 0x5e, 0x64, 0x6a,
	0x70, 0x78, 0x80, 0x86, 0x8e, 0x96, 0x9e, 0xa6, 0x9e, 0x96, 0x8e, 0x86, 0x80, 0x78, 0x70, 0x6a,
	0x64, 0x5e, 0x58, 0x52, 0x4c, 0x46, 0x40, 0x3c, 0x36, 0x32, 0x2e, 0x28, 0x26, 0x22, 0x1e, 0x1a,
	0x18, 0x14, 0x12, 0x0e, 0x0c, 0x0a, 0x08, 0x06, 0x04, 0x04, 0x02, 0x02, 0x00, 0x00, 0x00,
};

uint8_t full_sine[] = {
	0x38, 0x40, 0x46, 0x4c, 0x50, 0x56, 0x5a, 0x60, 0x64, 0x68, 0x6a, 0x6e, 0x70, 0x72,
	0x72, 0x72, 0x70, 0x6e, 0x6a, 0x68, 0x64, 0x60, 0x5a, 0x56, 0x50, 0x4c, 0x46, 0x40,
	0x38, 0x32, 0x2c, 0x26,	0x22, 0x1c, 0x16, 0x12, 0x0e, 0x0a, 0x08, 0x04, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x04, 0x08, 0x0a, 0x0e, 0x12, 0x16, 0x1c, 0x22, 0x26, 0x2c, 0x32,
};

char *scroll_text = {
	"     THE ALPHA FLIGHT AMIGA SECTION BECOMES BIGGER . THIS TIME "
	"IT'S DOCTOR MABUSE, THE MASTER OF THE COLOR BARS !! AFTER  A LOT"
	" OF PROBLEMS I CAN FINALLY PRESENT YOU MY FIRST AMIGA INTRO . "
	"DONT'T BE SORRY FDT, THIS TIME I AM THE FIRST ! ITS HARD TO GET"
	" INFORMATIONS ABOUT THE AMIGA BECAUSE THE GUYS AROUND HERE HAVE "
	"NOT THE SLIGHTEST NOTION IN PROGRAMMING .  IF YOU WANT TO CONTACT"
	" ME FOR SWAPPING OR OTHER INTELLIGENT THINGS WRITE TO  :  DR. MABUSE"
	" ( AFL ) - PLK 043366 B - 4000 DUESSELDORF 1 - GERMANY .   NOW "
	"THE GREETINGS O.K. ??   MEGA GREETINGS TO ALL MEMBERS OF THE GREAT"
	" ALPHA FLIGHT ( NOT VERY MUCH MEMBERS NOW )   AND TO AOFCF ( "
	"HE IS NOW A GREAT GSC MEMBER ! )   NORMAL GREETINGS TO :  DNS, "
	"HQC, RELAX, RSI, TLC, ACF, MOVERS, TTF, TSK, SKAR, 1001, FLT, "
	"SAP, TDF, DEF JAM AND J.R.   AND OF COURSE TO ALL MY FRIENDS "
	"FROM THE C-64   BALDUR, DARKNESS  AND MANY MORE .  AND AT LAST"
	" TO ALL OTHER DR. MABUSES AROUND THE WORLD . SPECIAL FUCKINGS "
	"GO TO S.C.A AND THEIR VIRUS. CRIME, WHY DIDN'T YOU HELP FDT AND"
	" ME WITH OUR 'C' PROBLEMS ?? I DON'T UNDERSTAND YOUR STRANGE "
	"BEHAVIOUR .   THIS INTRO IS COMPLETELY WRITTEN IN ASSEMBLER ."
	" INCREDABLE HOW FAST THE 68000 IS .  THE MUSIC IS OF COURSE "
	"FROM DETONATOR . THE COLOR BARS ARE NICE AREN'T THEY . IT WAS"
	" A VERY HARD WORK .   LET ME FINISH NOW . I HOPE YOU ENJOYED"
	" THIS INTRO .  GOOD BYE        ........             "
	"(C)  DR. MABUSE INDUSTRIES                     ^"
};


uint32_t copper_background[229];
uint32_t copper_behind_image[BACKGROUND_HEIGHT];
uint32_t bars_on_top_of_eachother_sine_offset = 0;
uint32_t bars_on_top_of_eachother[87];

uint8_t scroll_buffer[(320+16) * 14];

int32_t mainloop_callback(struct loader_shared_state *state) {
	struct remake *remake = (struct remake *)state->remake_userdata;
	memset(copper_background, 0, sizeof(copper_background));
	memset(copper_behind_image, 0, sizeof(copper_behind_image));

	{	// clear screen to deep blue
		uint32_t *dst = state->buffer;
		for(uint32_t i = 0; i < state->buffer_width*state->buffer_height; ++i) {
			dst[i] = 0x000044ff;
		}
	}

	{	// background blue stripes to copper_background buffer..
		for(uint32_t y = 0; y < 229; ++y) {
			copper_background[y] = copper_blue_stripes[(copper_blue_stripes_start_index + y) % ARRAY_SIZE(copper_blue_stripes)];
		}
		copper_blue_stripes_start_index++;
	}

	{	// Bouncing copper bars

		uint32_t temp_sine_offset = large_copper_bar_sine_offset;
		for(uint32_t i = 0; i < 7; ++i) {
			uint32_t *dst = copper_background + 139 + (half_sine_bounce[temp_sine_offset % ARRAY_SIZE(half_sine_bounce)] >> 1);
			for(uint32_t y = 0; y < 7; ++y) {
				dst[y] = large_copper_bars[i][y];
			}
			temp_sine_offset += 4;
		}
		if((state->frame_number & 0x1) == 0) {
			large_copper_bar_sine_offset += 1;
		}
	}

	copper_background[202] = 0x000000ff;	// offset 202 = black line above scroller
	copper_background[219] = 0x000000ff;	// offset 219 = black line under scroller

	{	// render background copper
		uint32_t *dst = state->buffer + state->buffer_width;
		for(uint32_t y = 0; y < 229; ++y) {
			uint32_t line_color = copper_background[y];
			for(uint32_t x = 0; x < state->buffer_width; ++x) {
				dst[x] = line_color;
			}
			dst += state->buffer_width;
		}
	}

	{ // copper bars moving up and down in sine offset = 22   (24 high area)  6 pixels separation
		uint32_t temp_sine_offset = bars_on_top_of_eachother_sine_offset;
		uint32_t *dst = copper_behind_image + 4;
		uint32_t *src = bars_on_top_of_eachother + (full_sine[temp_sine_offset % ARRAY_SIZE(full_sine)] >>1);
		for(uint32_t y = 0; y < 24; ++y) {
			dst[y] = src[y];
		}
		if((state->frame_number % 4) == 0) {
			bars_on_top_of_eachother_sine_offset += 1;
		}
	}

	{	// small copper bars in sine seen through the AFL logo
		uint32_t temp_sine_offset = small_copper_bars_sine_offset;
		for(uint32_t i = 0; i < 7; ++i) {
			uint32_t *dst = copper_behind_image + 51 + (full_sine[temp_sine_offset % ARRAY_SIZE(full_sine)] >>1);
			for(uint32_t y = 0; y < 5; ++y) {
				dst[y] = small_copper_bars[i][y];
			}
			temp_sine_offset += 3;
		}
		if((state->frame_number % 3) == 0) {
			small_copper_bars_sine_offset += 1;
		}
	}

	{	// Draw logo
		uint32_t *dst = state->buffer + 18*state->buffer_width + ((state->buffer_width-BACKGROUND_WIDTH)>>1);
		uint8_t *src = background_data;
		for(uint32_t y = 0; y < BACKGROUND_HEIGHT; ++y) {
			uint32_t line_color = copper_behind_image[y];
			for(uint32_t x = 0; x < BACKGROUND_WIDTH; ++x) {
				uint8_t color = *src++;
				switch(color) {
					case 0: {
					} break;
					case 1: {
						if(line_color) {
							dst[x] = line_color;
						}
					} break;
					default: {
						dst[x] = background_palette[color];
					} break;
				}
			}
			dst += state->buffer_width;
		}
	}

	{ // Scroller
		memmove(scroll_buffer, scroll_buffer+2, sizeof(scroll_buffer)-2);
		static uint32_t scroll_count = 7;
		static uint32_t scroll_text_offset = 0;

		if(scroll_count-- == 0) {
			scroll_count = 7;

			uint8_t c = scroll_text[scroll_text_offset++];

			// NOTE(peter): Parse scroller before subtracting 0x20 ' ' from the character!
			if(c == '^') {
				scroll_text_offset = 1;
				c = scroll_text[0];
			}

			c = c - ' ';

			uint8_t *src = testfont14_data + (c*16*14);
			uint8_t *dst = scroll_buffer + 320;
			for(uint32_t y = 0; y < 14; ++y) {
				uint32_t line_color = scroll_colors[y];
				for(uint32_t x = 0; x < 16; ++x) {
					uint8_t color = *src++;
					if(color) {
						dst[x] = line_color;
					} else {
						dst[x] = 0;
					}
				}
				dst += 336;
			}
		}

		uint32_t *dst = state->buffer + 205*state->buffer_width + ((state->buffer_width-320) >> 1);
		uint8_t *src = scroll_buffer;
		for(uint32_t y = 0; y < 14; ++y) {
			uint32_t line_color = scroll_colors[y+1];
			for(uint32_t x = 0; x < 320; ++x) {
				uint8_t color = src[x];
				if(color) {
					dst[x] = line_color;
				}
			}
			src += 336;
			dst += state->buffer_width;
		}
	}

	{ // Cycle the scroll_colors
		if((state->frame_number & 0x3) == 0) {
			// Cycle down
			uint32_t first = 0;
			uint32_t temp = scroll_colors[0];
			for(uint32_t i = 0; i < 42; ++i) {
				scroll_colors[first] = scroll_colors[first+2];
				first += 2;
			}
			scroll_colors[84] = temp;

			// Cycle up
			uint32_t last = 83;
			temp = scroll_colors[83];
			for(uint32_t i = 0; i < 41; ++i) {
				scroll_colors[last] = scroll_colors[last-2];
				last -= 2;
			}
			scroll_colors[1] = temp;
		}

	}

	return 0;
}

int16_t *resampled_music_data;
uint32_t outsize;

struct remake  {
};

void setup(struct loader_shared_state *state) {
	state->remake_userdata = (struct remake *)calloc(1, sizeof(struct remake));
	resampled_music_data = resample_audio(music_data, sizeof(music_data), 380, &outsize);
}

void cleanup(struct loader_shared_state *state) {
	struct remake *remake = (struct remake *)state->remake_userdata;

	free(resampled_music_data);
}

void key_callback(struct loader_shared_state *state, int key) {
	struct remake *remake = (struct remake *)state->remake_userdata;
}

void audio_callback(struct loader_shared_state *state, int16_t *audio_buffer, size_t frames) {
	struct remake *remake = (struct remake *)state->remake_userdata;
	static uint32_t audio_sample_count;
	int16_t *dst = audio_buffer;
	for(uint32_t i = 0; i < frames; ++i) {
		int16_t sample = resampled_music_data[audio_sample_count];
		*dst++ = sample;
		*dst++ = sample;
		audio_sample_count++;
		audio_sample_count %=  outsize;
	}
}

struct remake_info remake_information = {
	.release_name = "Alpha Flight - Dr.Mabuse First Intro",
	.display_name = "Alpha Flight - Dr.Mabuse First Intro",
	.author_name = "ViTAL/Mindkiller Systems",
	.buffer_width = 368,
	.buffer_height = 276,
	.frames_per_second = 50,
	.setup = setup,
	.cleanup = cleanup,
	.key_callback = key_callback,
	.audio_callback = audio_callback,
	.mainloop_callback = mainloop_callback,
};

// NOTE(peter): This is only for windows, as it's too lame to be able to getProcessAddress of the struct, like dlsym can on linux.
EXPORT struct remake_info *get_remake_information() { return &remake_information; }
