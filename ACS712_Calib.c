#define VIN A0
const float VCC = 5.0
const int model = 2;


float sensitivity[] = {0.185, 0.100, 0.066};

float cutOffLimit = 1.01;
const float QVV = 0.5 * VCC;
float voltage;

void setup()
{
   Serial.begin(9600);
   Serial.println("Project!");
}

void loop()
{
    float voltage_raw = ((5.0) / 1023.0) * analogRead(VIN);
    voltage = voltage_raw - QOV + 0.012;
    float current = voltage / sensitivity[model];

if (abs(current) > cutOffLimit)
{
  Serial.print("V: ");
  Serial.print(voltage,3);
  Serial.print("V, I: ");
  Serial.print(current,2);
  Serial.println("A");
}
else
{
  Serial.println("No current");
}

delay(500);
}
