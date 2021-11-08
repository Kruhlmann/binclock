#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define LOAD_DELAY 6
#define DEVICE_PATH "/dev/fb0"
#define WORD_W 8
#define WORD_H 8
#define NUM_WORDS WORD_W *WORD_H
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))

void delay(int);
void clear_map(uint16_t *map);
void show_map_loading(uint16_t *map);
void ensure_rpi_sense_exists(char *id, int device);
uint16_t *allocate_map_memory(int device);

int main() {
  int fbfd;
  struct fb_fix_screeninfo fix_info;

  fbfd = open(DEVICE_PATH, O_RDWR);
  if (fbfd == -1) {
    fprintf(stderr, "Unable to open device '%s'", DEVICE_PATH);
    perror("");
    exit(EXIT_FAILURE);
  }

  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
    perror("Unable to open RPI Sense");
    close(fbfd);
    exit(EXIT_FAILURE);
  }

  ensure_rpi_sense_exists(fix_info.id, fbfd);
  uint16_t *map = allocate_map_memory(fbfd);
  show_map_loading(map);

  while (1) {
    time_t now;
    struct tm *now_tm;
    now = time(NULL);
    now_tm = localtime(&now);

    for (char i = 0; i < 8; i++) {
      *(map + i + WORD_W * 0) = ((now_tm->tm_hour >> i) & 0x01) * 0xF800;
    }
    for (char i = 0; i < 8; i++) {
      *(map + i + WORD_W * 1) = ((now_tm->tm_min >> i) & 0x01) * 0xF80;
    }
    for (char i = 0; i < 8; i++) {
      *(map + i + WORD_W * 2) = ((now_tm->tm_sec >> i) & 0x01) * 0xF0F;
    }
    delay(100);
  }
}

void ensure_rpi_sense_exists(char *id, int device) {
  if (strcmp(id, "RPi-Sense FB") != 0) {
    printf("%s\n", "Error: RPi-Sense FB not found");
    close(device);
    exit(EXIT_FAILURE);
  }
}

uint16_t *allocate_map_memory(int device) {
  uint16_t *map =
      mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, device, 0);
  if (map == MAP_FAILED) {
    close(device);
    perror("Error during map allocation");
    exit(EXIT_FAILURE);
  }
  return map;
}

void delay(int ms) { usleep(ms * 1000); }

void show_map_loading(uint16_t *map) {
  for (int i = 0; i < NUM_WORDS; i++) {
    *(map + i) = 0xF800;
    delay(LOAD_DELAY);
  }
  clear_map(map);
}

void clear_map(uint16_t *map) {
  for (int i = 0; i < NUM_WORDS; i++) {
    *(map + i) = 0;
  }
}
