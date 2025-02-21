#include <WiFi.h>
#include <WebServer.h>

//using pointers
//const char* ssid = "eir74872544";      
//const char* password = "6ECx8p8cXB"; 
const char* ssid = "";      
const char* password = ""; 
//136.206.12.161

WebServer server(80);
int imageNumber = 1;  // Start with Image 1


// Serve the HTML page
void handleRoot() {
    String html = "";
  
    // Meta tag for auto refresh every 2 seconds
    html += "<meta http-equiv='refresh' content='2'>";  // Refresh every 2 seconds


    // Check the image number and change the image accordingly
    if (imageNumber == 1) {
        html += "<img src='https://github.com/LauraEEcock/Innovation1Attempt1/blob/main/Green.png?raw=true' width='600'>";
    } 
    else if (imageNumber == 2) {
        html += "<img src='https://github.com/LauraEEcock/Innovation1Attempt1/blob/main/FinalONE.png?raw=true' width='1513'>";

    }
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// Handle the image number (Green or Red)
void handleGetImage() {
    server.send(200, "text/plain", String(imageNumber));
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    // A part of the wifi library
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup web server routes
    server.on("/", handleRoot);         // Root page with the image
    server.on("/getImage", handleGetImage); // Get current image number

    server.begin();
}

void loop() {
static unsigned long lastChangeTime = 0; // To track when to toggle the image
    unsigned long currentMillis = millis();

    // Toggle between Green (1) and Red (2) every 2 seconds
    if (currentMillis - lastChangeTime >= 2000) {  // 2 seconds passed
        imageNumber = (imageNumber == 1) ? 2 : 1;  // Toggle between 1 (Green) and 2 (Red)
        lastChangeTime = currentMillis;  // Reset the timer
    }

    // Handle client requests
    server.handleClient();  // This keeps checking if there are incoming requests
}
