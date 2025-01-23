int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

void setup() { 
  Serial.begin(9600); 
}

void loop() {
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 200); // set speed of left motor
  analogWrite(motor2Phase, 255); //forward right moto
  analogWrite(motor2PWM, 200); // set speed of right motor
  
  stop();
  delay(1000);
  // turn 180 degrees
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 200); // set speed of left motor
  analogWrite(motor2Phase, 0); //reverse right motor
  analogWrite(motor2PWM, 200); // set speed of right motor
  delay(500);
  stop();
}

void stop(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 0); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 0); // set speed of right motor
}
