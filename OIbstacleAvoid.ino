int DistancePin = 16;
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;


void setup() { 
  Serial.begin(9600); 
}

void loop() { 
  int VoltagePWM = analogRead(DistancePin);
  Serial.println(VoltagePWM);

  while(VoltagePWM < 4000){
    analogWrite(motor1Phase, 255); //forward left motor
    analogWrite(motor1PWM, 200); // set speed of left motor
    analogWrite(motor2Phase, 255); //forward right moto
    analogWrite(motor2PWM, 200); // set speed of right motor
  }
  
  stop();
  delay(1000);
  /*
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 200); // set speed of left motor
  analogWrite(motor2Phase, 0); //forward right moto
  analogWrite(motor2PWM, 200); // set speed of right motor
  */

  }
}

void stop(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 0); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 0); // set speed of right motor
}
