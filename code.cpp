
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System"
#define BLYNK_AUTH_TOKEN    "YOUR_DEVICE_AUTH_TOKEN"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Wi-Fi credentials
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// ESP32 pins
const int SOIL_PIN  = 34;
const int TANK_PIN  = 27;
const int RELAY_PIN = 26;

// Change to false if your relay is active HIGH
const bool RELAY_ACTIVE_LOW = true;

// Calibrate these using your sensor
const int DRY_RAW = 3000;
const int WET_RAW = 1300;

// Moisture hysteresis thresholds
const int DRY_THRESHOLD = 35;
const int WET_THRESHOLD = 55;

// Maximum continuous pump run
const unsigned long MAX_PUMP_MS = 30000;

// Set false if the switch is closed when the tank is LOW
const bool TANK_SWITCH_CLOSED_WHEN_WATER_PRESENT = true;

BlynkTimer timer;

// Mode and pump state
bool autoMode = true;
bool manualRequest = false;
bool pumpOn = false;
bool autoDemand = false;
bool runtimeLockout = false;

// Latest sensor values
int moisture = 0;
int soilRaw = 0;
bool waterAvailable = false;

unsigned long pumpStartedAt = 0;

// Alert state
bool dryAlertSent = false;
bool lowWaterAlertSent = false;

void setPump(bool on) {
  if (on && !waterAvailable) {
    on = false;
  }

  if (on && runtimeLockout) {
    on = false;
  }

  if (pumpOn == on) {
    return;
  }

  pumpOn = on;

  if (pumpOn) {
    pumpStartedAt = millis();
  }

  int relayLevel;

  if (RELAY_ACTIVE_LOW) {
    relayLevel = pumpOn ? LOW : HIGH;
  } else {
    relayLevel = pumpOn ? HIGH : LOW;
  }

  digitalWrite(RELAY_PIN, relayLevel);

  if (Blynk.connected()) {
    Blynk.virtualWrite(V4, pumpOn ? 1 : 0);
  }

  Serial.println(pumpOn ? "Pump ON" : "Pump OFF");
}

void readSensors() {
  // Read analog soil sensor
  soilRaw = analogRead(SOIL_PIN);

  // Higher raw = drier, for this example calibration
  moisture = map(soilRaw, DRY_RAW, WET_RAW, 0, 100);
  moisture = constrain(moisture, 0, 100);

  // Float switch: INPUT_PULLUP means closed = LOW
  bool switchClosed = (digitalRead(TANK_PIN) == LOW);

  if (TANK_SWITCH_CLOSED_WHEN_WATER_PRESENT) {
    waterAvailable = switchClosed;
  } else {
    waterAvailable = !switchClosed;
  }

  Serial.print("Raw soil: ");
  Serial.print(soilRaw);
  Serial.print(" | Moisture: ");
  Serial.print(moisture);
  Serial.print("% | Water available: ");
  Serial.println(waterAvailable ? "YES" : "NO");

  // Notify only on entering an alert condition
  if (moisture < DRY_THRESHOLD && !dryAlertSent) {
    if (Blynk.connected()) {
      Blynk.logEvent(
        "soil_dry",
        "Soil moisture is below the configured threshold."
      );
    }
    dryAlertSent = true;
  }

  if (moisture >= WET_THRESHOLD) {
    dryAlertSent = false;
    runtimeLockout = false;
  }

  if (!waterAvailable && !lowWaterAlertSent) {
    if (Blynk.connected()) {
      Blynk.logEvent(
        "low_water",
        "Water tank is low. Pump operation is disabled."
      );
    }
    lowWaterAlertSent = true;
  }

  if (waterAvailable) {
    lowWaterAlertSent = false;
  }

  // Immediate local safety response
  if (!waterAvailable) {
    setPump(false);
  }

  // Automatic watering with hysteresis
  if (autoMode) {
    if (moisture < DRY_THRESHOLD) {
      autoDemand = true;
    }

    if (moisture >= WET_THRESHOLD) {
      autoDemand = false;
    }

    // A runtime timeout prevents immediate restarting.
    // Reset lockout after soil reaches the wet threshold.
    if (runtimeLockout) {
      autoDemand = false;
    }

    setPump(autoDemand && waterAvailable);
  } else {
    // Manual mode still respects tank and runtime safety
    setPump(manualRequest && waterAvailable);
  }

  // Update Blynk dashboard
  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, moisture);
    Blynk.virtualWrite(V1, waterAvailable ? 1 : 0);
    Blynk.virtualWrite(V2, autoMode ? 1 : 0);
    Blynk.virtualWrite(V4, pumpOn ? 1 : 0);
  }
}

void checkPumpRuntime() {
  if (pumpOn && millis() - pumpStartedAt >= MAX_PUMP_MS) {
    Serial.println("Maximum pump runtime reached.");

    runtimeLockout = true;
    autoDemand = false;
    manualRequest = false;

    setPump(false);
  }
}

// Dashboard: V2 = 1 for auto, 0 for manual
BLYNK_WRITE(V2) {
  autoMode = (param.asInt() == 1);

  if (autoMode) {
    manualRequest = false;
  } else {
    autoDemand = false;
  }

  readSensors();
}

// Dashboard: V3 = manual pump request
BLYNK_WRITE(V3) {
  manualRequest = (param.asInt() == 1);

  if (manualRequest) {
    autoMode = false;
  }

  readSensors();
}

// Synchronize dashboard state on reconnection
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2, V3);
}

void setup() {
  Serial.begin(115200);

  pinMode(SOIL_PIN, INPUT);
  pinMode(TANK_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);

  // Set relay OFF before starting Wi-Fi
  digitalWrite(
    RELAY_PIN,
    RELAY_ACTIVE_LOW ? HIGH : LOW
  );

  analogReadResolution(12);

  Blynk.begin(
    BLYNK_AUTH_TOKEN,
    ssid,
    pass
  );

  timer.setInterval(2000L, readSensors);
  timer.setInterval(500L, checkPumpRuntime);

  readSensors();
}

void loop() {
  Blynk.run();
  timer.run();
}
