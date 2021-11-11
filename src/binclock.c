#include <binclock.h>
#include <render.h>

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

uint16_t *map;
struct fb_fix_screeninfo screen;
time_t now;
struct tm *now_tm;

void verify_argc(int argc) {
  if (argc != 3) {
    printf("Usage: binclock <output_output_device> <input_device>\n");
    exit(0);
  }
}

int get_rpi_output_device(char* path) {
  int output_device = open(path, O_RDWR);

  if (output_device == -1) {
    fprintf(stderr, "Unable to open output device '%s'", path);
    perror("");
    exit(EXIT_FAILURE);
  }

  if (ioctl(output_device, FBIOGET_FSCREENINFO, &screen) == -1) {
    perror("Unable to open RPI Sense");
    close(output_device);
    exit(EXIT_FAILURE);
  }
  return output_device;
}

FILE* get_rpi_input_device(char* path) {
    return fopen(path, "r");
}

uint8_t read_directions(FILE* input_device) {
    uint8_t directions = 0;
    char input_buffer[INPUT_DEV_LEN];
    fgets(input_buffer, INPUT_DEV_LEN, input_device);
    char direction_key = input_buffer[RPI_INPUT_DIRECTION_IDX];
    
    if (direction_key == RPI_INPUT_UP) { directions |= 1; }
    if (direction_key == RPI_INPUT_RIGHT) { directions |= 2; }
    if (direction_key == RPI_INPUT_DOWN) { directions |= 4; }
    if (direction_key == RPI_INPUT_LEFT) { directions |= 8; }
    return directions;
}

void set_clock_mode(short* clock_mode, uint8_t directions, uint8_t last_directions) {
    if (GETBIT(directions, 0) == 1 && GETBIT(last_directions, 0) == 0) {
        *clock_mode *= -1;
    }
    if (GETBIT(directions, 2) == 1 && GETBIT(last_directions, 2) == 0) {
        *clock_mode *= -1;
    }
}

void set_clock_format(short* clock_format, uint8_t directions, uint8_t last_directions) {
    if (GETBIT(directions, 1) == 1 && GETBIT(last_directions, 1) == 0) {
        *clock_format *= -1;
    }
    if (GETBIT(directions, 3) == 1 && GETBIT(last_directions, 3) == 0) {
        *clock_format *= -1;
    }
}

int main(int argc, char *argv[]) {
  verify_argc(argc);

  int output_device = get_rpi_output_device(argv[1]);
  FILE* input_device = get_rpi_input_device(argv[2]);

  ensure_rpi_sense_exists(screen.id, output_device);
  map = allocate_map_memory(output_device);
  show_map_loading(map);

  /* signal(SIGINT, setup_sigint_handler); */
  /* slide_string("retrats temmargorP", 15, COLOR_MINUTES); */

  uint8_t last_directions = 0;
  short clock_mode = CLOCK_MODE_DOUBLE;
  short clock_format = CLOCK_FORMAT_24;

  while (1) {
    uint8_t directions = read_directions(input_device);
    set_clock_mode(&clock_mode, directions, last_directions);
    set_clock_format(&clock_format, directions, last_directions);
    last_directions = directions;

    now = time(NULL);
    now_tm = localtime(&now);

    clear_map(map);
    if (clock_mode == CLOCK_MODE_DOUBLE) {
        render_6_row_clock(map, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, clock_format);
    } else {
        render_3_col_clock(map, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec, clock_format);
    }
  }
}

void setup_sigint_handler(int sig) {
  slide_string(map, "reppots temmargorP", 15, COLOR_SECONDS);
  signal(sig, SIG_IGN);
  exit(0);
}


void ensure_rpi_sense_exists(char *id, int output_device) {
  if (strcmp(id, "RPi-Sense FB") != 0) {
    printf("%s\n", "Error: RPi-Sense FB not found");
    close(output_device);
    exit(EXIT_FAILURE);
  }
}

uint16_t *allocate_map_memory(int output_device) {
  uint16_t *map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                       output_device, 0);
  if (map == MAP_FAILED) {
    close(output_device);
    perror("Error during map allocation");
    exit(EXIT_FAILURE);
  }
  return map;
}

void delay(int ms) { usleep(ms * 1000); }

