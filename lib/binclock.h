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

#ifndef BINCLOCK_H
#define BINCLOCK_H

#define _XOPEN_SOURCE 700
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))
#define CLOCK_MODE_SINGLE 1
#define CLOCK_MODE_DOUBLE -1
#define CLOCK_FORMAT_24 1
#define CLOCK_FORMAT_12 -1
#define INPUT_DEV_LEN 33
#define RPI_INPUT_UP 0x6C
#define RPI_INPUT_RIGHT 0x69
#define RPI_INPUT_DOWN 0x67
#define RPI_INPUT_LEFT 0x6A
#define RPI_INPUT_DIRECTION_IDX 19
#define GETBIT(var, bit) (((var) >> (bit)) & 1)

void delay(int);
void ensure_rpi_sense_exists(char *id, int device);
void setup_sigint_handler(int sig);
void read_rpi_input(FILE* device);
void verify_argc(int argc);
void set_clock_mode(short* clock_mode, uint8_t directions, uint8_t last_directions);
void set_clock_format(short* clock_format, uint8_t directions, uint8_t last_directions);
void render_loop();
void input_loop();
int get_rpi_output_device(char* path);
uint16_t *allocate_map_memory(int device);
FILE* get_rpi_input_device(char* path);

#endif
