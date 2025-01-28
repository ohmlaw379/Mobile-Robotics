// TEST 180 DEGREE TURN
// direction from 1 - add extra options? have a middle ground function before drive junctions? 

// hardcode, matrix, nodes
// innovation
//someone - take the wifi/server code, integrate it into this code

int Junctions[10]; // Array to store integers
int arrayLength = 0;
int junctions = 0; 

int current_junction = 0;
int next_junction = 0;

int ninety = 0; 
int old_direction = 0; //0 = clockwise, 1= anticlockwise 
String old_mid_direction = "left"; //0 = left, 1 = right
int count = 0; // counts home many junctions it has passed
int i = 0; 
int go = 1; 


//Motors 
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

//Speeds
int steps = 5; //gradual speed change
int turn[5][4] = {
  {255, 150, 255, 150}, //forward 
  {255, 50, 255, 130}, //inner left
  {255, 30, 255, 130}, //outer left
  {255, 130, 255, 50}, //inner right
  {255, 130, 255, 30} // outer right
};
int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {4,5,6,7,15}; // keep 8 free for tone O/P music
int sensors_between_line = 0; 
int a = 0;


// functions
void follow_the_line();
void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time);
void parking();
void stop();
void drive_to_junction(String mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park);
void ZeroJunction();
void OneJunction();
void TwoJunction();
void ThreeJunction();
void FourJunction();
void FiveJunction();
void junction_1_directions(String mid_direction, int direction, String direction_of_ninety, int park);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
    Serial.begin(9600);
    analogWrite(motor1Phase, 255); //forward left motor
    analogWrite(motor1PWM, 0); // set speed of left motor
    analogWrite(motor2Phase, 255); //forward right moto
    analogWrite(motor2PWM, 0); // set speed of right motor

    Junctions[0] = 0;
    Junctions[1] = 1;
    Junctions[2] = 2;
    Junctions[3] = 5;
    current_junction = Junctions[a];
    a++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
//next junction
if( go == 1){

switch (Junctions[a]){
case 0:
    ZeroJunction();
    break;
  
  case 1:
    OneJunction();
    break;
    
  case 2:
  TwoJunction();
    break;
    
  case 3:
  ThreeJunction();
    break;
    
  case 4:
  FourJunction();
    break;
    
  case 5:
  FiveJunction();
    break;
 
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
    //delay(120); // display new set of readings every 600mS

  follow_the_line();

  }
 }
 }
 }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//int mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

// going to junction 0
void ZeroJunction(){
  //past junction
  switch (current_junction){

  case 0:
    break;

  case 1:
  //(string mid_direction, direction, direction_of_ninety, park)
    junction_1_directions("right", 0, "right", 0);

    break;
    
  case 2:
    drive_to_junction("left", 0, 2, 0, 0, "n/a", 0);  
    break;
    
  case 3:
    drive_to_junction("left", 0, 3, 0, 0, "n/a", 0); 
    break;
    
  case 4:
    drive_to_junction("left", 1, 1, 0, 0, "n/a", 0);  
    break;
    
 }
 current_junction = 0; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
//string mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

//going to junction one
void OneJunction(){
  switch (current_junction){
  //coming from junction zero
  case 0:
    drive_to_junction("left", 1, 2, 1, 1, "left", 0);  // 90 degree turn @ annoying line
    break;
  
  case 1:
    break;
    
  case 2:
    drive_to_junction("left", 1, 2, 1, 1, "right", 0);  
    break;
    
  case 3:
    drive_to_junction("right", 1, 3, 2, 1, "left", 0);  
    break;
    
  case 4:
    drive_to_junction("right", 1, 2, 1, 1, "right", 0);  
    break;
    
 } 
  
  current_junction = 1; 

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//string mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

void TwoJunction(){
    switch (current_junction){
  case 0:
    drive_to_junction("left", 1, 2, 0, 0, "N/a", 0);
    break;

  case 1:
    //(string mid_direction, direction, direction_of_ninety, park)
    junction_1_directions("right", 1, "left", 0);
    break;
    
  case 2:
    break;
    
  case 3:
    drive_to_junction("left", 0, 1, 0, 0, "N/a", 0);
    break;
    
  case 4:
    drive_to_junction("left", 1, 3, 0, 0, "N/a", 0);
    break;
 } 
  current_junction = 2; 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//string mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

void ThreeJunction(){
    switch (current_junction){
  case 0:
    drive_to_junction("left", 1, 3, 0, 0, "N/a", 0);
    break;
  
  case 1:
    //(string mid_direction, direction, direction_of_ninety, park)
    junction_1_directions("left", 0, "right", 0);

    break;
    
  case 2:
    drive_to_junction("left", 1, 1, 0, 0, "N/a", 0);
    break;
    
  case 3:
    break;
    
  case 4:
    drive_to_junction("left",0, 2, 0, 0, "N/a", 0);
    break;
 } 
  	current_junction = 3; 

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//string mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

void FourJunction(){
    switch (current_junction){
  case 0:
    drive_to_junction("left",0, 1, 0, 0, "N/a", 0);  //U turn
    break;
  
  case 1:
    //(string mid_direction, direction, direction_of_ninety, park)
    junction_1_directions("left", 1, "left", 0);   
    break;
    
  case 2:
    drive_to_junction("left",0, 3, 0, 0, "N/a", 0);
    break;
    
  case 3:
    drive_to_junction("left",0, 2, 0, 0, "N/a", 0);
    break;
    
  case 4:
    break;
    
 } 
  current_junction = 4; 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//string mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park

void FiveJunction(){
  switch (current_junction){
  //coming from junction zero
  case 0:
    drive_to_junction("left",1, 2, 1, 1, "left", 1);  // 90 degree turn @ annoying line
    break;
  
  case 1:
    //(string mid_direction, direction, direction_of_ninety, park)
    junction_1_directions("left", 0, "left", 1);    
    break;
    
  case 2:
    drive_to_junction("left",1, 2, 1, 1, "right", 1);  
    break;
    
  case 3:
    drive_to_junction("left",1, 3, 2, 1, "right", 1);  
    break;
    
  case 4:
    drive_to_junction("left",1, 2, 1, 1, "left", 1);  
    break;
    
 } 
  current_junction = 5; 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drive_to_junction(String mid_direction, int direction, int no_junctions_pass, int junctions_until_turn, int ninety, String direction_of_ninety, int park){
  old_mid_direction = mid_direction;    

 if (direction != old_direction){
  //180 turn // test 180 code
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 200); // set speed of left motor
  analogWrite(motor2Phase, 0); //reverse right motor
  analogWrite(motor2PWM, 200); // set speed of right motor
  delay(730); //ADJUST 
  old_direction = direction;
 }

  if (count == no_junctions_pass){
    if (park == 1){
    parking();
  }
    else{
    count = 0; 
    stop();
    delay(1000);
    a++;
    }
  }
  if (ninety == 1 && count == junctions_until_turn){
      //turn 90
      if (direction_of_ninety = "right"){
        analogWrite(motor1Phase, 255); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 0); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(365); //ADJUST 
    
      }
      else if (direction_of_ninety = "left"){
        analogWrite(motor1Phase, 0); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 255); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(365); //ADJUST     
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void junction_1_directions(String mid_direction, int direction, String direction_of_ninety, int park){
  if (park = 1){
    parking();
  }
  else{
    if (mid_direction != old_mid_direction){

        analogWrite(motor1Phase, 255); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 0); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(730); //ADJUST 

      old_mid_direction = mid_direction;    
      }
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
    sensors_between_line = 0; 
    delay(50);
  
   if (AnalogValue[3]<900 && AnalogValue[2]<900 && AnalogValue[1]<900){
     if (direction_of_ninety = "right"){
        analogWrite(motor1Phase, 255); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 0); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(365); //ADJUST 
    
      }
     else if (direction_of_ninety = "left"){
        analogWrite(motor1Phase, 0); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 255); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(365); //ADJUST     
      }

//from annoying line
  drive_to_junction(mid_direction, direction, 1, 0, 0, "N/a", park);
}
}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void stop(){ 
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 0); // set speed of left motor
  analogWrite(motor2Phase, 255); //forward right moto
  analogWrite(motor2PWM, 0); // set speed of right motor
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void parking(){
  analogWrite(motor1Phase, 255); //forward left motor
  analogWrite(motor1PWM, 0); // set speed of left motor
  analogWrite(motor2Phase, 255); //forward right moto
  analogWrite(motor2PWM, 0); // set speed of right motor
  go = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time){

  for (int i = 0; i<5; i++){ 
  
  int new_speed_m1 = current_speed_m1 + ((speed_m1 - current_speed_m1) * 5) / steps;
  int new_speed_m2 = current_speed_m2 + ((speed_m2 - current_speed_m2) * 5) / steps;


  analogWrite(motor1Phase, direction_m1); //forward left motor
  analogWrite(motor1PWM, new_speed_m1); // set speed of left motor
  analogWrite(motor2Phase, direction_m2); //forward right moto
  analogWrite(motor2PWM, new_speed_m2); // set speed of right motor

  delay(20);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void follow_the_line(){
    //If statements - follow the line
  if(AnalogValue[2]<300){       //forward
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
    sensors_between_line = 0; 
    }

  else if(AnalogValue[1] <300){ //inner left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[1][0], turn[1][1], turn[1][2], turn[1][3], 50);
    sensors_between_line = 1; 
    }
  
  else if(AnalogValue[0]<300){  //outer left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[2][0], turn[2][1], turn[2][2], turn[2][3], 50);
    sensors_between_line = 2; 
    }

  else if(AnalogValue[3]<300){ //inner right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[3][0], turn[3][1], turn[3][2], turn[3][3], 50);
    sensors_between_line = 3; 
    }

  else if(AnalogValue[4]<300){ //outer right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[4][0], turn[4][1], turn[4][2], turn[4][3], 50);
    sensors_between_line = 4; 
    }
  
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){ //stop
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
 }
   
  if (AnalogValue[3]<900 && AnalogValue[2]<900 && AnalogValue[1]<900){ 
    count++;
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
    sensors_between_line = 0; 
    delay(50);
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////�0U#0��c:\�1��C{|F�3hZ�mU0VUO0M0K�I�G�Ehttp://crl.microsoft.com/pki/crl/products/MicTimStaPCA_2010-0