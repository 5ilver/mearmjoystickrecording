#include <Servo.h>

//mearm joystick recording sketch for atmega8
//2017 silver
//recording and playback ripped from Ted Lien's http://www.instructables.com/id/MeArmJoystick-recording/
//Left button click records position
//Right button click plays through recorded positions
//Left button hold will chomp chomp the claw
//Right button hold will restart recording from beginning
    
const int SERVOS = 4;
int PIN[SERVOS] = {11,10,9,5}; //R,L,claw,base
int ANGLE[SERVOS] = {90,90,90,90};
int MIN[SERVOS] = {40,10,0,0};
int MAX[SERVOS] = {160,170,80,170};
int POTPIN[SERVOS] = {A5,A4,A3,A2};
int POTZERO[SERVOS];
int buttonL = A1, buttonR = A0;
int buttonLheld = 0, buttonRheld = 0;
int clickThresh = 4, holdThresh = 50;
Servo servo[SERVOS];
int potdeadzone = 300;
int maxlrdiff = 100;
int minlrdiff = -15;
int maxAutoActions = 100;
int autoAction[100][SERVOS];
int actionIndex = 0;
boolean learnMode = 1;
boolean didmove = 0;

void setup()
  {    
    pinMode(buttonL, INPUT_PULLUP);  //to memorize 
    pinMode(buttonR, INPUT_PULLUP);  //to start actions
    for (int i = 0; i < SERVOS; i++){
      servo[i].attach(PIN[i]);
      servo[i].write(ANGLE[i]);
      pinMode(POTPIN[i],INPUT);
      POTZERO[i]=analogRead(POTPIN[i]);
    }  
  }

void loop()
  {
    delay(20);
    if (!digitalRead(buttonL)) {
      
      if (buttonLheld == holdThresh) {
        //L hold action
        servo[2].write(MIN[2]);
        delay(100);
        servo[2].write(MAX[2]);
        delay(100);
        servo[2].write(MIN[2]);
        delay(100);
        servo[2].write(ANGLE[2]);
        buttonLheld++;
      }
      if (buttonLheld < holdThresh) {
        buttonLheld++;
      }
    }else{
      if (buttonLheld > clickThresh && buttonLheld < holdThresh) {
        //L click action
        
        if (learnMode == 1){
          if (actionIndex < maxAutoActions){
            for(int i = 0; i < SERVOS; i++){
              autoAction[actionIndex][i] = ANGLE[i];
            }
            actionIndex++;  
            if (actionIndex  == maxAutoActions) learnMode = 0;
          }
        }
      }
      buttonLheld=0;
    }
    if (!digitalRead(buttonR)) {

      if (buttonRheld == holdThresh) {
        //R hold action
        actionIndex=0;
        learnMode = 1;
        maxAutoActions = 100;
        buttonRheld++;
      }
      if (buttonRheld < holdThresh) {
        buttonRheld++;
      }
    }else{
      if (buttonRheld > clickThresh && buttonRheld < holdThresh){
       if(learnMode == 1) {
        //R click action
        learnMode = 0;
        maxAutoActions = actionIndex-1 ;
        actionIndex = 0;
       }
        servo[2].write(5);
        delay(500);
        servo[2].write(ANGLE[2]);
                actionIndex++;
      }
      buttonRheld=0;
    }
    
    
    if (learnMode == 1){
      readpots();
    }else{
      playback();
    }
  }  
  
void readpots(){
  for (int i = 0; i < SERVOS; i++){    
    if (analogRead(POTPIN[i]) > POTZERO[i] + potdeadzone) ANGLE[i]=ANGLE[i]+1;
    if (analogRead(POTPIN[i]) < POTZERO[i] - potdeadzone) ANGLE[i]=ANGLE[i]-1;
    moveit(i);
  }
}

void playback(){
  didmove=0;
  if (actionIndex  > maxAutoActions) actionIndex = 0;
  for (int i = 0; i < SERVOS; i++){    
    if ( ANGLE[i] > autoAction[actionIndex][i]) {
      ANGLE[i]--;
      didmove=1;
    }
    if ( ANGLE[i] < autoAction[actionIndex][i]) {
      ANGLE[i]++;
      didmove=1;
    }
  moveit(i);
  }
  if (didmove == 0) {
    actionIndex++;
  }
  
}

void moveit(int i){
  //servo 1 and 2 have a motion relationship and cannot exceed a certain distance between eachothers values  
  //keep in mind they are inverted in orientation    
  if (i == 0 || i == 1) {

    if (ANGLE[1] > 180-ANGLE[0] + maxlrdiff) ANGLE[1] = 180-ANGLE[0] + maxlrdiff;
    if (ANGLE[1] < 180-ANGLE[0] +minlrdiff) ANGLE[1] = 180-ANGLE[0] +minlrdiff;

  }      
  //contrain the min and max servo values
  if (ANGLE[i] > MAX[i]) ANGLE[i] = MAX[i];
  if (ANGLE[i] < MIN[i]) ANGLE[i] = MIN[i];
  servo[i].write(ANGLE[i]);  
}
