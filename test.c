
#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//these really should be enums but eh
#define RED 0   //define the red data source
#define BLUE 1  //define the blue data source
#define GREEN 2 // define the green data source
#define ROW 3   // define row selector field

static const int delay_time = 1;
static const uint32_t clear_signal = 0xFFFFFFFF;

static const size_t MATRIX_SIZE = 8;

int main(int argc, char **argv)
{
  wiringPiSetup();

  // initialize SPI with (channel, clock rate). 0 is channel. clock rate is between 500,000 and 32,000,000
  int fd = wiringPiSPISetup(0, 1000000);
  if (fd == -1)
  {
    perror("wiringPiSetup failed");
    return EXIT_FAILURE;
  }

  static uint8_t data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  if (argc >= 2 && !strcmp("off", argv[1]))
  {
    printf("Turning off LED matrix\n");
    write(fd, data, 4);
    return EXIT_SUCCESS;
  }

  uint8_t on = 0x00;
  uint8_t off = 0xFF;
  while (1)
  {
    for (size_t row = 0; row < MATRIX_SIZE; row++)
    {
      data[ROW] = 1 << row;
      uint8_t bits = (uint8_t)row;
      for (size_t i = 0; i < 3; i++)
      {
        data[RED] = off;
        data[GREEN] = off;
        data[BLUE] = off;

        data[i] = bits & 1 ? on : off;
        write(fd, data, 4);
        bits >>= 1;
      }
    }
    write(fd, &clear_signal, 4);
    delay(delay_time);
  }
}