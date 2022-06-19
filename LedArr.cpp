/* Hay que añadir el instanciador de NeoPixel un update type
*/
#include "LedArr.h"

// 5x5 Font data for: +,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
     static const uint8_t PROGMEM _fnt[150] = {
      1,  28,  64,   0,   0,  32, 128,   7,   0,   0,   0,   0,  64,   8, 136,   3, 148, 165,  57, 132,  33,  28, 225,  17,  14, 112, 140,  39,  41,  78,
     16, 156, 135,   9, 206,  67, 148, 231,   8,  68,  35, 148, 229,  57, 202, 112, 132,   2,   0, 128,   1,   0,  68,   8, 136,  32, 128, 224,  56,   8,
     32, 136, 135,   8, 128,  35, 163, 218, 245, 209, 252,  99, 232, 250,  62, 124,  33,   7, 250,  49, 143, 191,  14,  67, 255, 135,  33,   7, 194, 113,
    124,  99, 248, 199, 228,  33,  63, 242,  18, 140, 140, 185,  40, 194,  16, 135, 227, 186, 198,  49, 205, 103,  23,  70,  49, 119, 163, 232,  65, 209,
    172, 155, 232, 250,  81, 124,  28,  31, 124, 132,  33,  35,  24, 197, 209, 138, 148,  72, 198, 181,  84,  84,  69,  70,  42,  33,   9, 244,  16,  95};

// *************************************** //
// *  Constructors and initiators        * //
// *************************************** //

LedArr::LedArr( int dP):_inited(false),_dataPin(6)
{
  _dataPin=dP;
  
}

LedArr::LedArr(void):_inited(false),_dataPin(6){};

//Initiator... 
// SetsUp memory array & Pixel controller
// this initiator is only for WS2811 16x16 array
void LedArr::InitLedArr(uint8_t dataPin)
{
  _dataPin= dataPin;
   //Serial.println((int)dataPin);
 //_dataPin= dataPin; 
 // pinMode (LedArr::_dataPin,OUTPUT);
   if (InitScr() == SCR_OK) 
    {     
         _inited=true;
       
    }
      pixels.updateType(NEO_GRB + NEO_KHZ800);
      pixels.updateLength(NUM_LEDS);
      pixels.setPin(_dataPin);
      pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
      pixels.setBrightness(BRIGHTNESS);
      randomSeed(7);
      LedString[0].msg[20]="                   ";     LedString[1].msg[20]="                   ";      
      LedString[0].stry=3;                            LedString[1].stry=9;       
      LedString[0].strx=15;                           LedString[1].strx=15;       
      LedString[0].byidx=0;                           LedString[1].byidx=0;      
      LedString[0]. bidx=0;                           LedString[1]. bidx=0;     
      LedString[0].col=1;                             LedString[1].col=1;       
      LedString[0].tmr=80;                            LedString[1].tmr=80;              
}

// private function memmory allocator for virtual screen
uint8_t LedArr:: InitScr()
  {
     _bytes = SCR_SIZE;
     _bits = 4;            
     _error = SCR_NO_MEMORY_ERR;
     
    // Start the Memory SCR 
        _scr = (uint8_t*) malloc(SCR_SIZE);
        
        if (_scr == NULL)
        {
            _error = SCR_NO_MEMORY_ERR;
            return _error;
        }
        return SCR_OK;
  }
// *************************************** //
// * Public methods for handling Screen   //
// **************************************//
// function that will set all the memory arry bits too 0
void  LedArr::ScrClear()     
{
  for (int8_t Y=0; Y<SCRY ; Y++) 
    {
        for (int8_t X=0; X<SCRX; X++)
        {
            SetLedCol(X,Y,0);
        }
    }
}

//String clears de path of the string can be developed further for backcolor efect --ie mask color or overlay
void  LedArr:: ClearLine(uint8_t stry)
{
  for (int8_t Y=0; Y<5 ; Y++) 
    {
        for (int8_t X=0; X<SCRX; X++)
        {
            SetLedCol(X,Y+stry,0);
        }
    }
  }
  
//function that will upload the contents of the memory screen on to the led array
void  LedArr:: RefreshLedArr()
{
     SendLed(0);
     for (uint8_t i = 0; i++ <255 ;)
    {
         SendLed(i);     
    }
    pixels.show();
}
/* function that will get a value from memory arry 4 bit > convert into 32 bit color code > send to the indexed led... 
 * bare in mind: LED 0 of this particular array is at the top left corner && on even row it dicreses from last to first whilest on even row is the opposite:
 *      3   2   1   0
 *      4   5   6   7   like S or 5
 *     11  10   9   8
 *     12  13  14  15
 */
inline void LedArr:: SendLed(uint8_t idx)
{
  struct mRGB Led={0,0,0};
  uint8_t r =0;
        if (((idx/16)%2)==0)
       { r=(((((idx/16)+1)*16)-(idx%16))-1);
          Led = GetColors(GetLedColI(r));
          pixels.setPixelColor(idx, pixels.Color(Led[1], Led[0], Led[2]));
       }
       else
        {
          Led = GetColors(GetLedColI(idx));
           pixels.setPixelColor(idx, pixels.Color(Led[1], Led[0], Led[2]));
        }
        
  }
  
// color table this is calculated for 8 colors 0-black to 7 white 
// if col > 7 then halftones 128
// Hardcoded 8 :RGB(33,33,33)
// 4 bit to 32bit translator. for Led Output.
LedArr::mRGB  LedArr:: GetColors(uint8_t Col) {
  struct mRGB Led = {0,0,0};  
  uint8_t Bri = (Col < 8) ? 255 : 128;
  uint8_t mCol = Col%16; //(Col==15)? 7 : (Col < 8) ? Col : Col -8;
   // ?Col==15 >> mCol=7 : ?Col<8 >> mCol=7 ELS mCol = Col - 8
   if (mCol==8)
   {Led[2]= 33; Led[1]= 33; Led[0]=33;}
   else
   {
      Led[2] = ((mCol%2) * Bri) ;  
      Led[1] = (trunc((mCol-(trunc(mCol/4)*4))/2)) * Bri;
      Led[0] = (trunc (mCol/4)) * Bri;
   }
   return Led;
  }



// *************************************** //
// *  Memory Screen Handelling Methods   * //
// *************************************** //

//basic private functions
// will retrieve a particular bit of our memory array
inline bool LedArr:: _bitget(uint16_t pos)
{
    uint8_t by = pos / 8;
    uint8_t bi = pos & 7;
    uint8_t * p = _scr;
    return (p[by] >> bi) & 0x01; // bitRead(p[by], bi);
}
// function that will set a particular bit from our memory array
// very interesting piece of code that will compare with an and for 0 and or for positive... 
inline void  LedArr:: _bitset(uint16_t pos, uint8_t value)
{
    uint8_t by = pos / 8;
    uint8_t bi = pos & 7;
    uint8_t * p = _scr;
    
    if (value == 0) p[by] &= ~(1 << bi); // bitClear(p[by], bi);
    else p[by] |= (1 << bi);             // bitSet(p[by], bi);
}



//basical that will be used to access our memory array public functions ::: range is 0 to 15 
uint8_t  LedArr:: GetLedCol(const uint8_t X,const uint8_t Y)
{
 uint8_t v = 0;
 uint16_t pos = ((Y * SCRX) + X) * _bits;
   for (uint8_t i = _bits; i-- > 0;)
    {
        v <<= 1;
        v += _bitget(pos + i);
    }
    return v;
}
// Same as previous one but instead of accesing through (x,y) access directly to the memory position. 4 bits * 256 only 1024bits...
uint8_t  LedArr:: GetLedColI(const uint8_t idx)
{
 uint8_t v = 0;
 uint16_t pos = (idx * _bits);
   for (uint8_t i = _bits; i-- > 0;)
    {
        v <<= 1;
        v += _bitget(pos + i);
    }
    return v;
}

// function that will set a position of our memoey array with a specif color
uint8_t  LedArr:: SetLedCol(const uint8_t X,const uint8_t Y, const uint8_t value)
{
    uint16_t pos = ((Y * SCRX) + X) * _bits;
    uint32_t mask = 1UL;
    for (uint8_t i = 0; i < _bits; i++)
    {
        uint8_t v = (value & mask) > 0 ? 1 : 0;
        _bitset(pos + i, v);
        mask <<= 1;
    }
    return value;
}

// function that will set a position of our memoey array with a specif color
uint8_t  LedArr:: SetLedColI(uint8_t pos, const uint8_t value)
{
    uint16_t mpos = pos * _bits;
    uint32_t mask = 1UL;
    for (uint8_t i = 0; i < _bits; i++)
    {
        uint8_t v = (value & mask) > 0 ? 1 : 0;
        _bitset(mpos + i, v);
        mask <<= 1;
    }
    return value;
}

//**********************//
// String definitions  *//
//**********************//
// will return the value of a secifyc pixell in the letter font matrix
// true = 1 & false=0
bool LedArr:: GetChrPix(char lt, uint8_t x, uint8_t y)
{
   if ((int)(lt)==32) // space shoould be trated as 3x5
   {
     return 0; //((y<=0 && y <=5) && ((x<=0 && y <=2)));    
   }
   else 
   {
     uint16_t pos = ((((int(lt)-43) * 5) + y) * 5)+x;
     uint8_t by = pos / 8;
     uint8_t bi = pos & 7;
     uint8_t tbyte = pgm_read_byte(&_fnt[by]);
     return (tbyte >> 7-bi) & 0x01;
   }
}
// will allow to display any 1 character of from the line definition onto the memory array
// very usefull for single sprite animations (scores and such) mindyou only 9 characters can be displayed (if not ovrlayed)
void LedArr:: DisplayChar(char chr, int8_t x, int8_t y, uint8_t col, bool sbbkgrnd)
{
  if ((x > -5) && (y > -5))
  {
    for (int8_t ly=0; ly < 5; ly++)
        { if ((y+ly)>=0)   
           { for (int8_t lx = 0; lx <5 ; lx++)
                { if ((x+lx)>=0)
                   {
                     if (GetChrPix(chr,lx,ly)) 
                      { 
                        SetLedCol((x+lx),(y+ly),col);                      
                      }
                     else {
                            if (sbbkgrnd)
                              {
                                 SetLedCol((x+lx),(y+ly),0);
                              }
                          }
                   }
                }
           }     
        }
    }
}
// overload 
void LedArr:: DisplayChar(char chr, int8_t x, int8_t y, uint8_t col)
 { DisplayChar(chr, x, y, col, true);}

//Will display 1x5 bit line from the current message. for String animation purposes.
void LedArr :: DisplaySnglLine(LedStrStruc  mls)
{
          uint8_t mby = mls.byidx;
          uint8_t mbi = mls.bidx;
          for (uint8_t n = mls.strx; n<16 ;n++) 
            {
               if ((mbi>4) || (((isDigit(mls.msg[mby])))&& (mbi==4)) || ((mbi==1)&&(mls.msg[mby]==' ')))
                   { 
                    
                      mbi=0; mby+=1;
                        for (uint8_t i = 0; i<5; i++)
                         {
                           SetLedCol (n,(i+mls.stry),0); // put empty line down
                         }
                        if (isDigit(mls.msg[mby])) {mbi=1;} // only do not print the presiding empty pixels for numbers
                      
                   }
                 else
                   {
                     for (uint8_t i = 0; i<5; i++)
                       {           
                               if (((mls.msg[mby]-43)>=0) && ((mls.msg[mby]-43)<= 48))  //defined characters range
                               { 
                                 SetLedCol (n,(i+mls.stry),((GetChrPix(mls.msg[mby],mbi,i))? mls.col:0));
                               }
                            
                        }
                     mbi++;
                    }
                 }
  }

  //will load one a String in our predefine arrays
void LedArr::LoadString(int idx, String mstr,int8_t mcol)
{
       for (uint8_t i = 0; i<= (min((mstr.length()),18)) ;i++)
          { LedString[idx].msg[i] = (mstr[i]>42 || mstr[i]<91)? mstr[i] : 0x00;}
          
   LedString[idx].msg[min((mstr.length()+1),19)]=0x00;      
   LedString[idx].col = (mcol%16);
   LedString[idx].byidx=0;
   LedString[idx].bidx=0;
 }

 //Allow the user to change the scroll speed of lines 1-100%
void LedArr::SetScrollSpeed(int idx,int nspd)
 {
    if (idx==0 || idx==1)
    {
      if (nspd<0) {nspd = -nspd;}
      if (nspd>100) {nspd = nspd%101;}
      LedString[idx].tmr = nspd;
      LedString[idx].lstshwn=0;
    }
  }

// function that if called within a loop will display the nessages previously stored in a scroll mode
// you can set the message as well as the color
void LedArr :: DisplayString()
{
  for (int8_t idx=0;idx<2;idx++)
  { 
    if (strlen(LedString[idx].msg) > 0)
    {
      long nticks = micros(); 
      if ( ((nticks - LedString[idx].lstshwn)>((long)(100-LedString[idx].tmr) * 10000))  || (LedString[idx].lstshwn==0))     
      {
        LedString[idx].lstshwn = nticks;
          if (LedString[idx].strx > 0) // start point has room to grow
             { LedString[idx].strx-=1;
               DisplaySnglLine(LedString[idx]);
             }
           else
            {
              LedString[idx].bidx +=1;
              if ((LedString[idx].bidx > 5)||
                   (
                     (
                        (LedString[idx].bidx == 2)
                      &&
                        (LedString[idx].msg[LedString[idx].byidx]==' ')
                      )
                    )
                  )
                  {LedString[idx].bidx = 0; LedString[idx].byidx +=1;}
              if (((LedString[idx].byidx) > (strlen(LedString[idx].msg))) && (LedString[idx].strx==0))
               {LedString[idx].strx = 16;
                LedString[idx].bidx = 0;
                LedString[idx].byidx = 0;
                ClearLine(LedString[idx].stry);
               } 
              DisplaySnglLine(LedString[idx]);
            }
        }
      }
    }
  }