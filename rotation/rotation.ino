
#include "mousemovement.h"
#include "soundsensor.h"
#include <Keyboard.h>

MouseMovement* mm;
SoundSensor* ss;

// reboot pin
int rebootPin = 2;

// reload pin
int reloadPin = 11;

// audio pin
int audio = 21;

// trigger pins
int xTrigger = A2;
int yTrigger = A3;

// movement/rotatiom pins
int xMovement = A0; // 14
int yMovement = A1; // 15
int switchPin = 12;

void setup() {
  mm = new MouseMovement();
  ss = new SoundSensor(200, 21);
  
  Keyboard.begin();
  
  // put your setup code here, to run once:  
  pinMode(rebootPin, INPUT);
  pinMode(reloadPin, INPUT);
  pinMode(xTrigger, INPUT);
  pinMode(yTrigger, INPUT);
  pinMode(xMovement, INPUT);
  pinMode(yMovement, INPUT);

  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  mm->update();

  int x = analogRead(xMovement);
  int y = analogRead(yMovement);

  // movement a/d
  if(x < 700){ // a
    Keyboard.press('a');
  }else if(x > 1000){ // d
    Keyboard.press('d');
  }else{
    Keyboard.release('a');
    Keyboard.release('d');
  }

  // movement s/w
  if(y < 500){ // s
    Keyboard.press('s');
  }else if(y > 1000){ // w
    Keyboard.press('w');
  }else{
    Keyboard.release('s');
    Keyboard.release('w');
  }

  int swi = digitalRead(switchPin);

  Serial.print(" movement { switch: ");
  Serial.print(swi);

  Serial.print(" , x: ");
  Serial.print(x);
  Serial.print(" , y: ");
  Serial.print(y);
  Serial.print(" } ");

  x = analogRead(xTrigger);
  y = analogRead(yTrigger);
  
  if(x > 1000){ // trigger press
    Mouse.press(MOUSE_LEFT);
  }else{
    Mouse.release(MOUSE_LEFT);  
  }

  Serial.print(" trigger { x ");
  Serial.print(x);
  Serial.print(" , y: ");
  Serial.print(y);
  Serial.println(" }");

  // reload r
  if(reloadPin == LOW){
    Keyboard.press('r');
  }else{
    Keyboard.release('r');
  }
}
