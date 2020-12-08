
int xPin = A0; // 14
int yPin = A1; // 15

void setup() {
  Serial.begin(8000);
  // put your setup code here, to run once:
}

void loop() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);

  Serial.print("x: ");
  Serial.print(x);
  Serial.print("y: ");
  Serial.println(y);
}
