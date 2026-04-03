
/*  
  ELEG2700 Reference Program for Project Development Week 1 Task 2
  Board: Arduino Nano
  Version: 202508
  Output: Servo1 (D6)
  Output: Servo2 (D9)
  Output: 7-segment (D10,D11)
  Output: motors (D3,D2), (D5,D4)
  Input: IR sensor long range (A5)
  Input: IR sensor left (A4)
  Input: IR sensor middle (A3)
  Input: IR sensor right (A2)
*/
#include "ELEG2700.h"
#include <Servo.h>
#define BLK 0
#define WHT 1
// Use the values you tuned in Lab 6
#define RIGHT_POS 180
#define REAR_POS 120
#define LEFT_POS 0
#define CLAMP_OPEN 125
#define CLAMP_RELEASE 40
#define CLAMP_CLOSE 15
#define NORMAL_SPEED 230
#define TURN_SPEED 180
#define LEFT_OFFSET 0
#define RIGHT_OFFSET 12
 
FSMClass FSM1;                            // The given FSM is named 'FSM1'.
TM1637DisplayClass LEDDisplay(D10, D11);  // 7-segment LED Display is connected to D10,D11 for debug
ServoClass Servo1(D6);                    // Servo Motor 1: for Left/Right/Rear position of clamp
ServoClass Servo2(D9);                    // Servo Motor 2: for Open/Close/Release of clamp
SensorClass S1(A5);                       // Long-range IR sensor (for cup detection)
SensorClass S2(A4);                       // Short-range IR sensor
SensorClass S3(A3);                       // Short-range IR sensor
SensorClass S4(A2);                       // Short-range IR sensor
SensorClass S5(A7);                       // Short-range IR sensor
SensorClass S6(A6);                       // Short-range IR sensor
SensorClass S7(A0);                       // Short-range IR sensor
MotorClass MotorR(D3,D2);                 // Right Motor
MotorClass MotorL(D5,D4);                 // Left Motor


// Define total number of BBB joints in one cycle
#define NUM_JOINT 20

// Global variable to count the number of joints reached
int joint = 0;
// Global variable to indicate cup is detected or not
int cup_det = 0;
// Global variable to count the number of cups catch
int cup = 0;
//int c=0;
int white =0;
//===== Basically, no need to modify setup() and loop() ====
void setup()
{
  Serial.begin(115200);
  LEDDisplay.setBrightness(15);
  FSM1.init(S_999);             // must have this line, you can change the first state of the FSM
  Servo1.init();
  Servo2.init();
}

void loop()
{
    FSM1.run();     // Important, don't modify this line              
}
//================= Students add STATES below this line ====================
//----------start of state S_999 -----
void S_999()
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(0);
    MotorL.setSpeed(0);  
    Servo1.setValue(LEFT_POS);
    Servo2.setValue(CLAMP_OPEN);
    LEDDisplay.setValue((white*1000)+999);
  }
  if (FSM1.getTime() > 1500) FSM1.transit(S_101);      
}
//----------start of state S_998 -----
void S_998()  
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(0);
    MotorL.setSpeed(0);  
    Servo1.setValue(RIGHT_POS);
    Servo2.setValue(CLAMP_OPEN);
    LEDDisplay.setValue((white*1000)+998);
  }
  if (FSM1.getTime() > 1500) FSM1.transit(S_101);      
}
//------------------------------------
//------------------------------------

void S_101()  
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue((joint*1000)+101);
  }
  if (S2.getHiLow()== WHT && S4.getHiLow()== WHT) FSM1.transit(S_102);    // Car is at the center of the black track -> keeps going straight forward
  if (S2.getHiLow()== WHT && S4.getHiLow()== BLK) FSM1.transit(S_103);    // Car is left-biased -> turn right
  if (S2.getHiLow()== BLK && S4.getHiLow()== WHT) FSM1.transit(S_104);    // Car is right-biased -> turn left
  //if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_201);    // Car reach the joint point
  //if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK) FSM1.transit(S_201);
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (S2.getHiLow()== BLK && S3.getHiLow()== BLK && S4.getHiLow()== BLK){
    if(cup>=10&&cup<=13){joint=joint+1;}
    if(cup>=10&&cup<=12&&joint!=5){FSM1.transit(S_202);}
    else if(cup==12&&joint==5){FSM1.transit(S_203);}
    //if(joint=6){FSM1.transit(S_102);}
    else{FSM1.transit(S_201);}
  }
}

//------------------------------------
void S_102()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward
    LEDDisplay.setValue((white*1000)+102);
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);  
  }
  //if (!(S2.getHiLow()== WHT && S4.getHiLow()== WHT)) FSM1.transit(S_101);  
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S1.getHiLow()== BLK && cup_det==0))  FSM1.transit(S_101);
 // if(joint==6&&S2.getHiLow()== WHT && S4.getHiLow()== WHT) FSM1.transit(S_101);
}
//------------------------------------
void S_103()  
{
  if(FSM1.doTask())
  {
    // Car turn right
    LEDDisplay.setValue((white*1000)+103);
    // Add action statement here!  
    MotorR.setSpeed(0);
    MotorL.setSpeed(TURN_SPEED);
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S2.getHiLow()== WHT && S4.getHiLow()== BLK)) FSM1.transit(S_101);  
}
//------------------------------------
void S_104()  
{
  if(FSM1.doTask())
  {
    // Car turn left
    LEDDisplay.setValue((white*1000)+104);
    // Add action statement here!
    MotorR.setSpeed(TURN_SPEED);
    MotorL.setSpeed(0);
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (!(S2.getHiLow()== BLK && S4.getHiLow()== WHT)) FSM1.transit(S_101);  
 
}
//------------------------------------
//------------------------------------
void S_201()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second
    LEDDisplay.setValue((white*1000)+201);
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(NORMAL_SPEED-LEFT_OFFSET);
    // The 1st BBB Joint reached
    //joint = joint +1;//has joint, not the same with 102
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (FSM1.getTime() > 30) FSM1.transit(S_101);
}
//------------------------------------
void S_202()  
{
  if(FSM1.doTask())
  {
    // Car turns left/right
    LEDDisplay.setValue((white*1000)+202);
    // Add action statement here!
    MotorR.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    MotorL.setSpeed(-85);
    //joint=joint+1;
  }
  if (S2.getHiLow()== WHT && FSM1.getTime() > 450) FSM1.transit(S_101);
  //if (S2.getHiLow()== WHT) FSM1.transit(S_101);
}
//------------------------------------
void S_203()  
{
  if(FSM1.doTask())
  {
    // Car turns left/right
    LEDDisplay.setValue((white*1000)+203);
    // Add action statement here!
    MotorR.setSpeed(-85);
    MotorL.setSpeed(NORMAL_SPEED-RIGHT_OFFSET);
    //joint=joint+1;
  }
 
  if (S4.getHiLow()== WHT && FSM1.getTime() > 250) FSM1.transit(S_101);
  //if (S2.getHiLow()== WHT) FSM1.transit(S_101);
}
//------------------------------------
void S_204()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second
    LEDDisplay.setValue((joint*1000)+204);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));
    // The 1st BBB Joint reached
    //joint = joint+ 1;//has joint, not the same with 102
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (FSM1.getTime() > 300)FSM1.transit(S_301);
 
}
//------------------------------------
void S_205()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second
    LEDDisplay.setValue((white*1000)+205);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(85);
    // The 1st BBB Joint reached
    //joint = joint+ 1;//has joint, not the same with 102
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (S5.getHiLow()== WHT && FSM1.getTime() > 250)FSM1.transit(S_301);
 
 
}
//------------------------------------
void S_206()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second
    LEDDisplay.setValue((white*1000)+206);
    MotorR.setSpeed(85);
    MotorL.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    // The 1st BBB Joint reached
    //joint = joint+ 1;//has joint, not the same with 102
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (S7.getHiLow()== WHT && FSM1.getTime() > 250)FSM1.transit(S_301);
 
 
}
//------------------------------------
//------------------------------------
void S_301()  
{
  if(FSM1.doTask())
  {
    LEDDisplay.setValue((joint*1000)+301);
  }
 
  if (S5.getHiLow()== BLK && S6.getHiLow()== BLK && S7.getHiLow()== BLK){
    if(cup>=10&&cup<=13){joint=joint+1;}
    if(cup>10&&cup<=12&&joint!=6){FSM1.transit(S_205);}
    else if(cup==13&&joint==6){FSM1.transit(S_206);}
    //if(joint==6){FSM1.transit(S_302);}
    else{FSM1.transit(S_204);}}// 204(move backward when arrive the joint)




  if (S5.getHiLow()== WHT && S7.getHiLow()== WHT) FSM1.transit(S_333);  // Car is at the center of the black track -> keeps going straight forward
  if (S5.getHiLow()== WHT && S7.getHiLow()== BLK) FSM1.transit(S_303);    // move Left
  if (S5.getHiLow()== BLK && S7.getHiLow()== WHT) FSM1.transit(S_304);    // move R
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
 
}
//------------------------------------
void S_333()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward
    LEDDisplay.setValue((white*1000)+333);
    MotorR.setSpeed(-(180));
    MotorL.setSpeed(-(180));  
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (FSM1.getTime() > 100)  FSM1.transit(S_302);
}
//------------------------------------
//------------------------------------
void S_302()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward
    LEDDisplay.setValue((white*1000)+302);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));  
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (S5.getHiLow()== WHT &&S6.getHiLow()== WHT&& S7.getHiLow()== WHT) FSM1.transit(S_111);
  if (! (S5.getHiLow()== WHT && S7.getHiLow()== WHT))  FSM1.transit(S_301);
  //if(joint==6&&S5.getHiLow()== WHT && S7.getHiLow()== WHT) FSM1.transit(S_301);
}
//------------------------------------
void S_111() //count write  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward for 0.1 second
    LEDDisplay.setValue((white*1000)+111);
    //MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    //MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));
    // The 1st BBB Joint reached
    white = white+ 1;//
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (S5.getHiLow()== WHT &&S6.getHiLow()== WHT&& S7.getHiLow()== WHT&&(white==1||white==2||white==4||white==6||white==8||white==10||white==13||white==16)) FSM1.transit(S_000);
  else {FSM1.transit(S_322);}
 
}
//------------------------------------
//------------------------------------
void S_322()  
{
  if(FSM1.doTask())
  {
    // Car goes straight forward
    LEDDisplay.setValue((white*1000)+322);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));  
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (! (S5.getHiLow()== WHT && S7.getHiLow()== WHT))  FSM1.transit(S_301);
  if(S5.getHiLow()== WHT &&S6.getHiLow()== WHT&& S7.getHiLow()== WHT&&cup>10&&cup<=13) FSM1.transit(S_000);
}
//------------------------------------
//------------------------------------
void S_303()  
{
  if(FSM1.doTask())
  {
    // Car turn right
    LEDDisplay.setValue((white*1000)+303);
    // Add action statement here!  
    MotorR.setSpeed(0);
    MotorL.setSpeed(-TURN_SPEED);
  }
 
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (/*FSM1.getTime() >30*/!(S5.getHiLow()== WHT && S7.getHiLow()== BLK)) FSM1.transit(S_301);  
}
//------------------------------------
void S_304()  
{
  if(FSM1.doTask())
  {
    // Car turn left
    LEDDisplay.setValue((white*1000)+304);
    // Add action statement here!
    MotorR.setSpeed(-TURN_SPEED);
    MotorL.setSpeed(0);
  }
  if ((S1.getHiLow()== BLK) && (cup_det==0))  FSM1.transit(S_401);
  if (/*FSM1.getTime() >30*/!(S5.getHiLow()== BLK && S7.getHiLow()== WHT)) FSM1.transit(S_301);  
}
//------------------------------------
void S_401()
{
  if(FSM1.doTask())
  {
    // Stop the car
    LEDDisplay.setValue((white*1000)+401);
    MotorR.setSpeed(0);
    MotorL.setSpeed(0);
    cup = cup + 1;
    cup_det = 1;
  }
  if (FSM1.getTime() > 500) FSM1.transit(S_402);
}
//------------------------------------
void S_402()  
{
  if(FSM1.doTask())
  {
    // Catch the cup
    LEDDisplay.setValue((white*1000)+402);
    Servo2.setValue(CLAMP_CLOSE);
  }
  if (FSM1.getTime() > 500&&(cup>=3&&cup<=13)) FSM1.transit(S_404);//404 (clamp backward)->405 (move backward)->000(clamp release)
  if (FSM1.getTime() > 500&&(cup==1||cup==2 )) FSM1.transit(S_403); // 403 (clamp backward) ->000(clamp release)
}
//------------------------------------
void S_403()  
{
  if(FSM1.doTask())
  {
    // Move the clamp to the back
    LEDDisplay.setValue((white*1000)+403);
    Servo1.setValue(90);
    MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
    MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET));
   
  }
  if (FSM1.getTime() >550) FSM1.transit(S_000);
}
//------------------------------------
void S_404()  
{
  if(FSM1.doTask())
  {
    // clamp Move backward
    LEDDisplay.setValue((white*1000)+404);
    if(cup==3||cup==4||cup==7||cup==8||cup==9||cup==10){
      MotorR.setSpeed(-(NORMAL_SPEED-RIGHT_OFFSET));
      MotorL.setSpeed(-(NORMAL_SPEED-LEFT_OFFSET)); }
    if(cup==5){
      MotorR.setSpeed(150);
      MotorL.setSpeed(0);}

    Servo1.setValue(90);
   
  }
 
  if (cup==5&&FSM1.getTime() >200) FSM1.transit(S_301);
  if (FSM1.getTime() >300) FSM1.transit(S_301);
}
//------------------------------------

void S_000()  
{
  if(FSM1.doTask())
  {
    // release the cup
    LEDDisplay.setValue((white*1000)+000);
    Servo2.setValue(CLAMP_RELEASE);
    cup_det=0;
   
  }
  if (FSM1.getTime() >100&&cup==2||cup==4 ||cup== 6||cup== 8) FSM1.transit(S_999);
  if (FSM1.getTime() >100&&(cup==1 ||cup== 3||cup== 5||cup== 7||cup== 9||cup==12 )) FSM1.transit(S_998);
  if (FSM1.getTime() >100&&(cup>=10 &&cup<=11))FSM1.transit(S_999);

}





























