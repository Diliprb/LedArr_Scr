#include "LedArr.h"

LedArr LA = new LedArr(); //definition with output pin

void setup() {
  // put your setup code here, to run once:
  LA.InitLedArr(6); // Initializer required for proyect to run.parameter dataPin
  LA.LoadString(0,"DILIP RB",14);
  
  LA.LoadString(1,"IS HERE", 9);
}
bool ScreenCleared = false;

void loop() {
  if (!ScreenCleared) {ScreenCleared=true; LA.ScrClear();}
  LA.DisplayString();
  LA.RefreshLedArr();
}