//Questions and Notes for later
/* 
Will we be given the number of junctions needing to be passed? - shouldnt affect it much since the parking function can have a variable that 
ends the loop
*/

#include <vector>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Motors 
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Speed and Turning
int steps = 5; //gradual speed change
int sensors_between_line = 0;

int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {10,11,12,13,14}; // keep 8 free for tone O/P music
int turn[5][4] = {
  {255, 160, 255, 173}, //forward //  {255, 152, 255, 164}, //forward 
  {255, 110, 255, 193}, //inner right
  {255, 15, 255, 255}, //outer right
  {255, 193, 255, 110}, //inner left
  {255, 255, 255, 15} //outer left
};//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Adjacent Matrix for Dijkstra Algorithm
std::vector<std::vector<int>> adjMatrix = {
    {0, 4, 6, INF, 2},
    {4, 0, 2, 5, 5},
    {6, 4, 0, 2, INF},
    {INF, 5, 2, 0, 8},
    {2, 5, INF, 8, 0}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Variables 
int count = 0;//counts number of junctions passed
int start = 0;//driving and direction commands once the mobot has driven up to the starting junction
int quota = 1;//number of 90 turns allowed for each direction
int current_junction = 0; 
int total = 0; //total number of sensors added together

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//direction variables
//note I tried strings but it made the code a lot messier, so 0 is left and 1 is right
int old_mid_direction = 0;
int mid_direction = 0;   // 0 = left, 1 = right
int old_direction = 0;  
int direction = 1; //0 = acw, 1 = cw
int turn90_direction = 0; //direction of 90 degrees

// 0 = Anticlockwise
// 1 = Clockwise
// 0 = left
// 1 = right

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//array
int a = 0; //position in array
//int Junctions[] = {0, 2, 4, 3, 2};
int Junctions[] = {0, 1, 3, 2, 1, 4, 0, 3, 4, 1};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int INF = 1e9; // Define infinity as a large value
const int V = 5; // Number of vertices for vector matrix

int distanceSensorPin = 9; // define distance sensor pin
bool obstacleDetected = false; // bool value for obstacle detection
int currentNode = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Declarations
void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time);
void follow_the_line();
void first_junction();
void stop();

void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass);
void junction_1_directions(int mid_direction, int direction, int direction_of_turn);



void ZeroJunction(); //going to zero, coming from..
void OneJunction();
void TwoJunction();
void ThreeJunction();
void FourJunction();
void FiveJunction();

void turn180_function();
void turn90_function(int direction_of_turn);

std::vector<int> dijkstra(int start, int destination);
void getPath(std::vector<int> route);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    analogWrite(motor1Phase, 255); //forward left motor
    analogWrite(motor1PWM, 0); // set speed of left motor
    analogWrite(motor2Phase, 255); //forward right moto
    analogWrite(motor2PWM, 0); // set speed of right motor
    pinMode(10,INPUT);
    pinMode(11,INPUT);
    pinMode(12,INPUT);
    pinMode(13,INPUT);
    pinMode(14,INPUT);

    current_junction = Junctions[0];
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  Serial.println(count);
  
  if (a<10){
    int i;
    for (i=0;i<5;i++)
      {
      AnalogValue[i]=analogRead(AnalogPin[i]);

      if(i==4)
        {
          if(start<1){
            first_junction();
          }
          else{
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          switch (Junctions[a])
              {case 0:
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

              default:
                Serial.println("Error: Undefined Junction!");
                stop();
                break;
              }
 
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            follow_the_line();
          }
        }
      }
  }
  // stops once it has completed its list of destinations (number 10 can be swapped out how many commands will be going through)
  else if (a>=10){
    stop();
    delay(10000);
    
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Definitions

void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2, int time){
  // gradual speed change
  for (int b = 0; b<5; b++){ 
  
  int new_speed_m1 = current_speed_m1 + ((speed_m1 - current_speed_m1) * 5) / steps;
  int new_speed_m2 = current_speed_m2 + ((speed_m2 - current_speed_m2) * 5) / steps;


  analogWrite(motor1Phase, direction_m1); //forward left motor
  analogWrite(motor1PWM, new_speed_m1); // set speed of left motor
  analogWrite(motor2Phase, direction_m2); //forward right moto
  analogWrite(motor2PWM, new_speed_m2); // set speed of right motor

  delay(15);
 }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//follow the line code
void follow_the_line(){
  // adds the total readings of sensors to detect junctions
  total = AnalogValue[4] + AnalogValue[3] + AnalogValue[2] + AnalogValue[1] +  AnalogValue[0];

    //If statements - follow the line
  if(AnalogValue[2]<350){       //forward
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
    sensors_between_line = 0; 
    }

  else if(AnalogValue[1] <280){ //inner left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[1][0], turn[1][1], turn[1][2], turn[1][3], 50);
    sensors_between_line = 1; 
    }
  
  else if(AnalogValue[0]<280){  //outer left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[2][0], turn[2][1], turn[2][2], turn[2][3], 50);
    sensors_between_line = 2; 
    }

  else if(AnalogValue[3]<280){ //inner right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[3][0], turn[3][1], turn[3][2], turn[3][3], 50);
    sensors_between_line = 3; 
    }

  else if(AnalogValue[4]<280){ //outer right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[4][0], turn[4][1], turn[4][2], turn[4][3], 50);
    sensors_between_line = 4; 
    }
  
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){ //stop
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
 }

  if (total < 6700){ 
    // counts every time it passes over a junction
    count++;
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
    delay(10);
}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// drives up to first junction (i.e. the mobot will start between 4 and 0, it will drive up to 0 and set this as its starting position before moving on)
// mobot will always start moving anticlockwise
void first_junction(){
  if (AnalogValue[3]<900 && AnalogValue[2]<900 && AnalogValue[1]<900){ 
  start = 1; 
  a++;
  stop();
  Serial.println("Start Line");
  delay (2000);
  drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
  delay(50);
  count = 0; 

  }

  else{
    follow_the_line();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  old_mid_direction = mid_direction;   
    //if mobot is facing the wrong direction, turn 180
    if (direction != old_direction){
          turn180_function();
      }

    //if mobot has not turned already, mobot will turn when mobot has passed a certain amount of junctions
    if (quota == 1 && count == junctions_until_turn && junctions_until_turn == 1){
          turn90_function(direction_of_turn);

    }

    if (count == no_junctions_pass){
        //it has reached the next destination 
        drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 50);
        delay(10);
        //resets junction to zero
        count = 0; 
        stop();
        delay(1000);
        //refills quota (for number of turns allowed) for the next set of directions
        quota = 1; 
        //remembers current junction as refernce for directions
        current_junction = Junctions[a];
        //moves on to the next destination in the array
        a++;
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

void turn180_function(){
      //updates direction the mobot is facing
      old_direction = direction;    
      //rotate 180 degrees
      analogWrite(motor1Phase, 255); //forward left motor
      analogWrite(motor1PWM, 200); // set speed of left motor
      analogWrite(motor2Phase, 0); //reverse right motor
      analogWrite(motor2PWM, 200); // set speed of right motor
      delay(730); //ADJUST 
      // drives forward a little bit before next commamd
      drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
      delay(50);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void turn90_function(int direction_of_turn){
      delay(100);
      //left turn
      if (direction_of_turn == 0){
        analogWrite(motor1Phase, 250); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 0); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(355); //ADJUST 
    
      }
      //right turn
      else if (direction_of_turn == 1){
        analogWrite(motor1Phase, 0); //forward left motor
        analogWrite(motor1PWM, 200); // set speed of left motor
        analogWrite(motor2Phase, 250); //reverse right motor
        analogWrite(motor2PWM, 200); // set speed of right motor
        delay(355); //ADJUST     
      }
      //drives past the junction
      drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3], 50);
      delay(50);
      //no more turns allowed until next command given (otherwise it will continue to keep turning)
      quota = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//going to zero, coming from..

void ZeroJunction(){
  Serial.println("J: 0");
  switch (current_junction){
      //coming from junction zero
      case 0:
        break;
      //going to zero, coming from 1
      case 1:
        junction_1_directions(1, 1, 1);

        break;
        
      case 2:
        drive_to_junction(0, 1, 0, 1, 2);

        break;
        
      case 3:
        drive_to_junction(0, 0, 0, 1, 3);

        break;
        
      case 4:
          drive_to_junction(0, 1, 0, 1, 1);

        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  //to one, travelling from ....current_junction
void OneJunction(){
  Serial.println("J: 1");
  switch (current_junction){
      //coming from junction zero
      case 0:
        Serial.println("From J: 0");
        drive_to_junction(0, 0, 1,0, 2);
        break;
      
      case 1:
        break;
        
      case 2:
        drive_to_junction(0, 1, 1, 1, 2);

        break;
        
      case 3:
        drive_to_junction(1, 0, 1, 0, 2);

        break;
        
      case 4:
        drive_to_junction(1, 1, 1, 1, 2);
        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  //to two, travelling from ....current_junction
void TwoJunction(){
  Serial.println("J: 2");
  switch (current_junction){
      //coming from junction zero
      case 0:
        drive_to_junction(0, 0, 0, 0, 2);
        break;
      
      case 1:
        junction_1_directions(1, 0, 0);
        break;
        
      case 2:
        break;
        
      case 3:
        drive_to_junction(0, 1, 0, 2, 1);

        break;
        
      case 4:
        drive_to_junction(0, 0, 0, 1, 3);
        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//to three, travelling from ....current_junction

void ThreeJunction(){
  Serial.println("J: 3");
  switch (current_junction){
      //coming from junction zero
      case 0:
        Serial.println("From J: 0");
        break;
      
      case 1:
        junction_1_directions(0, 1, 1);

        break;
        
      case 2:
        drive_to_junction(0, 1, 0, 1, 1);

        break;
        
      case 3:
        break;
        
      case 4:
        drive_to_junction(1, 1, 0, 0, 2);

        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
void FourJunction(){
  //to four, travelling from ....current_junction
  Serial.println("J: 4");
  switch (current_junction){
      //coming from junction zero
      case 0:
        Serial.println("From J: 0");
        break;
      
      case 1:
        junction_1_directions(0, 0, 0);
        break;
        
      case 2:
        drive_to_junction(0, 1, 0, 0, 3);

        break;
        
      case 3:
          drive_to_junction(0, 0, 0, 1, 2);

        break;
        
      case 4:
        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//no parking function yet
void FiveJunction(){
  Serial.println("J: 5");
  switch (current_junction){
      //coming from junction zero
      case 0:
        Serial.println("From J: 0");
        break;
      
      case 1:
        break;
        
      case 2:
        break;
        
      case 3:
        break;
        
      case 4:
        break;
    
 } 

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function for leaving junction 1 
//mid_direction tells if the mobot is exiting junction 1 to the left (0) or right (1)
void junction_1_directions(int mid_direction, int direction, int direction_of_turn){
    old_direction = direction; 
    //if the mobot is facing the wrong direction it will turn around to exit junction 1 facing the other direction
    if (mid_direction != old_mid_direction){
      turn180_function();
      }
    //when it reaches the edge junction it will turn right or left
    if (AnalogValue[3]<900 && AnalogValue[2]<900 && AnalogValue[1]<900){
          turn90_function(direction_of_turn);
    }
    
  quota = 0;
  //driving from edge junctions (5&6) to the next junction
  drive_to_junction(mid_direction, direction, 0, 2, 2);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dijkstra Algorithm
std::vector<int> dijkstra(int start, int destination) {
    std::vector<int> dist(V, INF);
    std::vector<int> parent(V, -1);
    std::vector<bool> visited(V, false);
    dist[start] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (u == -1 || dist[u] == INF) break;

        visited[u] = true;
        for (int v = 0; v < V; v++) {
            if (!visited[v] && adjMatrix[u][v] != INF) {
                int newDist = dist[u] + adjMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                }
            }
        }
    }

    std::vector<int> path;
    for (int v = destination; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    return path;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> getPath(int nextNode){
  std::vector<int> path = dijkstra(currentNode,nextNode);
  currentNode = nextNode;
  return path;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
