/*
 * Velostat Measurement
 * 8 kOhm unpressed
 * 1 kOhm pressed
 */

int velostatPin = A1;
int velostatValue = 0;
int threshold = 255;

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("Velostat Reading");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  velostatValue = analogRead(velostatPin);
  Serial.println(velostatValue);
  delay(1000);
  //if (velostatValue > threshold){ Serial.println("button pressed");}
}
