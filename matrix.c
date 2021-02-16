
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <math.h>
//#include <time.h> // for nanosleep() potentially

//these really should be enums but eh
#define RED 0   //define the red data source
#define BLUE 1  //define the blue data source
#define GREEN 2 // define the green data source
#define ROW 3   // define row selector field

// six bits of color in RRGGBB form. Taken from lightshow page
uint8_t zombie[] = {
    0b110100,
    0b110100,
    0b110100,
    0b000000,
    0b000000,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b010000,
    0b110100,
    0b000000,
    0b000000,
    0b110100,
    0b010000,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b000000,
    0b110100,
    0b111111,
    0b110100,
    0b110100,
    0b111111,
    0b110100,
    0b000000,
    0b000000,
    0b110100,
    0b000000,
    0b110100,
    0b110100,
    0b000000,
    0b110100,
    0b000000,
    0b110100,
    0b110100,
    0b110100,
    0b010000,
    0b010000,
    0b110100,
    0b110100,
    0b110100,
    0b000000,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b000000,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
    0b110100,
};

static const int delay_time = 1;
static const uint32_t clear_signal = 0xFFFFFFFF;

static const size_t MATRIX_SIZE = 8;

/*
  bit_array 64 bytes (1 byte per pixel, 6 bits of which is used)
  hi_bits 32 bytes (4 bytes/row (RRBBGG plus two row bits))
  low_bits 32 bytes 
  both_bits 32 bytes

*/
void convert_bit_array(const uint8_t *bit_array, uint8_t *hi_bits, uint8_t *low_bits, uint8_t *both_bits)
{
  for (size_t i = 0; i < MATRIX_SIZE; i++)
  {
    uint8_t h_red = 0;
    uint8_t l_red = 0;

    uint8_t h_grn = 0;
    uint8_t l_grn = 0;

    uint8_t h_blu = 0;
    uint8_t l_blu = 0;

    for (size_t j = 0; j < MATRIX_SIZE; j++)
    {
      uint8_t bits = bit_array[i << 3 | j];
      h_red = (h_red << 1) | ((bits >> 5) & 1);
      l_red = (l_red << 1) | ((bits >> 4) & 1);
      h_grn = (h_grn << 1) | ((bits >> 3) & 1);
      l_grn = (l_grn << 1) | ((bits >> 2) & 1);
      h_blu = (h_blu << 1) | ((bits >> 1) & 1);
      l_blu = (l_blu << 1) | ((bits >> 0) & 1);
    }
    size_t index = i * 4;

    hi_bits[index + ROW] = 1 << i;
    hi_bits[index + RED] = ~h_red;
    hi_bits[index + BLUE] = ~h_blu;
    hi_bits[index + GREEN] = ~h_grn;

    low_bits[index + ROW] = 1 << i;
    low_bits[index + RED] = ~l_red;
    low_bits[index + BLUE] = ~l_blu;
    low_bits[index + GREEN] = ~l_grn;
  }

  for (size_t i = 0; i < MATRIX_SIZE * 4; i++)
  {
    both_bits[i] = hi_bits[i] | low_bits[i];
  }

  return;
}

void write_buffer(int fd, uint8_t *buf)
{
  for (size_t i = 0; i < MATRIX_SIZE * 4; i += 4) //write each row 
  {
    write(fd, (void *)(buf + i), 4);
  }
  write(fd, (void *)&clear_signal, 4); // turn off, otherwise the last row is "brighter"
  return;
}

void set_bit(uint8_t *buf, int color, size_t x, size_t y)
{
  uint8_t mask = ~(1 << x);
  buf[y * 4 + color] &= mask;
}

void draw_spiral(uint8_t *buf, int color, uint32_t curl)
{
  return;
}

int main(void)
{
  wiringPiSetup();

  // initialize SPI with (channel, clock rate). 0 is channel. clock rate is between 500,000 and 32,000,000
  int fd = wiringPiSPISetup(0, 1000000);
  if (fd == -1)
  {
    perror("wiringPiSetup failed");
    return EXIT_FAILURE;
  }

  static uint8_t hbits[MATRIX_SIZE * 4] = {}; // high bits drawbuffer
  static uint8_t lbits[MATRIX_SIZE * 4] = {}; // low bits drawbuffer
  static uint8_t bbits[MATRIX_SIZE * 4] = {}; // both bits

  convert_bit_array(zombie, hbits, lbits, bbits);

  // static uint8_t *buffers[4] = {lbits, hbits, hbits, bbits};
  static uint8_t *buffers[4] = {lbits, lbits, lbits, lbits};
// //{0x0F, 0x33, 0x55, 0x80};
//   static uint8_t write_test[4] = {0xEF, 0x01, 0xEE, 0x80};

  uint8_t tick = 0;
  while (1)
  {
    // is SPI writes sequential? need to do write(4 bytes), write(4 bytes) instead of write(8 bytes)
    write_buffer(fd, buffers[tick]);
    delay(delay_time);

    tick = (tick + 1) % 4;
  }
}