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

// ACS712 sensor pins
#define VIN1 39   // GPIO39 = ADC1_CH3
#define VIN2 36   // GPIO36 = ADC1_CH0

float zeroVoltage1 = 0;
float zeroVoltage2 = 0;

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

float calibrateSensor(int pin) {
  long sum = 0;
  for (int i = 0; i < 500; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  int adcZero = sum / 500;
  return (adcZero * 3.3) / 4095.0;
}

float readCurrent(int pin, float zeroV) {
  int adc = analogRead(pin);
  float voltage = (adc * 3.3) / 4095.0;
  float current = (voltage - zeroV) / 0.185;  // 0.185 V/A for ACS712-5A
  if (fabs(current) < 0.1) current = 0;
  return current;
}

void setup() {
  Serial.begin(115200);
  connectWiFi();

  analogReadResolution(12);  // 0–4095

  // Calibrate both sensors
  zeroVoltage1 = calibrateSensor(VIN1);
  zeroVoltage2 = calibrateSensor(VIN2);

  Serial.print("Calibrated Zero Voltage (Sensor1): ");
  Serial.println(zeroVoltage1, 3);
  Serial.print("Calibrated Zero Voltage (Sensor2): ");
  Serial.println(zeroVoltage2, 3);

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

    float current1 = readCurrent(VIN1, zeroVoltage1);
    float current2 = readCurrent(VIN2, zeroVoltage2);

    Serial.print("Sensor1 Current: ");
    Serial.print(current1, 3);
    Serial.println(" A");

    Serial.print("Sensor2 Current: ");
    Serial.print(current2, 3);
    Serial.println(" A");

    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setFloat(&fbdo, "/sensor1/current", current1)) {
        Serial.println("✅ Sent Sensor1");
      } else {
        Serial.printf("❌ Firebase error (Sensor1): %s\n", fbdo.errorReason().c_str());
      }

      if (Firebase.RTDB.setFloat(&fbdo, "/sensor2/current", current2)) {
        Serial.println("✅ Sent Sensor2");
      } else {
        Serial.printf("❌ Firebase error (Sensor2): %s\n", fbdo.errorReason().c_str());
      }
    }
  }
}
