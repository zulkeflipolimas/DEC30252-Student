#define LED_PIN 2
#define BUTTON_PIN 32

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Guna internal pull-up resistor ESP32
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("ESP32 Push Button LED Control Started");
}

void loop() {

  int buttonState = digitalRead(BUTTON_PIN);

  // Bila button ditekan, pin jadi LOW
  if (buttonState == LOW) {

    digitalWrite(LED_PIN, HIGH);
    Serial.println("Button Pressed - LED ON");

  } else {

    digitalWrite(LED_PIN, LOW);
    Serial.println("Button Released - LED OFF");
  }

  delay(100);
}
