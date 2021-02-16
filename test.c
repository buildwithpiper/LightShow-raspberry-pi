
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

static const int delay_time = 1;
static const uint32_t clear_signal = 0xFFFFFFFF;

static const size_t MATRIX_SIZE = 8;
/*
TODO: figure out if R- -G is the same as red AND green at the same time, then one after another
*/
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

  // static uint8_t *buffers[4] = {lbits, hbits, hbits, bbits};
// //{0x0F, 0x33, 0x55, 0x80};
  static uint8_t test[4] = {0xEF, 0x01, 0xEE, 0x80};

  uint8_t tick = 0;
  while (1)
  {
    // is SPI writes sequential? need to do write(4 bytes), write(4 bytes) instead of write(8 bytes)
    write(fd, test, 4);
    delay(delay_time);

    tick = (tick + 1) % 4;
  }
}