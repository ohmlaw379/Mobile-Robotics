int DistancePin = 16;

void setup() { 
  Serial.begin(9600); 
}
void loop() { 
  int VoltagePWM = analogRead(DistancePin);
  Serial.println(VoltagePWM);
  delay(100);
}
