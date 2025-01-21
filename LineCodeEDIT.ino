int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;
//
int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {4,5,6,7,15}; // keep 8 free for tone O/P music
void setup()
{
 // put your setup code here, to run once:
 Serial.begin(9600);
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 0); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 0); // set speed of right motor
}

void loop()
 {
 // put your main code here, to run repeatedly:
 //Reads value from sensor 
 int i;
 for (i=0;i<5;i++)
 {
  AnalogValue[i]=analogRead(AnalogPin[i]);

  Serial.print(AnalogValue[i]); // This prints the actual analog reading from the sensors
  Serial.print("\t"); //tab over on screen
  if(i==4)
    {
    Serial.println(""); //carriage return
    delay(100); // display new set of readings every 600mS

    //If statements - follow the line
  if(AnalogValue[2]<350){
    forward();
    }

  else if(AnalogValue[1] <350){
    inner_left();
    }
  
  else if(AnalogValue[0]<350){
    outer_left();
    }

  else if(AnalogValue[3]<350){
    inner_right();
    }

  else if(AnalogValue[4]<350){
    outer_right();
    }
  
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){
    stop();
  }

 }
 } 
 }
void forward(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 100); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 100); // set speed of right motor
}

void inner_left(){
 analogWrite(motor1Phase, 0); //forward left motor
 analogWrite(motor1PWM, 50); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 50); // set speed of right motor
}

void outer_left(){
 analogWrite(motor1Phase, 0); //forward left motor
 analogWrite(motor1PWM, 120); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 120); // set speed of right motor
}

void inner_right(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 50); // set speed of left motor
 analogWrite(motor2Phase, 0); //forward right moto
 analogWrite(motor2PWM, 50); // set speed of right motor
}

void outer_right(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 120); // set speed of left motor
 analogWrite(motor2Phase, 0); //forward right moto
 analogWrite(motor2PWM, 120); // set speed of right motor
}

void stop(){
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 0); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 0); // set speed of right motor
}


