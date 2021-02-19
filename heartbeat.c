#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>

#define RED_DATA 0
#define BLUE_DATA 1
#define GREEN_DATA 2

int j;
int x = 2;

static uint8_t data[4] = {0x0, 0x0, 0x0, 0x0};

void heartbig()
{
   static uint8_t heart[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
   for (j = 0; j < 8; j++)
   {
      data[0] = ~heart[j];
      data[2] = 0xFF;
      data[1] = 0xFF;
      data[3] = 0x01 << j;
      wiringPiSPIDataRW(0, data, sizeof(data));
      delay(x);
   }
}

void heartsmall()
{
   static uint8_t heart[8] = {0x00, 0x00, 0x24, 0x7E, 0x7E, 0x3C, 0x18, 0x00};
   for (j = 0; j < 8; j++)
   {
      data[0] = ~heart[j];
      data[2] = 0xFF;
      data[1] = 0xFF;
      data[3] = 0x01 << j;
      wiringPiSPIDataRW(0, data, sizeof(data));
      delay(x);
   }
}

void matrixoff()
{
   static uint8_t heart[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   for (j = 0; j < 8; j++)
   {
      data[0] = ~heart[j];
      data[2] = 0xFF;
      data[1] = 0xFF;
      data[3] = 0x01 << j;
      wiringPiSPIDataRW(0, data, sizeof(data));
      delay(x);
   }
}

int main(void)
{
   wiringPiSetup();
   wiringPiSPISetup(0, 500000);

   while (1)
   {
      int m = 10;
      for (m = 10; m > 0; m--)
      {
         heartbig();
      };
      matrixoff();
      delay(100);
      for (m = 10; m > 0; m--)
      {
         heartsmall();
      };
      matrixoff();
      delay(100);
   }
}