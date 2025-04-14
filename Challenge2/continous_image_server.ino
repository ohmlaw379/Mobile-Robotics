#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Structure to hold the received data
typedef struct Junctions {
  int junction1;
} Junctions;

Junctions myjun;

// WiFi configuration
const char* ssid = "iot";
const char* password = "xenophobia68dermoplasty";

// Image URLs corresponding to numbers
String imageURLs[] = {
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_0.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_1.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_2.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_3.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_4.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_5.jpg"
};

// ----------------------------------------------------------------------------
// WiFi handling
// ----------------------------------------------------------------------------
void initWiFi() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE); // Disable WiFi sleep mode
  WiFi.begin(ssid, password);    // Connect to WiFi network

  // Wait until the WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ----------------------------------------------------------------------------
// ESP-NOW handling
// ----------------------------------------------------------------------------
void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (len == sizeof(myjun)) {  // Ensure correct data size
        memcpy(&myjun, data, sizeof(myjun));  // Copy received data into struct
        Serial.printf("Received Junction1: %d\n", myjun.junction1);

        // Ensure the received number is valid (1-3)
        if (myjun.junction1 <= 5) {
            // Display the corresponding image URL
            Serial.println("Image URL: " + imageURLs[myjun.junction1 - 1]);
        } else {
            Serial.println("Invalid number received. Please send a number between 1 and 3.");
        }
    }
}

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP NOW failed to initialize");
        while (1);
    }
    esp_now_register_recv_cb(onReceive);  // Register the callback to handle incoming data
}

void setup() {
    Serial.begin(115200);
    delay(500);
    initWiFi();      // Initialize WiFi and connect to the network
    initEspNow();    // Initialize ESP-NOW for communication
}

void loop() {
    // Main loop does nothing as ESP-NOW communication happens asynchronously
    // The code works by triggering the onReceive callback when data is received
    delay(10);  // Small delay to ensure the loop keeps running, allowing ESP-NOW to function
}
