int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

int direction_m1 = 0; 
int speed_m1 = 0; 
int direction_m2 = 0; 
int speed_m2 = 0; 
//
int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {4,5,6,7,15}; // keep 8 free for tone O/P music

void drive(int direction_m1, int speed_m1, int direction_m2, int speed_m2);

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
  if(AnalogValue[2]<350){       //forward
    drive(255, 110, 255, 110);
    }

  else if(AnalogValue[1] <350){ //inner left
    drive(0, 0, 255, 60);
    }
  
  else if(AnalogValue[0]<350){  //outer left
    drive(0, 0, 255, 80);
    }

  else if(AnalogValue[3]<350){ //inner right
    drive(255, 60, 0, 0);
    }

  else if(AnalogValue[4]<350){ //outer right
    drive(255, 80, 0, 30);
    }
  
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){ //stop
    drive(0,0,0,0);
  }
 }
 }
 }

 
void drive(int direction_m1, int speed_m1, int direction_m2, int speed_m2){
  analogWrite(motor1Phase, direction_m1); //forward left motor
  analogWrite(motor1PWM, speed_m1); // set speed of left motor
  analogWrite(motor2Phase, direction_m2); //forward right moto
  analogWrite(motor2PWM, speed_m2); // set speed of right motor
 }

