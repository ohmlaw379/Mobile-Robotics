#include <WiFi.h>

const char* ssid = "iot";
const char* password = "xenophobia68dermoplasty";

String imageURLs[] = {
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_0.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_1.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_2.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_3.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_4.jpg",
    "https://raw.githubusercontent.com/ohmlaw379/images/main/MAP_5.jpg"
};

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    randomSeed(analogRead(0)); // Seed the random number generator
}

void loop() {
    // Generate a random number between 1 and 3 (inclusive)
    int randomIndex = random(0, 5); // Random index between 0 and 2 (for 3 images)

    // Print the random number and corresponding image URL
    Serial.print("Random Number: ");
    Serial.println(randomIndex + 1); // Display number 1-3
    Serial.println("Image URL: " + imageURLs[randomIndex]);  // Display the corresponding image URL

    // Wait for a while before selecting the next random image
    delay(5000);  // Wait for 5 seconds (adjust as needed)
}
