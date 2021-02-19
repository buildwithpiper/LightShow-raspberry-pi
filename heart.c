// from https://wiki.52pi.com/index.php/RPI-RGB-LED-Matrix_SKU:EP-0043#How_to_use_it
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>

#define RED_DATA 0   //define the red data source
#define BLUE_DATA 1  //define the blue data source
#define GREEN_DATA 2 // define the green data source

int main(void)
{
  static uint8_t data[4] = {0x0, 0x0, 0x0, 0x0}; // initialize RGB data source

  wiringPiSetup();             // initialize wiringPi
  wiringPiSPISetup(0, 500000); // initialize SPI  information, 0 is channel 0, 500000 is clock rate.
  while (1)
  {
    int j;
    int x = 2;
    static uint8_t heart[8] =
        {
            0b00000000,
            0b01100110,
            0b11111111,
            0b11111111,
            0b11111111,
            0b01111110,
            0b00111100,
            0b00011000}; // this is a array of heart
    for (j = 0; j < 8; j++)
    {
      data[0] = ~heart[j];
      data[2] = 0xFF;
      data[1] = 0xFF;
      data[3] = 0x01 << j;
      wiringPiSPIDataRW(0, data, sizeof(data)); // send data to SPI channel 0, and the length of the data
      delay(x);
    }
  }
}