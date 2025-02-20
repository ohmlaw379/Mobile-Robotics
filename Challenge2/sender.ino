#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>


constexpr uint8_t ESP_NOW_RECEIVER[] = {0x48, 0x27, 0xE2, 0x15, 0x50, 0x78};
esp_now_peer_info_t peerInfo;
bool channelFound = false;
uint8_t channel = 1;
uint32_t last;

typedef struct Junctions {
  int junction1;
} Junctions;

Junctions myjun;

void initEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);
  if (esp_now_init() != ESP_OK) {
      Serial.println("ESP NOW failed to initialize");
      while (1);
  }

  memcpy(peerInfo.peer_addr, ESP_NOW_RECEIVER, 6);
  // peerInfo.ifidx   = ESP_IF_WIFI_STA;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("ESP NOW pairing failure");
      while (1);
  }
  
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  
  esp_now_register_send_cb(onDataSent);
}



void tryNextChannel() {
  Serial.println("Changing channel from " + String(channel) + " to " + String(channel+1));
  channel = channel % 13 + 1;
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
}

// ----------------------------------------------------------------------------
// Callback function for when data is sent  
// ----------------------------------------------------------------------------
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

void send_Message(){
  if (millis() - last > 2000) {
      myjun.junction1 = random(0,5);
      esp_now_send(ESP_NOW_RECEIVER, (uint8_t *) &myjun, sizeof(Junctions));
      last = millis();
  }
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  //connect_esp32_wifi_network(WIFI_AP_SSID, WIFI_AP_PASS);
  //esp_wifi_set_ps(WIFI_PS_NONE);
  //WiFi.printDiag(Serial);
  initEspNow();
}

void loop() {
  send_Message();
  delay(1000);
}
