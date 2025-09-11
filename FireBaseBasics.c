#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// WiFi & Firebase Config
#define WIFI_SSID "Airtel_vija_3856"
#define WIFI_PASSWORD "Air@16510"
#define API_KEY "AIzaSyAV0rS7v4HFL3Ba4pFOx37-Wnqkq1CFyQA"
#define DATABASE_URL "https://sample-ef735-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

unsigned long prevMillis = 0;
const long interval = 2000;  // 2 seconds

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 15) {
    delay(300);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi!");
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
    Serial.println("✅ Firebase auth OK");
  } else {
    Serial.printf("❌ Firebase auth failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevMillis >= interval) {
    prevMillis = currentMillis;

    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setString(&fbdo, "/message", "Hello")) {
        Serial.println("✅ Sent: Hello");
      } else {
        Serial.printf("❌ Firebase error: %s\n", fbdo.errorReason().c_str());
      }
    }
  }
}
