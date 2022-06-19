/*!
 *  @file LedArr_Scr.h
 *
 *  This file uses BitArray Snips, as well as Adafruit.NeoPixel controller to emulate a virtual Screen
 *  to this end will create a 4 bit x 256 position bitarray that will represent 16 colors combination
 *  for a 16x16 WS2811 LED Array (180 bytes). 
 *  
 *  Also it adds an array of (150 bytes) in flash memory with the defitinions of letters and methods to
 *  display  short strings in banners with capital letters and numbers and olso some symbols.
 *  
 *   So the actual size of the array will 180 bytes in memory + controller and font deffinitions in flash 
 *   Should occupy less than 10% of the Arduino memeory!!!
 */

#ifndef LEDARR_H
#define LEDARR_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>


class LedArr{
   public:

    
/// Simplified Adfruit Representation of an RGB pixel (Red, Green, Blue) + Access -> mRGB
 struct mRGB
    {
     union {
             struct {
                      uint8_t r;
                      uint8_t g;
                      uint8_t b;
                     };
              uint8_t raw[3];
            };
      inline uint8_t& operator[] (uint8_t x) __attribute__((always_inline))
            {
                return raw[x];
            }
    };
    
 
  

    // Screen Array definitions
     #define SCR_SIZE               128    // 256 leds * 4 bits per led / 8bits in a byte = 128 bytes
     #define MAXCOL                   8    // from 8 to 15 will use half tones 8 HARD CODED as RGB:33,33,33,33 (dark gray)
     #define SCRX                    16    // row size
     #define SCRY                    16    // row size
     #define SCR_NO_MEMORY_ERR      0x01   //Error de memoria
     #define SCR_OK                 0x00   // todo bien

    
  //LEDS definitions
    //#define LED_PIN                   6   // digital  : Pin de data 
     #define NUM_LEDS                256   
     #define BRIGHTNESS               24   // each led will consume .3W/pixell x 255 = 80W at all white full intensity... for reasonable consumption set at 10% of brigthnes 8W.  

   //#define UPDATES_PER_SECOND      100    for assurance think that will require at least a 40W power supply.

    //Fonts definitions
     #define FNT_Size                 5                   
     #define FNT_COUNT               48
     #define MSG_MAXCHAR             20
     
 int _dataPin;
  
    // Constructor
  LedArr( int dP); // not working dP is not geting the correct value on cpp
  LedArr(void);
  
  // Screen definitions
  void InitLedArr(uint8_t dataPin);         // constructor for LEDScreen will allocate working memory as well as initiate the controller. Will return error code 
  void  ScrClear(void);          // will set all memory pixells to 0;
  void ClearLine(uint8_t stry);  // will clear the String line 5 pixells tall side to side of the array
  uint8_t GetLedCol(const uint8_t X,const uint8_t Y);  //Will return the pixel color in the memmory array
  uint8_t GetLedColI(const uint8_t idx);               // same as above but directly by pixel index
  uint8_t SetLedCol(const uint8_t X,const uint8_t Y, const uint8_t value); // will set a collor on the bitarray 
  uint8_t SetLedColI(const uint8_t pos, const uint8_t value); // will set a collor on the bitarray 
  struct mRGB GetColors(uint8_t Col);    // funcion that will translate the 0-15 bit into a RGB struct
  void RefreshLedArr(void);      // Will force the controller to show the memoryarray color configuration
  void DisplayChar(char chr, int8_t x, int8_t y, uint8_t col); //Sets a character if previously defined on the memory array , solid black bkgrnd
  void DisplayChar(char chr, int8_t x, int8_t y, uint8_t col, bool sbbkgrnd); //Sets a character if previously defined on the memory array solid_back_ground?
  void LoadString(int idx, String mstr,int8_t mcol);    //Allows the user to Set 0 or 1 predefined dispalayable Strings param idx 0-1, mstr : my string, mcol color
  void SetScrollSpeed(int idx,int nspd);  //Allow the user to change the default speed 80%
  void DisplayString();        // if called in a loop will make the effect of animated string so it can be read
 
 protected:
 // Variables
 bool _inited;
 
 // LED STRINGS
   struct LedStrStruc //data structure definition for strings upto 19 displayable characters
    {
   LedStringStruc();
     char msg[MSG_MAXCHAR];      //mdgs data
     int8_t stry;       // y position to display message
     int8_t strx;       // x position to start displaying message (will eventually hold all the LED ROW)
     int8_t byidx;      // first letter displaying on screen (with 5x5 mostly 3 letteres will be displayed or 6 numbers
     int8_t bidx;       // first letters current Row that will be displayed
     int8_t col;        // Color of the string
     long lstshwn;      // last Shown millis
     int tmr;          // used to set speed of text in  1-100% default 50;
   };
   LedStrStruc LedString[2];

 private:
 // variables
    

 Adafruit_NeoPixel pixels;  //controler for the LED 
//MemmoryArray variables
     uint8_t   _bytes;
     uint8_t   _bits;            
     uint8_t * _scr;
     uint8_t   _error;

 // functions
 uint8_t InitScr(void); //Initialize Memory Screen as well as pixels controler.
 //basic private functions
    inline bool _bitget(uint16_t pos);
    inline void _bitset(uint16_t pos, uint8_t value);
    inline void SendLed(uint8_t idx); // function that will translate the 4 bit memory array (0-15) into the apropiate adfruit LED controller as RGB
    bool GetChrPix(char lt, uint8_t x, uint8_t y); // will get a particular pixell from a specific char (from previously defined font)
    void DisplaySnglLine(LedStrStruc  mls); // display a 1 x 5bit line of the LedString 
};
#endif