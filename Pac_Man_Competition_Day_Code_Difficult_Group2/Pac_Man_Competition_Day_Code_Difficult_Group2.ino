

//Questions and Notes for later
//goes to junction 0, then calls server
//corrects direction after start to be straight
#include <WiFi.h> // Use WiFi.h for ESP32
#include <vector>
#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "Chords.h" //music

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Setting Pins 
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;
int distance_sensor = 9; 
int reverse_sensor = 18;
int change_direction = -1;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PacMan Song 
int music[] = {
  NOTE_B3, NOTE_B4, NOTE_FS4, NOTE_DS4, NOTE_B4, NOTE_FS4, NOTE_DS4, NOTE_C4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_B3, NOTE_B4, NOTE_FS4, NOTE_DS4, NOTE_B, NOTE_FS4, NOTE_DS4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_F4,NOTE_FS4, NOTE_G4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_B4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {

  8, 8, 8, 8, 12, 8, 4, 8, 8, 8, 8, 12, 8, 4, 8, 8, 8, 8, 12, 8, 4, 16, 16, 8, 16, 16, 8, 16, 16, 8, 4
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wifi variables

char ssid[] = "iot";

char password[] = "prerequiring9unraveller";
char server[] = "3.250.38.184";
int port = 8000;
WiFiClient client;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESP NOW variables
constexpr uint8_t ESP_NOW_RECEIVER[] = {0x48, 0x27, 0xE2, 0x15, 0x50, 0x78};
esp_now_peer_info_t peerInfo;
bool channelFound = false;
uint8_t channel = 1;
uint32_t last;

typedef struct Junctions {
  int junction1;
} Junctions;

Junctions myjun;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Speed and Turning
int steps = 5; //gradual speed change
int sensors_between_line = 0;

//Line Sensor Pins
int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {10,11,12,13,14}; // keep 8 free for tone O/P music
int distance_sensor_reading = 0; 
int reverse_sensor_reading = 0;

//Speed Values
int turn[5][4] = {
  { 255, 172, 255, 165 },  //forward 
  { 255, 100, 255, 195 },  //inner right - slight adjustments to turn
  { 255, 12, 255, 255 },   //outer right - sharp adjustments to turn
  { 255, 213, 255, 118 },  //inner left
  { 255, 255, 255, 12 }    //outer left
}; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Variables 
int count = 0;//counts number of junctions passed
int stage = 0;//driving and direction commands once the mobot has driven up to the starting junction
int end_stage = 0; 
int reverse_count = 0; // number of 180 turns allowed for for each instruction
int quota = 1;//number of 90 turns allowed for each direction
int current_junction = 0; 
int total = 0; //total number of sensors added together

int junction = 0; //used to tell if it is coming from the middle node (1) or not
int obstacle_count = 0; 
int threshold = 1890;

int previous_checkpoint = 0;
int next_checkpoint = 0;
int target_junction = 8;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//direction variables
//integers were easier to use rather than strings
int old_mid_direction = -1; //used for the car's direction on the middle road (not the perimeter) - directions were left or right
int mid_direction = -1;   // 0 = left, 1 = right
int old_direction = -1;  
int direction = -1; //-1 = acw, 1 = cw
int turn90_direction = -1; //direction of 90 degrees

// 0 = Anticlockwise
// 1 = Clockwise
// 0 = left
// 1 = right

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Algorithm 
int desired_destination = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//array
int r = 0; //position in route array
int next = 0; //next destination
std::vector<int> route = {0}; //directions to destinations


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int INF = 1e9; // Define infinity as a large value
const int V = 6; // Number of vertices for vector matrix

/*map array - distance from each node. Used later to calculate the shortest routes
Each line in the vector shows its connections to the other nodes on the map (e.g. line 1 is node Zero)
Therefore Node Zero is not connected to Node Three because it is set to INF
*/
std::vector<std::vector<int>> adjMatrix = {
    {0, 2, 2, INF, 1, INF},
    {2, 0, 2, 3, 3, 1},
    {2, 2, 0, 1, INF, INF},
    {INF, 3, 1, 0, 4, INF},
    {1, 3, INF, 4, 0, INF},
    {INF, 5, INF, INF, INF,0}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Declarations3
void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2);
void follow_the_line();
void stop();

void obstacle_you_are_in_my_way(int pass);
void playmusic();

void parking(); 
void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass);
void junction_1_directions(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass );

void ZeroJunction(); //going to zero, coming from..
void OneJunction();
void TwoJunction();
void ThreeJunction();
void FourJunction();
void FiveJunction();

void turn180_function();
void turn90_function(int direction_of_turn, int pass);

void avoid_obstacles(int current_junction, int next_junction, int no_junctions_pass);
void tryNextChannel();

//wifi function declarations 
void connectToWiFi();
bool connectToServer();
int getStatusCode(String &response);
String getResponseBody(String &response);
int getNextPosition(int currentPosition);

//ESP NOW function declarations
void initEspNow();
void tryNextChannel();
void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
void send_Message();
//reroute algorithm declarations
std::vector<int> dijkstra(int startNode, int destinationNode);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    //straightens motor after power surge
    analogWrite(motor1Phase, 255); //forward left motor
    analogWrite(motor1PWM, 255); // set speed of left motor
    analogWrite(motor2Phase, 255); //forward right moto
    analogWrite(motor2PWM, 0); // set speed of right motor
    delay(168);
    stop();
    //setting Sensor pins as inpits
    pinMode(9,INPUT);
    pinMode(10,INPUT);
    pinMode(11,INPUT);
    pinMode(12,INPUT);
    pinMode(13,INPUT);
    pinMode(14,INPUT);
    connectToWiFi();
    Serial.println(WiFi.channel());
    initEspNow();
    //sets first junction as the first destination (0)
    current_junction = desired_destination;
    // updates map
    send_Message();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  //stage 0 = driving up to first junction
  //stage 1 = driving from junction to junction 
  //stage 2 = driving to park 
  //stage 3 = stop

  if (stage < 2){
    //reads line sensors
    for (int i=0;i<5;i++)
      {
      AnalogValue[i]=analogRead(AnalogPin[i]);

      //after all line sensors are read it proceeds to driving instructions
      if(i==4)
        {
          //Driving to the start line
          if(stage<1){
            drive_to_junction(-1, -1, 0, 1, 1);
            follow_the_line();   
          }
          //directions
          else{
            // going to next checkpoint
          switch(next_checkpoint)
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
             follow_the_line();
          
        }
      }
  }

  }
  //continues to park
  else if (stage == 2){
    parking();
  }

  //parked, stop
  else if (stage == 3){
    stop();
    delay(20);
    int finish = getNextPosition(5);
    current_junction = 5;
    //updates map
    send_Message();
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connect to WiFi

void connectToWiFi() {
    Serial.print("Connecting to network: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 30000) {
        Serial.print(".");
        delay(300);
    }

    //if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi. Check credentials or network status.");
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Connect to the server
bool connectToServer() {
    if (!client.connect(server, port)) {
        Serial.println("Error connecting to server");
        return false;
    }
    Serial.println("Connected to server");
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to read response from the server
String readResponse() {
    #define BUFSIZE 512
    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);
    client.readBytes(buffer, BUFSIZE);
    String response(buffer);
    return response;
}

// Function to extract HTTP status code
int getStatusCode(String& response) {
    String code = response.substring(9, 12);
    return code.toInt();
}

// Function to extract response body
String getResponseBody(String& response) {
    int split = response.indexOf("\r\n\r\n");
    String body = response.substring(split + 4);
    body.trim();
    return body;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//gets next destination from server
int getNextPosition(int currentPosition) {
    if (connectToServer()) {
            // Prepare POST body
            String postBody = "position=" + String(next);

            // Send POST request and headers
            client.println("POST /api/arrived/rthj3564 HTTP/1.1");
            client.println("Host: " + String(server));
            client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.println(postBody.length());
            client.println("Connection: close");
            client.println();
            client.println(postBody);

            // Read and handle response
            String response = readResponse();
            Serial.println("Raw Response: " + response); // Debugging: Print the entire response

            int statusCode = getStatusCode(response);

            if (statusCode == 200) {
                Serial.println("POST request successful!");
                String body = getResponseBody(response);
                Serial.println("Response body: " + body);

                // Handle destination logic
                if (!body.equals("undefined")) {
                    int destination = body.toInt();
                    if (destination == -1) { // Assuming -1 indicates the final stop
                        Serial.println("Final stop reached. Exiting loop.");
                        return -1;// break; // Exit the loop if the final stop is reached
                    } else {
                        //position = destination;
                        //int next= destination// Update position for the next iteration
                        Serial.println("Next destination: " + String(destination));
                        return destination;
                    }
                }
            } else {
                Serial.println("POST request failed. Status code: " + String(statusCode));
                return -2;// break; // Exit the loop if a request fails
            }

            client.stop(); // Close the connection before the next iteration
            delay(500); // Add a small delay to prevent overwhelming the server
        } else {
            Serial.println("Failed to connect to server. Retrying...");
            delay(500);
            return -2;// Wait before retrying the server connection
        }
        return 100;
        delay(3000);
    // Nothing needed here as the main logic is in setup()
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dijkstra's Algorithm - Returns the path 
std::vector<int> dijkstra(int startNode, int destinationNode) {
    std::vector<int> dist(V, INF);
    std::vector<int> parent(V, -1);
    std::vector<bool> visited(V, false);
    dist[startNode] = 0;

    //goes through vector checking distance
    for (int counter = 0; counter < V - 1; counter++) {
        int u = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        //if vector has INF in a line then it the connection between the two nodes are broken
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

    //Creates the shortest route and adds it to Path
    std::vector<int> path;
    for (int v = destinationNode; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    
    //prints of the route
    Serial.print("route: ");
    reverse(path.begin(), path.end());
    for(int j: path){
      Serial.print(" ");
      Serial.print(j);
    }
    Serial.println("");
    return path;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Function Definitions

void drive(int current_direction_m1, int current_speed_m1, int current_direction_m2, int current_speed_m2, int direction_m1, int speed_m1, int direction_m2, int speed_m2){
  // gradual speed change, gradually changes the speed across 5 stages
  for (int b = 0; b<5; b++){ 
  
  int new_speed_m1 = current_speed_m1 + ((speed_m1 - current_speed_m1) * b) / steps;
  int new_speed_m2 = current_speed_m2 + ((speed_m2 - current_speed_m2) * b) / steps;


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
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3]);
    sensors_between_line = 0; 
    }
 
  else if(AnalogValue[0]<280){  //outer left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[2][0], turn[2][1], turn[2][2], turn[2][3]);
    sensors_between_line = 2; 
    }

  else if(AnalogValue[1] <280){ //inner left
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[1][0], turn[1][1], turn[1][2], turn[1][3]);
    sensors_between_line = 1; 
    }

  else if(AnalogValue[4]<280){ //outer right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[4][0], turn[4][1], turn[4][2], turn[4][3]);
    sensors_between_line = 4; 
    }

  else if(AnalogValue[3]<280){ //inner right
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[3][0], turn[3][1], turn[3][2], turn[3][3]);
    sensors_between_line = 3; 
    }

  //if all sensors do not detect the line then repeat the last command until it finds it again
  else if(AnalogValue[4]>350 && AnalogValue[3]>350 && AnalogValue[2]>350 && AnalogValue[1]>350 && AnalogValue[0]>350){ //stop
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3]);
 }

//checks the average value of the line sensors checking for a juntion
  if (total < 6405){ 
    // counts every time it passes over a junction
    delay(10);
    count++;
    delay(10);

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//takes all the variables required for direction, number of junctions until turn and direction until turn, number of junctions to pass until next destination
void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  //if it is not coming from junction 1
  //Changes direction whether the car is travelling anticlockwise or clockwise
  if (junction == 0){
  //left or right in the middle of map
      old_mid_direction = mid_direction;   
        //if mobot is facing the wrong direction, turn 180
          //checks the direction it is currently facing and the direction it has to travel next, if they are not the same the car does a 180
        if (direction != old_direction){
              //updates direction the mobot is facing
              old_direction = direction;  
              turn180_function();
              count = 0;

          }
  }

  //if it is coming from junction 1, it changes direction based on whether the car is facing left or right
  else {
      old_direction = direction;  
      //if the mobot is facing the wrong direction it will turn around to exit junction 1 facing the other direction
      if (mid_direction != old_mid_direction){
        old_mid_direction = mid_direction;  
        turn180_function();
        count = 0;
        }

  }
    //if mobot has not turned already, mobot will turn when mobot has passed a certain amount of junctions
    if (quota == 1 && count == junctions_until_turn && junctions_until_turn == 1){
          turn90_function(direction_of_turn, no_junctions_pass);

    }

    //checks for obstacles before proceeding
    obstacle_you_are_in_my_way(no_junctions_pass);
  delay(5);

    //if the mobot has crossed the number of junctions until the next junction
    if (count == no_junctions_pass){
          delay(5);
          count = 0;               //resets junction to zero
          quota = 1;               //refills quota (for number of turns allowed) for the next set of directions
          stage = 1;
          junction = 0; 

          //it has reached the next destination 
          //updates variables for Switch Cases
          current_junction = next_checkpoint;
          Serial.println(current_junction);  
          previous_checkpoint = current_junction; 
          //if it has not reached the end stage of its operation (endstage == if the mobot is parked or parking)
          //end_stage hasnt been called yet
          if (end_stage == 0){
            //stops at junction
              stop();
              delay(150);

              //if the car after rerouting has returned to its previous position (last known position)
              if (current_junction == target_junction){
                target_junction = 8; 
                //reroutes based on new vector conditions (after updating because of the obstacle)
                route = dijkstra(current_junction, desired_destination);
                r = 0; 

              }

              //if the mobot's current junction is the destination from the server it will communicate with the server to get a new destiantion
              if (current_junction == desired_destination){
                //updates map
                  send_Message();
                //communicates with server
                  next = getNextPosition(next);
                  if(next!= -2){
                    desired_destination = next;
                  }
                  //gets new route
                  route = dijkstra(current_junction, desired_destination);
                  r = 0; 
                  //plays iconic pacman music
                  playmusic();
                }
              
              r++;                     //moves on to the next destination in the array
              //remembers current junction as refernce for directions
              next_checkpoint = route[r];

          }
          
          else if (end_stage == 1){
              stop();
              delay(350);
              stage = 2; 
              parking();
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

void turn180_function(){
      stop();
      delay(10);
      //rotate 180 degrees
      analogWrite(motor1Phase, 255); //forward left motor
      analogWrite(motor1PWM, 200); // set speed of left motor
      analogWrite(motor2Phase, 0); //reverse right motor
      analogWrite(motor2PWM, 200); // set speed of right motor
      delay(710);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void turn90_function(int direction_of_turn, int pass){
  delay(25);
  //left turn
  if (direction_of_turn == -1) {
    analogWrite(motor1Phase, 255);  //forward left motor
    analogWrite(motor1PWM, 255);    // set speed of left motor
    analogWrite(motor2Phase, 0);    //reverse right motor
    analogWrite(motor2PWM, 100);    // set speed of right motor
    delay(230);
    AnalogValue[3]=analogRead(AnalogPin[3]);

    //continues to turn until it detects the line again on the inner right line sensor (helps create a smooth transition when turning 90)
    while(AnalogValue[3]>400){
    AnalogValue[3]=analogRead(AnalogPin[3]);
    delay(2);

    }
  }
  //right turn
  else if (direction_of_turn == 1) {
    analogWrite(motor1Phase, 0);    //forward left motor
    analogWrite(motor1PWM, 100);    // set speed of left motor
    analogWrite(motor2Phase, 255);  //reverse right motor
    analogWrite(motor2PWM, 255);    // set speed of right motor
    delay(190);
   AnalogValue[1]=analogRead(AnalogPin[1]);

    //continues to turn until it detects the line again on the inner left line sensor (helps create a smooth transition when turning 90)
    while(AnalogValue[1]>400){
    AnalogValue[1]=analogRead(AnalogPin[1]);
    delay(2);
  }
  }
  
  //checks for immediate obstacles after turning

  obstacle_you_are_in_my_way(pass);

  //corrects its position on the line before next command
  for (int z = 0; z<3; z++){
    for (int i=0;i<4;i++)
      {
      //checks for obstacles while correcting position
      obstacle_you_are_in_my_way(pass);
      AnalogValue[i]=analogRead(AnalogPin[i]);
      if(i==4)
        {
        follow_the_line();
        delay(2);
        }
      }
  }

  //no more turns allowed until next command given (otherwise it will continue to keep turning)
  quota = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//going to zero, coming from..

void ZeroJunction(){
  switch (previous_checkpoint){
      //going to zero, coming from 1
      case 1:
        junction = 1; 
        //this command is used for rerouting if it encounters an obstacle between 1 and 0, this helps update the direction the car needs to face - helped minimise glitches in the rerouting
        change_direction = 1;
        //void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
        drive_to_junction(1, 1, 1, 1, 2);

        break;

      //going to zero, coming from 2
      case 2:
        change_direction = -1;
        drive_to_junction(-1, 1, 0, 1, 2);

        break;

      //going to zero, coming from 4
      case 4:
          change_direction = -1;
          drive_to_junction(-1, -1, 0, 1, 1);   

        break;
    
 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //to one, travelling from ....current_junction
void OneJunction(){
    switch (previous_checkpoint){
      //coming from junction zero
     case 0:
        //this command is used for rerouting if it encounters an obstacle between 1 and 0, this helps update the direction the car needs to face - helped minimise glitches in the rerouting
        change_direction = -1;
        //void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
        drive_to_junction(-1, -1, 1, -1, 2);
        break;
        
      //going to one, coming from 2
      case 2:
        change_direction = 1;
        drive_to_junction(-1, 1, 1, 1, 2);

        break;
        
      //going to one, coming from 3
      case 3:
        change_direction = 1;
        drive_to_junction(1, -1, 1, -1, 2);

        break;
        
      case 4:
      //going to one, coming from 4
        change_direction = 1;
        drive_to_junction(1, 1, 1, 1, 2);
        break;
 } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  //to two, travelling from ....current_junction
void TwoJunction(){
  switch (previous_checkpoint){
      //coming from junction zero
      case 0:
        //this variable is used for rerouting if it encounters an obstacle between 1 and 0, this helps update the direction the car needs to face - helped minimise glitches in the rerouting
        change_direction = -1;
        //void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
        drive_to_junction(-1, -1, 0, -1, 2);
        break;
      
      case 1:
        junction = 1; 
        change_direction = 1;
        drive_to_junction(1, -1, 1, -1, 2);
        break;
        
      case 3:
        change_direction = 1;
        drive_to_junction(-1, 1, 0, 2, 1);

        break;

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//to three, travelling from ....current_junction

void ThreeJunction(){
  switch (previous_checkpoint){
      //coming from junction one
      case 1:
        junction = 1;
        //this variable is used for rerouting if it encounters an obstacle between 1 and 0, this helps update the direction the car needs to face - helped minimise glitches in the rerouting
        change_direction = -1; 
        //void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
        drive_to_junction(-1, 1, 1, 1, 2);

        break;
        
      case 2:
        change_direction = -1; 
        drive_to_junction(-1, -1, 0, 1, 1);

        break;

      case 4:
        change_direction = 1; 
        drive_to_junction(1, 1, 0, -1, 2);

        break;
    
 } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
void FourJunction(){
  //to four, travelling from ....current_junction
  switch (previous_checkpoint){
      //coming from junction zero
      case 0:
        //this variable is used for rerouting if it encounters an obstacle between 1 and 0, this helps update the direction the car needs to face - helped minimise glitches in the rerouting 
        change_direction = 1;
        //void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
        drive_to_junction(1, 1, 0, 1, 1);
        break;
      
      case 1:
        junction = 1; 
        change_direction = -1; 
        drive_to_junction(-1, -1, 1, -1, 2);
        break;
        
      case 3:
        change_direction = -1; 
          drive_to_junction(-1, -1, 0, 1, 2);

        break;

 } 

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void drive_to_junction(int mid_direction, int direction, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
//only comes from junction 1 
void FiveJunction(){
  //enters parking/parked stage
  end_stage = 1;  
  parking();
 } 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void avoid_obstacles(int current_junction, int next_junction, int no_junctions_pass){
  Serial.println("Avoid Obstacle Function Called");
  
  //breaks connection between previous checking and the next checkpoint
  adjMatrix[previous_checkpoint][next_checkpoint] = INF;  // Break the connection from node 1 to node 2
  adjMatrix[next_checkpoint][previous_checkpoint] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  
  //these cases were put in to break multiple connections if needed (i.e. if the mobot is coming from one and meets an obstacle, it may need to break its connections from both 2 and 0)
  //these conditions were hardcoded to increase speed and efficiency on the day to minimise risks of rerouting errors

  //if the mobot is not going or coming from 1 (staying on the perimeter)
  if (next_junction != 1 || current_junction != 1){
  if (count == 0 && no_junctions_pass > 1){
    adjMatrix[current_junction][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][current_junction] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }
  else if(count == 1 && no_junctions_pass > 1){
    adjMatrix[next_junction][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][next_junction] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }
  }
  //if obstacle is next to node 1, car is coming from node 1
  if(current_junction == 1){
  //when count = 0 the obstacle is not on the perimeter of the circle, breaking the connection between 1 and two nodes
  //node 2 and 0 connect to 1 via the same connection in the middle of the map
  //node 3 and 4 connect to 1 via the same connection in the middle of the map
  if (next_junction == 2 && count == 0){
    adjMatrix[0][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][0] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  else if (next_junction == 0 &&  count ==0){
    adjMatrix[2][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][2] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  if (next_junction == 3 && count == 0){
    adjMatrix[4][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][4] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  else if (next_junction == 4 && count ==0){
    adjMatrix[3][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][3] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }
  }

  //similarly, if the mobot is travelling to node 1 and meets an obstacle next to node 1
  else if(next_junction == 1){
  if (current_junction == 2 && count == 1){
    adjMatrix[0][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][0] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  else if (current_junction == 0 &&  count == 1){
    adjMatrix[2][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][2] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  if (current_junction == 3 && count == 1){
    adjMatrix[4][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][4] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }

  else if (current_junction == 4 && count == 1){
    adjMatrix[3][1] = INF;  // Break the connection from node 1 to node 2
    adjMatrix[1][3] = INF;  // Break the connection from node 2 to node 1 (undirected graph)
  }
  }

//prints matrix of new map connections
    for (size_t i = 0; i < adjMatrix.size(); i++) {
        for (size_t j = 0; j < adjMatrix[i].size(); j++) {
            if (adjMatrix[i][j] == INF) {
                Serial.print("INF ");  // Print "INF" for no connection
            } else {
                Serial.print(adjMatrix[i][j]);
                Serial.print(" ");
            }
        }
        Serial.println();  // Move to the next row
    }
}  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void parking(){
    distance_sensor_reading=analogRead(distance_sensor);
    //checks if it is facing the right wall
    if (old_mid_direction != -1 && reverse_count == 0){
        turn180_function();
      count = 0;
        reverse_count = 1;
      //follows the line to straighten itself PRIOR to parking (malfunctioned on the day after working for the past two weeks)
      for (int i = 0; i<20; i++){
      for (int i=0;i<5;i++)
      {
      AnalogValue[i]=analogRead(AnalogPin[i]);

      if(i==4)
        {
        follow_the_line();
        delay(2);
         }
        }
  }
    }
    //stage 2 of Mobot - driving to park
    stage = 2; 
    // if it detects the wall it will come to a soft stop 
    if(distance_sensor_reading > 1900){
        //stage 3 of Mobot - parked
        stage = 3;

    }
    else{
      //otherwise drive straight
    drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], turn[0][0], turn[0][1], turn[0][2], turn[0][3]);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void playmusic(){
  for (int Note = 0; Note < 32; Note++) {

    // calculate note duration, take one second divided by the note type (quarter note, eighth note etc)
    //quarter note = 1000 / 4, eighth note = 1000/8, ...

    int noteDuration = 1000 / noteDurations[Note];

    tone(8, music[Note], noteDuration);

    // Sets note durection plus pause between notes

    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);

    // stop the tone playing:

    noTone(8);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void obstacle_you_are_in_my_way(int pass){
  //obstacle if statement 
  distance_sensor_reading=analogRead(distance_sensor);
  if (distance_sensor_reading > threshold){ //detects obstacle
    for (int s = 0; s < 4; s++){ //checks 3 times to ensure it isnt a glitch or a "fantom obstacle"
      distance_sensor_reading=analogRead(distance_sensor);
      if (distance_sensor_reading > threshold){
        obstacle_count++;
        delay(5);
      }
    }
  
    //if it detects an obstacle 4 times (meaning it is actually there)
    if (obstacle_count == 4){
      //Serial.println("Obstacle Detected");
      delay(5);
      stop();
      delay(200);
      
      //sets the next junction as the previous checkpoint (where it was going to) and the next check point as the current junction (where it was coming from) 
      previous_checkpoint = route[r];
      next_checkpoint = current_junction;

      //sets temporary target destination
      target_junction = current_junction;

      //updates map
      avoid_obstacles(current_junction, route[r], pass);

      //finds new route
      route = dijkstra(current_junction, route[r]);

      //adjusts direction for new directions
      old_direction = change_direction*direction;
      old_mid_direction = change_direction*mid_direction; 
      delay(5);

      //checks how many junctions it needs to pass to return to its last known position
      //if it has NOT passed a junction already then it doesnt need a right turn
      if (pass == 2 && count == 0){
      count=1;
      quota = 0;  //no right turns needed
      }
      else{

      count = 0;
      quota = 1; 
      }

      delay(5);
      turn180_function();
      delay(5);

      //reverses until it meets the obstacle again - this ensures that when it does its 180 degree turns it does not miss a junction
      //uses reverse distance sensor
      reverse_sensor_reading=analogRead(reverse_sensor);
      drive(turn[sensors_between_line][0], turn[sensors_between_line][1], turn[sensors_between_line][2], turn[sensors_between_line][3], 0, 125, 0, 115);
      
      //checks reverse sensor
      while(reverse_sensor_reading < 2420){
        if (analogRead(reverse_sensor) > 2420){
          for (int s = 0; s < 4; s++){ //checks 3 times to ensure it detects accurately and correctly 
            if (analogRead(reverse_sensor) > 2420){
            obstacle_count--;
            delay(5);
            }
          }
        }

        //if it detect 4 times them it updates the reverse sensor reading value and breaks the while loop
        if (obstacle_count == 0){
            stop();
            reverse_sensor_reading = analogRead(reverse_sensor);
          }

        //else it updates the counter to be equal to 4 again to repeats until it meets an obstalce
        else {
          obstacle_count = 4; 
        }
      }
    }
    obstacle_count = 0;
  } 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ESP NOW Functions
//Used for LIVE PacMan Mapping 

// Function to initialize ESP-NOW communication
void initEspNow() {
  //Sets up esp now WiFI
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP NOW failed to initialize");
      while (1);
  }

  // Copy receiver MAC address into peerInfo

  memcpy(peerInfo.peer_addr, ESP_NOW_RECEIVER, 6);
  // peerInfo.ifidx   = ESP_IF_WIFI_STA;
  peerInfo.encrypt = false;

  // Add the peer device
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("ESP NOW pairing failure");
      while (1);
  }
    // Set the WiFi channel in promiscuous mode
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  // Register callback function to monitor data send status
  esp_now_register_send_cb(onDataSent);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to go through WiFi channels if communication fails
void tryNextChannel() {
  Serial.println("Changing channel from " + String(channel) + " to " + String(channel+1));
  channel = channel % 13 + 1;  // go through channels 1-13
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Callback function triggered when data is sent
void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  if (!channelFound && status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Fail because channel" + String(channel) + " does not match receiver channel.");
    tryNextChannel(); // If message was not delivered, it tries on another wifi channel.
  }
  else {
    Serial.println("Delivery Successful ! Using channel : " + String(channel));
    channelFound = true;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to send mapping data to another ESP device
void send_Message(){
  if (millis() - last > 2000) { //Send message every 2 seconds
      myjun.junction1 = current_junction;
      esp_now_send(ESP_NOW_RECEIVER, (uint8_t *) &myjun, sizeof(Junctions));// Send data
      last = millis();
  }
}