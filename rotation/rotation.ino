
#include "mousemovement.h"
#include <Keyboard.h>

MouseMovement* mm;

// trigger
int xTrigger = A2;
int yTrigger = A3;

// movement/rotatiom
int xMovement = A0; // 14
int yMovement = A1; // 15
int switchPin = 12;

void setup() {
  mm = new MouseMovement();
  Keyboard.begin();
  
  // put your setup code here, to run once:  
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

  if(x < 700){ // a
    Keyboard.press('a');
  }else if(x > 1000){ // d
    Keyboard.press('d');
  }else{
    Keyboard.release('a');
    Keyboard.release('d');
  }

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
}
