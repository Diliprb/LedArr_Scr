/*!
*   LedArr vertion of popular game break trgough. will require at least a potenciometer input in pin A1/ A2 (left right) and button on 7 or 8 (start resume)
*   all the posibilities of the array (except Display Char) are shown in this example.
*    
*
*
*
*
*
*
*
/*


#include <LedArr.h>
LedArr LA = new LedArr(); //definition without output pin

//PIN DEFINITION
#define WHEEL_ONE_PIN    2   //analog
#define WHEEL_TWO_PIN    1   //analog
#define BUTTON_1_PIN    7   //digital 
#define BUTTON_2_PIN    8   //digital
#define SOUND_PIN       14  //analog

//PONG Definitions
#define PADDLE_HEIGHT    3
#define PADDLE_WIDTH     1
#define RIGHT_PADDLE_X  14 
#define MAX_Y_VELOCITY  0.12


 //controllers variables definitions
boolean buttonOneStatus = 0;
boolean buttonTwoStatus = 0;
int wheelOnePosition; 
int wheelTwoPosition;

//Game Variable Definitions
bool initiated = false;
uint8_t gstate=0;
uint8_t Level=1;    // Difficulty Level
uint8_t Hits=0;
int32_t Score=0;
uint8_t Lifes = 3;
bool StLoaded = false;

unsigned char x,y;
int leftPaddleY;
int leftPaddleY0;
byte ballX;
byte ballY = 13;
byte ballX0 ;
byte ballY0 = 13; 
float ballPX = 20;
float ballPY = 20; 
float ballVx;
float ballVy;
byte scoreL = 0; 
int PaddleHit = 0;  
uint8_t px,py,pc;


void setup() {
  // put your setup code here, to run once:
  LA.InitLedArr(6); // Initializer required for proyect to run.parameter dataPin
}

void PlaySound(int Frequency, int Duration)
 {
  tone(SOUND_PIN, Frequency);  //activa un tono de frecuencia y duracion determinados en un pin dado
  delay(Duration);
  noTone(SOUND_PIN);
 }

 /*
  * controllers actions
  */
inline void ClrStatusBtns() {
      buttonTwoStatus=0;
      buttonOneStatus=0; }
      
bool buttonStatus() {return(buttonOneStatus || buttonTwoStatus);}
      
void processInputs() {
  ClrStatusBtns();
  buttonOneStatus = digitalRead(BUTTON_1_PIN);
  while (digitalRead(BUTTON_1_PIN)); 
  buttonTwoStatus = digitalRead(BUTTON_2_PIN); //|| digitalRead(BUTTON_3_PIN) || digitalRead(BUTTON_4_PIN); 
  while (digitalRead(BUTTON_2_PIN));
  wheelOnePosition = analogRead(WHEEL_ONE_PIN);
  wheelTwoPosition = analogRead(WHEEL_TWO_PIN);
}

 /*
  * BRICK SMASH FUNCTIONS
  */
// GAME SPLASH

void gameSplash()
{
  switch (gstate)
  {
     case 0: 
      { 
         if (!StLoaded)
           { Hits=0;
             Lifes=3;
             LA.LoadString(0,"BRICK SMASH",14);
             LA.LoadString(1,"<< PRESS BTN >>",9);
             StLoaded=true;
           }
           break;
      }
   default:
     { 
       if (!StLoaded)
       {
           LA.ScrClear();
           LA.LoadString(0,":D END - SCR:"+ String(Score,DEC),4);
           LA.LoadString(1,("<< PRESS BTN >>"),9);
           StLoaded=true;
           
       }
     }
  }      
     LA.DisplayString();
     processInputs();
   if (buttonStatus()) {ClrStatusBtns();LA.ScrClear(); StLoaded=false; gstate>5?gstate=0:gstate=1;}
}


//SUPPOR Functions
void ShowLifes()    //will display at bottom left corner 1 lgthed bit per life remainimng
{
  for (uint8_t i=0;i<5;i++)
   {LA.SetLedCol(i,SCRY-1,(Lifes>i?5:0));}
}

// function that will draw the paddle
void drawPaddle(bool force = false) // Brick Smash
{ 
    // ----------------- draw left paddle -------------------------------------------------
    leftPaddleY = ((wheelTwoPosition / 64) * (PADDLE_HEIGHT-1))/ 2 - 1;
    x = RIGHT_PADDLE_X;
    if (leftPaddleY != leftPaddleY0 || PaddleHit > 0 || force){
       LA.SetLedCol(leftPaddleY0,x,0); LA.SetLedCol(leftPaddleY0+1,x,0); if (leftPaddleY0+2<SCRX){LA.SetLedCol(leftPaddleY0+2,x,0);}
       LA.SetLedCol(leftPaddleY,x,4);  LA.SetLedCol(leftPaddleY+1,x,4) ; if (leftPaddleY+2<SCRX){LA.SetLedCol(leftPaddleY+2,x,4);}
       leftPaddleY0=leftPaddleY;
    }
}

void drawBall(bool force = false) // Pong
{ 
   if ((ballX != ballX0) || ((gstate!=1)&&(ballY != ballY0)) || (PaddleHit>0 || force))
   {
      if (ballY0>0) 
        {
              if (!(gstate==1 && ballY0==SCRY-2)){ LA.SetLedCol(ballX0, ballY0, 0);}
              if (px==ballX0 && py==ballY0) { LA.SetLedCol(ballX0, ballY0, pc);px=16;}
              
        }
       LA.SetLedCol(ballX, ballY, 15);
      ballX0=ballX; ballY0=ballY;
    }
}

//function that will calculate the color of each brik line (dificulty) given level and line number
// works fine till level 20... after that colors are all mixed
uint8_t calCol(uint8_t lv, uint8_t l)
{
    if ((lv < 7) && (7 > l+lv))  {return (1);}
    else if (lv==0) {return(1);}
    else
       { 
           if ((l%2)==1)
             {return  (calCol((lv-1),l-1));}
           else
             {return (1+calCol((lv-1),l-1));}
             
       };
}

// function to draw the playing screens level  1 to 20

 void DrawSBScreen(uint8_t lvl)
 {
   LA.ScrClear();
   //draw frame
   for (uint8_t i=0; i<16; i++)
   {
      if (i>0 && i<14) {LA.SetLedCol(0,i,15);}
      if (i>0 && i<13) {LA.SetLedCol(15,i,15);}
      LA.SetLedCol (i,0,15);
    } 

    //draw lines
    uint8_t mcol;
    for (uint8_t i=0; i<6; i++)
       {
        mcol=calCol(lvl+1,i);   // recursive computing eats up less memory than keeping the patterns in memory
          for (uint8_t n=1; n<15; n++)
           {
             LA.SetLedCol(n,(7-i),mcol);
             if (mcol>0) {Hits++;}
           } 
       }
       ShowLifes();
 }


// restart of game after losing one life or when staring from scratch
void waitForStartBS() // BS
 { 
   processInputs();      
   if(Lifes >0){
    ballVx = random(-50., 50.)/500.;  
    ballVy = -.1;
    ballPY=SCRY-3;
    ballPX=leftPaddleY+1;
    if (!initiated)
      {
          ballX0=SCRY-2;
          ballY0=(uint8_t) ballPY;
          DrawSBScreen(Level);
          drawPaddle(true);
          initiated=true;
        }
     drawPaddle();
     ballPX=leftPaddleY+1 + (leftPaddleY+1==0) -(leftPaddleY+1==SCRX-1);
     ballX=(uint8_t) ballPX;
     ballY=(uint8_t) ballPY;
     drawBall();  
    }
    else
       {gstate=6; initiated=false;} //end game
 
    if ((buttonStatus() == 1) && (gstate<5))
    { 
       gstate=2;
    }
   
    LA.RefreshLedArr();
}
/*
 * BALL Releted functions and methods
 */
// As the calculation of remainig brik has to be done on the basis of remaing objet in on the screen
// when the ball reaches it maximum altitud i.e. y=0 will count the remainding bricks? on the screen that are lighted
 inline uint8_t GetHits()
{ uint8_t result=0;
  for (uint8_t i=33; i<128; i++)
   { if (i%16==15){i+=2;}
    result = result+(LA.GetLedColI(i)!=0);
  }
  if ((result<30) && (result%21==0)) {ballVy=-.2;}
  return result;
}

//will perform a bounce if a brick is hit
inline void Bounce(uint8_t x, uint8_t y, uint8_t ncol)
 {
    px=x; py=y; 
    LA.SetLedCol(x,y,15);
    LA.RefreshLedArr();
    delay(25);
    LA.SetLedCol(x,y,ncol);
    ballVy = -(ballVy + (0.01*ballVy));
    ballPX += ballVx;
    ballPY += ballVy;
    ballX = int(ballPX);
    ballY = int(ballPY);        
  }

// Tables of action based on collition brick color
//    color     %smash       %change1    %change2
//      1         100%   
//    2-3          80%      20% col-1
//   4-10          50%      50% col=2  
//  11-14          25%      50% col-6   25% col=2
//noexiste 15    15           10%      50% col-2   40% col=8
void Check4Collition(uint8_t x,uint8_t y)
{
  uint8_t mcol= LA.GetLedCol(x,y);
  switch (mcol)
 {
   case 0: {break;}
   case 1: {Hits --; 
            PlaySound(500+((int)mcol*25),20);
            Score+=1;
            Bounce(x,y,0);
            break;
          }
   case 2 ... 4 : {  
                    if (!y>12)   //hit the paddle
                    {
                       Score= Score + (int) mcol;
                       PlaySound(500+((int)mcol*25),20);
                       if (Score%5<4) 
                          { Hits--;}
                        else 
                         {Bounce(x,y,1);}
                    }
                   break;
                }
    case 5 ... 10 : {  
                 Score= Score + (int) mcol;
                 PlaySound(500+((int)mcol*25),20);
                 if (Score%2==0) 
                   { Hits--;}
                 else 
                 {Bounce(x,y,2);}
                 break;
                }
     case 11 ... 14 : {  
                 Score= Score + (int) mcol;
                 PlaySound(500+((int)pc*mcol),20);
                 if (Hits%2==0) 
                   {Bounce(x,y,mcol-6);}
                 else 
                   {Bounce(x,y,mcol-3);}
                 break;
                }
  default: {break;}
 } 
  if (y=1) {Hits=GetHits();} 
  if (Hits==0) {Level++; gstate=1; initiated=false;}
  drawPaddle(true);
  drawBall(true);
}

/*
 * THE GAME FUNCTION
 */

 void BrickSmash()
 {
   
    float NMAX_Y_VEL = MAX_Y_VELOCITY;

     ballPX += ballVx;
     ballPY += ballVy;
     ballX = int(ballPX);
     ballY = int(ballPY);
     
     //bounce on the sides   
     if(ballPX <= 1 || ballPX >= SCRX-1 ) {
        ballVx = -ballVx;
        ballPX += ballVx;
        ballX = int(ballPX);
        PlaySound(880,30); 
     }; 
     //Bounce on the top
     if(ballPY <= 1) {
        ballVy = -ballVy;
        ballPY += int(ballPY);
        PlaySound(880,30); 
     }; 
     
      processInputs();
      drawPaddle();
       
      Check4Collition(ballX,ballY);
    
      
     // ---------------------------------------- left paddle hit -------------------------------------------------
     if(ballVy > 0 && ballY == RIGHT_PADDLE_X && ballX >= leftPaddleY && ballX <= leftPaddleY + PADDLE_HEIGHT - 1) { 
         ballVy = -(ballVy + (0.03*ballVy));
        if (ballX==leftPaddleY) ballVx=ballVy; //hits the left side of paddle
        else if (ballX==leftPaddleY+2) 
            {
              ballVx=-ballVy;
            } // hits right side of paddle
        else {ballVx=0;} // center of paddle 50% chances of going straigth
        //ballVy = -(ballVy + (0.025*ballVy));
        PlaySound(660, 30);
        PaddleHit = 2; 
        drawPaddle(); 
      }
     
     LA.RefreshLedArr();
     //limit horizontal speed
     
     if(ballVx > NMAX_Y_VEL) ballVx = NMAX_Y_VEL;
     if(ballVx < -NMAX_Y_VEL) ballVx = -NMAX_Y_VEL;
     
     if(ballY >= SCRY-1) { // ------------------------- ball out from bellow ------------------------------------------
         Lifes -=1;
         gstate=1;
         ShowLifes();
         waitForStartBS();
          }
     
      
     //if (gstate == 1) {
     //if (gstate != 2) {
     if (gstate == 1 || gstate == 0) { 
       delay(50);
     }
     
     PaddleHit -= 1; 
     PaddleHit = constrain(PaddleHit, 0, 10); 
 }
 


  
void loop() {
  switch (gstate)
     {
       case 0:
         {gameSplash();break;}
       case 1:
         {
             waitForStartBS();
             break;
         }
        case 2:
         {BrickSmash();break;}  
       case 6:
         {gameSplash();break;}
         case 7:
           {gameSplash();break;} 
       default : break;    
     }
   LA.RefreshLedArr();


}