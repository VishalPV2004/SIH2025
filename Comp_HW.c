#define VIN1 39   // GPIO39 = ADC1_CH3
#define VIN2 36   // GPIO36 = ADC1_CH0

float zeroVoltage1 = 0;
float zeroVoltage2 = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // 0–4095

  // Calibrate Sensor1
  long sum1 = 0;
  for (int i = 0; i < 500; i++) {
    sum1 += analogRead(VIN1);
    delay(2);
  }
  int adcZero1 = sum1 / 500;
  zeroVoltage1 = (adcZero1 * 3.3) / 4095.0;

  // Calibrate Sensor2
  long sum2 = 0;
  for (int i = 0; i < 500; i++) {
    sum2 += analogRead(VIN2);
    delay(2);
  }
  int adcZero2 = sum2 / 500;
  zeroVoltage2 = (adcZero2 * 3.3) / 4095.0;

  Serial.print("Calibrated Zero Voltage Sensor1: ");
  Serial.println(zeroVoltage1, 3);
  Serial.print("Calibrated Zero Voltage Sensor2: ");
  Serial.println(zeroVoltage2, 3);
}

void loop() {
  // Sensor1
  int adc1 = analogRead(VIN1);
  float voltage1 = (adc1 * 3.3) / 4095.0;
  float current1 = (voltage1 - zeroVoltage1) / 0.185;

  if (fabs(current1) < 0.1) current1 = 0;

  // Sensor2
  int adc2 = analogRead(VIN2);
  float voltage2 = (adc2 * 3.3) / 4095.0;
  float current2 = (voltage2 - zeroVoltage2) / 0.185;

  if (fabs(current2) < 0.1) current2 = 0;

  // Display both
  Serial.print("Current1: ");
  Serial.print(current1, 3);
  Serial.print(" A   |   Current2: ");
  Serial.print(current2, 3);
  Serial.println(" A");

  delay(500);
}
