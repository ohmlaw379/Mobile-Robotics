int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

//gradual speed change
int steps = 5; 

//Following Directions to Destinations 
int direction = 0; // 0 = anticlockwise, 1 = clockwise
int junction = 0; //make this into an input in the next stage *****
int count = 0; 

int sensors_between_line = 0; ////    sensors_between_line[] variable remember its last command in case it loses the line
 
int turn[5][4] = {
  {255, 160, 255, 173}, //forward //  {255, 152, 255, 164}, //forward 
  {255, 120, 255, 193}, //inner right
  {255, 15, 255, 255}, //outer right
  {255, 193, 255, 120}, //inner left
  {255, 255, 255, 15} //outer left
};//
int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {10,11,12,13,14}; // keep 8 free for tone O/P music


void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time);
void junction_count();

void setup()
{
 // put your setup code here, to run once:
 Serial.begin(9600);
 analogWrite(motor1Phase, 0); //forward left motor
 analogWrite(motor1PWM, 0); // set speed of left motor
 analogWrite(motor2Phase, 0); //forward right moto
 analogWrite(motor2PWM, 0); // set speed of right motor
 pinMode(10,INPUT);
 pinMode(11,INPUT);
 pinMode(12,INPUT);
 pinMode(13,INPUT);
 pinMode(14,INPUT);
}

void loop()
 {
 // put your main code here, to run repeatedly:
 //Reads value from sensor 
 int i;
 for (i=0;i<5;i++)
 {
  AnalogValue[i]=analogRead(AnalogPin[i]);

  //Serial.print(AnalogValue[i]); // This prints the actual analog reading from the sensors
  //Serial.print("\t"); //tab over on screen
 
  if(i==4)
    {
    //Serial.println(""); //carriage return
    ////delay(120); // display new set of readings every 600mS

    junction_count();
//    sensors_between_line[] variable remember its last command in case it loses the line
//    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);

    //If statements - follow the line
  if(AnalogValue[2]<300){       //forward
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
    sensors_between_line = 0; 
    }
  
  else if(AnalogValue[0]<350){  //outer right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[2][0], turn[2][1], turn[2][2], turn[2][3], 50);
    sensors_between_line = 2; 
    }

  else if(AnalogValue[1] <300){ //inner right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[1][0], turn[1][1], turn[1][2], turn[1][3], 50);
    sensors_between_line = 1; 
    }
    
  else if(AnalogValue[4]<350){ //outer left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[4][0], turn[4][1], turn[4][2], turn[4][3], 50);
    sensors_between_line = 4; 
    }

  else if(AnalogValue[3]<300){ //inner left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[3][0], turn[3][1], turn[3][2], turn[3][3], 50);
    sensors_between_line = 3; 
    }

  // repeat last command if the sensors loose the line
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){ //stop
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
 }
 }
 }
 }


 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time){
  // gradual speed change
  for (int i = 0; i<5; i++){ 
  
  int new_speed_m1 = current_speed_m1 + ((speed_m1 - current_speed_m1) * 5) / steps;
  int new_speed_m2 = current_speed_m2 + ((speed_m2 - current_speed_m2) * 5) / steps;


  analogWrite(motor1Phase, direction_m1); //forward left motor
  analogWrite(motor1PWM, new_speed_m1); // set speed of left motor
  analogWrite(motor2Phase, direction_m2); //forward right moto
  analogWrite(motor2PWM, new_speed_m2); // set speed of right motor

  delay(15);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void stop(){ 
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 0); // set speed of left motor
  analogWrite(motor2Phase, 255); //forward right moto
  analogWrite(motor2PWM, 0); // set speed of right motor
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// counts number of junctions it has passed
void junction_count(){
  //  if (AnalogValue[3]<900 && AnalogValue[2]<900 && AnalogValue[1]<900){
  int total = AnalogValue[4] + AnalogValue[3] + AnalogValue[2] + AnalogValue[1] +  AnalogValue[0];
  Serial.println(total); 
  if (total < 7000){ // if the overall sensor count is less than 7000 then the mobot is passing over a junction
      stop();
      delay(500);
      count++; 
      if (count == 3){
        stop();
        delay(500); 
        count = 0; 
      }
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
    delay(5);
  }
}

