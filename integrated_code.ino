
//Questions and Notes for later
/* 
Will we be given the number of junctions needing to be passed? - shouldnt affect it much since the parking function can have a variable that 
ends the loop
*/
#include <WiFi.h> // Use WiFi.h for ESP32
#include <vector>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Motors 
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wifi variables

char ssid[] = "iot";
char password[] = "adenolipoma70skemmel";
char server[] = "3.250.38.184";
int port = 8000;
WiFiClient client;

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
int directions = 1; //0 = acw, 1 = cw
int turn90_direction = 0; //direction of 90 degrees

// 0 = Anticlockwise
// 1 = Clockwise
// 0 = left
// 1 = right

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//array
int a = 0; //position in array
int next =0;
//int Junctions[] = {0,2,3,2};
std::vector<int> Junctions = {0};
//int Junctions[] = {0, 2, 4, 3, 2,0,3};
//int Junctions[] = {0, 2, 4, 0, 3, 4, 1};
int size = Junctions.size()

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

void connectToWiFi();
bool connectToServer();
int getStatusCode(String &response);
String getResponseBody(String &response);
int getNextPosition(int currentPosition);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  // put your setup code here, to run once:
  
    Serial.begin(9600);
    connectToWiFi();
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
        next = getNextPosition(next);
        if(next!= -2){
          Junctions.push_back(next);
        }
        delay(3000);

  
    for (int i=0;i<5;i++)
      {
      AnalogValue[i]=analogRead(AnalogPin[i]);
      Serial.println(AnalogValue[i]);

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
  // stops once it has completed its list of destinations (number 10 can be swapped out how many commands will be going through)
  /*
  if{
    stop();
    delay(10000);
    
  }
  */
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
            delay(1000); // Add a small delay to prevent overwhelming the server
        } else {
            Serial.println("Failed to connect to server. Retrying...");
            delay(3000);
            return -2;// Wait before retrying the server connection
        }
        return 100;
        delay(3000);
    // Nothing needed here as the main logic is in setup()
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
void drive_to_junction(int mid_direction, int directions, int junctions_until_turn, int direction_of_turn, int no_junctions_pass){
  old_mid_direction = mid_direction;   
    //if mobot is facing the wrong direction, turn 180
    if (directions != old_direction){
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
      old_direction = directions;    
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
          drive_to_junction(0, 0, 0, 1, 1);   ////////////

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
        drive_to_junction(0, 0, 0, 1, 1);

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
void junction_1_directions(int mid_direction, int directions, int direction_of_turn){
    old_direction = directions; 
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
  drive_to_junction(mid_direction, directions, 0, 2, 2);
}
