#define POT_PIN 34   // ADC1 channel 6 on ESP32
const float VCC = 3.3;  // Reference voltage

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Potentiometer ADC with Voltage Reading");
}

void loop() {
  int adcValue = analogRead(POT_PIN);  // ADC reading (0-4095)
  float voltage = (adcValue / 4095.0) * VCC;  // Convert ADC to voltage

  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print("  |  Voltage: ");
  Serial.print(voltage, 2);  // 2 decimal places
  Serial.println(" V");

  delay(500);
}
