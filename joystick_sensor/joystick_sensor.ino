
int xPin = A2; // 14
int yPin = A3; // 15

int switchPin = 12;

void setup() {
  Serial.begin(8000);
  // put your setup code here, to run once:  
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  pinMode(switchPin, INPUT_PULLUP);
}

void loop() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);

  int swi = digitalRead(switchPin);

  Serial.print("switch: ");
  Serial.print(swi);

  Serial.print(" -> x: ");
  Serial.print(x);
  Serial.print(" y: ");
  Serial.println(y);
}
