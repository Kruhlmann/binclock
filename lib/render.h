#include <fcntl.h>
#include <linux/fb.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef RENDER_H
#define RENDER_H

#define LOAD_DELAY 4
#define WORD_W 8
#define WORD_H 8
#define NUM_WORDS WORD_W *WORD_H
#define COLOR_SECONDS 0xF0F
#define COLOR_MINUTES 0xF80
#define COLOR_HOURS 0xF800

int hi_bits_dec(int value, char shifter);
int lo_bits_dec(int value, char shifter);
void render_char(char character, uint16_t* map, int dx, unsigned int color);
void render_string(char *string, uint16_t* map, int dx, unsigned int color);
void slide_string(uint16_t *map, char* string, unsigned int interval, unsigned int color);
void render_6_row_clock(uint16_t *map, int hours, int minutes, int seconds, short format);
void render_3_col_clock(uint16_t *map, int hours, int minutes, int seconds, short format);
void clear_map(uint16_t *map);
void show_map_loading(uint16_t *map);
void blit(uint16_t *map, int x, int y, unsigned int color);
void blit_rect(uint16_t *map, char x, char y, char w, char h, unsigned int color);

#endif
