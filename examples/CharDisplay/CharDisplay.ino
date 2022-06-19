#include "LedArr.h"
#include <math.h>

LedArr LA = new LedArr(); //definition with output pin
String mst="HELLO WORLD";
uint8_t idx,x,y,col;

void setup() {
  // put your setup code here, to run once:
  LA.InitLedArr(6); // Initializer required for proyect to run.parameter dataPin
  randomSeed(millis());
}

inline void BckGrnd(uint8_t col)
{
   for (uint8_t i=0; i<255; i++)
    { LA.SetLedColI(i,col);
    }
    LA.SetLedColI(255,col);
    LA.RefreshLedArr();
  }



void loop() {

  if (idx==mst.length()) 
   {LA.ScrClear();    
    // BckGrnd(random(8,15));  //change to a random background 
    idx=0;}
  x = random(0,10);
  y = random(0,10);
  col = random(1,15);
   LA.DisplayChar(mst[idx],x,y,col,false); // do not print background
  // LA.DisplayChar(mst[idx],x,y,col);    // solid black background
  idx++;
  delay (400);
  LA.RefreshLedArr();
}