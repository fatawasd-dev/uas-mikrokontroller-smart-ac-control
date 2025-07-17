#define BLYNK_TEMPLATE_ID "TMPL6XWShyhEH"
#define BLYNK_TEMPLATE_NAME "Smart AC Control"
#define BLYNK_AUTH_TOKEN "sOcwugBrFDxjzsMCMNRSiDs_qa6feuPW"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// DHT sensor setup
#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Output pins
#define LED_AC_PIN 19
#define RELAY_PIN 13

// Blynk virtual pins
#define VPIN_TEMPERATURE V1
#define VPIN_AC_STATUS V3         // Displays "Menyala"/"Mati"
#define VPIN_AC_CONTROL V0        // Manual ON/OFF button
#define VPIN_MODE_SELECTOR V6     // 0 = Manual, 1 = Automatic
#define VPIN_AUTO_ON_TEMP V4     // Suhu untuk nyalakan AC otomatis
#define VPIN_AUTO_OFF_TEMP V5    // Suhu untuk matikan AC otomatis

// System state
bool acStatus = false;           // Current AC status (true = ON)
bool manualControl = false;     // Manual control flag
float autoOnTemp = 30.0;        // Temperature to turn on AC automatically
float autoOffTemp = 26.0;       // Temperature to turn off AC automatically

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Configure output pins
  pinMode(LED_AC_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(LED_AC_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Welcome animation
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Smart");
  lcd.setCursor(0, 1);
  lcd.print("AC Control");
  delay(2000);
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayLeft();
    delay(300);
  }
  lcd.clear();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting to");
    lcd.setCursor(0, 1);
    lcd.print("WiFi...");
    delay(1000);
    lcd.clear();
    Serial.println("Connecting to WiFi...");
  }

  lcd.setCursor(0, 0);
  lcd.print("Connected to WiFi");
  delay(2000);
  lcd.clear();

  // Start Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Sync virtual pins from Blynk app
  Blynk.syncVirtual(VPIN_MODE_SELECTOR);   // Restore Auto/Manual mode
  Blynk.syncVirtual(VPIN_AC_CONTROL);      // Restore manual AC switch state
  Blynk.syncVirtual(VPIN_AUTO_ON_TEMP);   // Restore suhu ON
  Blynk.syncVirtual(VPIN_AUTO_OFF_TEMP);  // Restore suhu OFF
}

// Automatic AC control based on temperature
void controlACAuto(float temperature) {
  if (manualControl) return;  // Skip if manual mode is active

  if (temperature >= autoOnTemp && !acStatus) {
    acStatus = true;
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_AC_PIN, HIGH);
    Blynk.virtualWrite(VPIN_AC_STATUS, "Menyala");
  } else if (temperature <= autoOffTemp && acStatus) {
    acStatus = false;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_AC_PIN, LOW);
    Blynk.virtualWrite(VPIN_AC_STATUS, "Mati");
  }
}

// Manual AC control via Blynk button (V0)
BLYNK_WRITE(VPIN_AC_CONTROL) {
  if (!manualControl) return;  // Only respond in manual mode

  int val = param.asInt();
  acStatus = (val == 1);

  digitalWrite(RELAY_PIN, acStatus ? HIGH : LOW);
  digitalWrite(LED_AC_PIN, acStatus ? HIGH : LOW);
  Blynk.virtualWrite(VPIN_AC_STATUS, acStatus ? "Menyala" : "Mati");
}

// Mode selector handler (V6): 0 = Manual, 1 = Auto
BLYNK_WRITE(VPIN_MODE_SELECTOR) {
  int mode = param.asInt();
  manualControl = (mode == 0); // Manual mode = true

  if (!manualControl) {
    // Switched to auto mode: reset AC
    acStatus = false;
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_AC_PIN, LOW);
    Blynk.virtualWrite(VPIN_AC_STATUS, "Mati");
    Blynk.virtualWrite(VPIN_AC_CONTROL, 0);  // Reset manual button
  } else {
    // Switched to manual mode: sync AC button state
    Blynk.syncVirtual(VPIN_AC_CONTROL);
  }
}

// Blynk input: suhu minimum untuk menyalakan AC otomatis (V4)
BLYNK_WRITE(VPIN_AUTO_ON_TEMP) {
  float val = param.asFloat();
  autoOnTemp = val;
  Serial.print("Auto ON Temp set to: ");
  Serial.println(autoOnTemp);
}

// Blynk input: suhu maksimum untuk mematikan AC otomatis (V5)
BLYNK_WRITE(VPIN_AUTO_OFF_TEMP) {
  float val = param.asFloat();
  autoOffTemp = val;
  Serial.print("Auto OFF Temp set to: ");
  Serial.println(autoOffTemp);
}

void loop() {
  Blynk.run();

  // Read temperature
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature);

  // Run automatic AC control if enabled
  controlACAuto(temperature);

  // Display temperature and AC status on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("C     ");

  lcd.setCursor(0, 1);
  lcd.print("AC:");
  lcd.print(acStatus ? "Menyala " : "Mati    ");
}
