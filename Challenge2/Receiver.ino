// ----------------------------------------------------------------------------
// ESP-NOW network + WiFi gateway to the Internet
// ----------------------------------------------------------------------------
// ESP-NOW receiver acting as a WiFi gateway to the Internet
// ----------------------------------------------------------------------------

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

typedef struct Junctions {
  int junction1;
} Junctions;

Junctions myjun;

// ----------------------------------------------------------------------------
// WiFi handling
// ----------------------------------------------------------------------------
/*
char* WIFI_AP_SSID = "SSIDs";
char* WIFI_AP_PASS = "PASSWORD";

char* WIFI_SSID = "SSID";
char* WIFI_PASS = "PASSWORD";*/

void initWiFi() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE); // Otherwise wifi card goes to sleep if inactive
  //connect_esp32_wifi_network(WIFI_SSID, WIFI_PASS);
  //WiFi.printDiag(Serial);
  //WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS); // Start local access point
}

void connect_esp32_wifi_network(char* ssid, char* password) {
  Serial.println("------------------------------------------------------------");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Ok\nConnected!");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.print("; MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("------------------------------------------------------------\n");
  Serial.print("WiFi channel: ");
  Serial.println(WiFi.channel());
}

// ----------------------------------------------------------------------------
// ESP-NOW handling
// ----------------------------------------------------------------------------

void onReceive(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    //if (len == sizeof(myjun)) {  // Ensure correct data size
        memcpy(&myjun, data, sizeof(myjun));  // Copy received data into struct
        Serial.printf("Junction1: %d\n", myjun.junction1);
}

void initEspNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP NOW failed to initialize");
        while (1);
    }
    esp_now_register_recv_cb(onReceive);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    initWiFi();
    initEspNow();
}


void loop() {} 
// All intructions related to WiFi gateway should be executed here or in setup()
// It is not possible to execute wifi-related instructions in the callback function since it needs to use the built-in wifi card until it returns.
