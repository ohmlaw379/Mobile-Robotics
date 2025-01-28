#include <WiFi.h> // Use WiFi.h for ESP32; ESP8266WiFi.h for ESP8266

char ssid[] = "conway_tv-4573";
char password[] = "5swjclt03h";
char server[] = "3.250.38.184";
int port = 8000;
int position = 0;

WiFiClient client;

// Function to connect to WiFi
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

// Function to connect to the server
bool connectToServer() {
    if (!client.connect(server, port)) {
        Serial.println("Error connecting to server");
        return false;
    }
    Serial.println("Connected to server");
    return true;
}

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

void setup() {
    Serial.begin(115200);
    delay(1000);
    connectToWiFi();

    while (true) { // Keep communicating until the last stop
        if (connectToServer()) {
            // Prepare POST body
            String postBody = "position=" + String(position);

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
                        break; // Exit the loop if the final stop is reached
                    } else {
                        position = destination; // Update position for the next iteration
                        Serial.println("Next destination: " + String(destination));
                    }
                }
            } else {
                Serial.println("POST request failed. Status code: " + String(statusCode));
                break; // Exit the loop if a request fails
            }

            client.stop(); // Close the connection before the next iteration
            delay(1000); // Add a small delay to prevent overwhelming the server
        } else {
            Serial.println("Failed to connect to server. Retrying...");
            delay(3000); // Wait before retrying the server connection
        }
    }
}

void loop() {
    // Nothing needed here as the main logic is in setup()
}
