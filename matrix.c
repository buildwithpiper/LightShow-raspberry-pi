
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

//these really should be enums but eh
#define RED 0   //define the red data source
#define BLUE 1  //define the blue data source
#define GREEN 2 // define the green data source
#define ROW 3   // define row selector field

const uint8_t zombie[64] = {
0b000100,0b000100,0b000100,0b000100,0b000100,0b000100,0b000100,0b000100,
0b000100,0b000100,0b000100,0b000100,0b001000,0b001000,0b000100,0b000100,
0b000100,0b001000,0b001000,0b001000,0b001000,0b001000,0b001000,0b000100,
0b001000,0b001000,0b001000,0b001000,0b001000,0b001000,0b001000,0b000100,
0b001000,0b000000,0b000000,0b001000,0b001000,0b000000,0b000000,0b001000,
0b000100,0b001000,0b001000,0b000000,0b000000,0b001000,0b001000,0b000100,
0b000100,0b001000,0b000000,0b001000,0b001000,0b000000,0b001000,0b000100,
0b000100,0b000100,0b000100,0b000100,0b000100,0b000100,0b000100,0b000100};

const uint8_t piperbot[64] = {
0b000001,0b000001,0b000001,0b000001,0b000001,0b000001,0b000001,0b000001,
0b000001,0b011101,0b011101,0b011101,0b011101,0b011101,0b011101,0b000001,
0b000001,0b011101,0b111111,0b011101,0b011101,0b111111,0b011101,0b000001,
0b000001,0b011101,0b000000,0b011101,0b011101,0b000000,0b011101,0b000001,
0b000001,0b011101,0b111111,0b011101,0b011101,0b111111,0b011101,0b000001,
0b000001,0b011101,0b011101,0b011101,0b011101,0b011101,0b011101,0b000001,
0b000001,0b011101,0b011101,0b000000,0b000000,0b011101,0b011101,0b000001,
0b000001,0b000001,0b000001,0b000001,0b000001,0b000001,0b000001,0b000001};

const uint8_t pip_mouse[64] = {
0b110100,0b110100,0b110100,0b000000,0b000000,0b110100,0b110100,0b110100,
0b110100,0b010000,0b110100,0b000000,0b000000,0b110100,0b010000,0b110100,
0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,
0b000000,0b110100,0b111111,0b110100,0b110100,0b111111,0b110100,0b000000,
0b000000,0b110100,0b000000,0b110100,0b110100,0b000000,0b110100,0b000000,
0b110100,0b110100,0b110100,0b010000,0b010000,0b110100,0b110100,0b110100,
0b000000,0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,0b000000,
0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,0b110100,0b110100};

//change this to "pip_mouse", "zombie", or "piperbot"
const uint8_t* imageData = pip_mouse;

static const int delay_time = 1;
static const uint32_t clear_signal = 0xFFFFFFFF;

static const size_t MATRIX_SIZE = 8;
static const size_t BYTES_PER_ROW = 4;

/*
  takes a uint_8[64] and converts it into easier-to-parse format for display 
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
    size_t index = i * BYTES_PER_ROW;

    hi_bits[index + ROW] = 1 << i;
    hi_bits[index + RED] = ~h_red;
    hi_bits[index + BLUE] = ~h_blu;
    hi_bits[index + GREEN] = ~h_grn;

    low_bits[index + ROW] = 1 << i;
    low_bits[index + RED] = ~l_red;
    low_bits[index + BLUE] = ~l_blu;
    low_bits[index + GREEN] = ~l_grn;
  }

  for (size_t i = 0; i < MATRIX_SIZE * BYTES_PER_ROW; i++)
  {
    both_bits[i] = hi_bits[i] | low_bits[i];
  }

  return;
}

void write_buffer(int fd, uint8_t *buf)
{
  static uint8_t data[BYTES_PER_ROW] = {0xFF, 0xFF, 0xFF, 0xFF};
  for (size_t row = 0; row < MATRIX_SIZE * BYTES_PER_ROW; row += BYTES_PER_ROW)
  {
    data[ROW] = buf[row + ROW];
    // need to flash red/green/blue components separately - don't ask me why
    for (size_t color = 0; color < 3; color++)
    {
      data[color] = buf[row + color];
      write(fd, data, BYTES_PER_ROW);
      data[color] = (uint8_t)0xFF;
    }
  }
  // turn off, otherwise the last row is on for longer
  write(fd, (void *)&clear_signal, BYTES_PER_ROW);
  return;
}

int main(void)
{
  wiringPiSetup();

  // initialize SPI with (channel, clock rate).
  //0 is channel. clock rate is between 500,000 and 32,000,000
  int fd = wiringPiSPISetup(0, 1000000);
  if (fd == -1)
  {
    perror("wiringPiSetup failed");
    return EXIT_FAILURE;
  }

  // drawbuffers
  static uint8_t hbits[MATRIX_SIZE * BYTES_PER_ROW] = {}; // high bits
  static uint8_t lbits[MATRIX_SIZE * BYTES_PER_ROW] = {}; // low bits
  static uint8_t bbits[MATRIX_SIZE * BYTES_PER_ROW] = {}; // both bits

  convert_bit_array(imageData, hbits, lbits, bbits);

  uint8_t *buffers[4] = {lbits, hbits, hbits, bbits};

  uint8_t tick = 0;
  while (1)
  {
    write_buffer(fd, buffers[tick]);
    delay(delay_time);

    tick = (tick + 1) % 4;
  }
}