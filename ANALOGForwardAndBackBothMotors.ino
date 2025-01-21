int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

// the setup routine runs once when you press reset:
void setup() {
 Serial.begin(9600);
 }
// the loop routine runs over and over again continuously:
void loop() {
 analogWrite(motor1Phase, 255); //forward left motor
 analogWrite(motor1PWM, 200); // set speed of left motor
 analogWrite(motor2Phase, 255); //forward right moto
 analogWrite(motor2PWM, 200); // set speed of right motor
 
 Serial.println("Forward"); // Display motor direction

 delay(2000); //2 seconds

 analogWrite(motor1Phase, 0); //Backward left motor
 analogWrite(motor1PWM, 255); // set speed of left motor
 analogWrite(motor2Phase, 0); //Backward left motor
 analogWrite(motor2PWM, 255); // set speed of right motor

 Serial.println("Backward"); // Display motor direction
 delay(2000); //2 seconds
 } 
