/*
  Smart Plant Watering System
  Four software improvements:
  1. Averaged sensor readings
  2. Hysteresis-based watering thresholds
  3. Pulse-and-soak watering
  4. Sensor fault detection and safety stop

  Serial baud rate: 9600
  Frontend commands: WATER and STOP
*/

#define SOIL_SENSOR A0
#define RELAY_PIN 7

// Example values: 
const int DRY_RAW = 510;
const int WET_RAW = 330;

const int DRY_START_PERCENT = 30;
const int WET_STOP_PERCENT = 45;

const unsigned long SENSOR_INTERVAL_MS = 500;
const unsigned long REPORT_INTERVAL_MS = 1000;
const unsigned long PUMP_PULSE_MS = 15000;
const unsigned long SOAK_TIME_MS = 5000;

const int MAX_PULSES = 5;
const int AVERAGE_SAMPLES = 5;

// This assumes an active-LOW relay.
// Change these if your relay module is active-HIGH.
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

enum WaterState { IDLE, PUMPING, SOAKING };
WaterState waterState = IDLE;

int rawReading = 0;
int moisturePercent = 0;
bool pumpOn = false;
bool sensorFault = false;
bool manualWaterRequest = false;

int pulseCount = 0;
int railReadingCount = 0;
int normalRecoveryCount = 0;

unsigned long stateStartedAt = 0;
unsigned long lastSensorAt = 0;
unsigned long lastReportAt = 0;
unsigned long wateringEvents = 0;

String lastEvent = "System started";

int readAveragedSensor() {
  long total = 0;

  for (int i = 0; i < AVERAGE_SAMPLES; i++) {
    total += analogRead(SOIL_SENSOR);
    delay(5);
  }

  return total / AVERAGE_SAMPLES;
}

int rawToPercent(int raw) {
  if (DRY_RAW == WET_RAW) return 0;

  long percent =
    ((long)(DRY_RAW - raw) * 100L) / (DRY_RAW - WET_RAW);

  percent = constrain(percent, 0, 100);
  return (int)percent;
}

void setPump(bool on) {
  digitalWrite(RELAY_PIN, on ? RELAY_ON : RELAY_OFF);
  pumpOn = on;
}

void stopWatering(const char* reason) {
  setPump(false);
  waterState = IDLE;
  manualWaterRequest = false;
  pulseCount = 0;
  lastEvent = reason;
}

void startPulse() {
  if (sensorFault) {
    setPump(false);
    lastEvent = "Safety stop: sensor fault";
    return;
  }

  setPump(true);
  pulseCount++;
  stateStartedAt = millis();
  waterState = PUMPING;
  lastEvent = "Watering pulse started";
}

void handleSerialCommands() {
  static String command = "";

  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      command.trim();
      command.toUpperCase();

      if (command == "STOP") {
        stopWatering("Pump stopped by user");
      }
      else if (command == "WATER") {
        if (!sensorFault) {
          manualWaterRequest = true;
          lastEvent = "Manual watering requested";
        } else {
          lastEvent = "Manual request rejected: sensor fault";
        }
      }

      command = "";
    }
    else if (command.length() < 20) {
      command += c;
    }
  }
}

void updateSensor() {
  rawReading = readAveragedSensor();

  // Detect repeated readings at the ADC limits.
  if (rawReading <= 2 || rawReading >= 1021) {
    if (railReadingCount < 20) railReadingCount++;
  } else {
    railReadingCount = 0;
  }

  if (railReadingCount >= 8) {
    sensorFault = true;
    setPump(false);
    waterState = IDLE;
    manualWaterRequest = false;
    normalRecoveryCount = 0;
    lastEvent = "Sensor fault: reading out of range";
  }
  else if (sensorFault) {
    if (rawReading > 2 && rawReading < 1021) {
      normalRecoveryCount++;
    } else {
      normalRecoveryCount = 0;
    }

    if (normalRecoveryCount >= 10) {
      sensorFault = false;
      railReadingCount = 0;
      normalRecoveryCount = 0;
      lastEvent = "Sensor readings recovered";
    }
  }

  moisturePercent = rawToPercent(rawReading);
}

void updateWateringState() {
  unsigned long now = millis();

  if (sensorFault) {
    setPump(false);
    waterState = IDLE;
    manualWaterRequest = false;
    return;
  }

  if (waterState == IDLE) {
    if (manualWaterRequest ||
        moisturePercent <= DRY_START_PERCENT) {

      manualWaterRequest = false;
      pulseCount = 0;
      wateringEvents++;
      startPulse();
    }
  }
  else if (waterState == PUMPING) {
    if (now - stateStartedAt >= PUMP_PULSE_MS) {
      setPump(false);
      stateStartedAt = now;
      waterState = SOAKING;
      lastEvent = "Pump off; soil soaking";
    }
  }
  else if (waterState == SOAKING) {
    if (now - stateStartedAt >= SOAK_TIME_MS) {
      updateSensor();

      if (sensorFault) {
        stopWatering("Safety stop: sensor fault");
      }
      else if (moisturePercent >= WET_STOP_PERCENT) {
        stopWatering("Watering complete: moisture sufficient");
      }
      else if (pulseCount >= MAX_PULSES) {
        stopWatering("Maximum pulses reached");
      }
      else {
        startPulse();
      }
    }
  }
}

void reportStatus() {
  bool waterRequired =
    sensorFault ||
    moisturePercent <= DRY_START_PERCENT ||
    waterState != IDLE;

  Serial.print("{\"raw\":");
  Serial.print(rawReading);

  Serial.print(",\"moisturePercent\":");
  Serial.print(moisturePercent);

  Serial.print(",\"pumpOn\":");
  Serial.print(pumpOn ? "true" : "false");

  Serial.print(",\"waterRequired\":");
  Serial.print(waterRequired ? "true" : "false");

  Serial.print(",\"sensorFault\":");
  Serial.print(sensorFault ? "true" : "false");

  Serial.print(",\"pulses\":");
  Serial.print(pulseCount);

  Serial.print(",\"events\":");
  Serial.print(wateringEvents);

  Serial.println("}");
}

void setup() {
  Serial.begin(9600);

  pinMode(SOIL_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Ensure pump is OFF at startup.
  setPump(false);

  delay(300);
  updateSensor();

  lastEvent = "System started";
  Serial.println("Smart Plant Watering System ready");
}

void loop() {
  handleSerialCommands();

  unsigned long now = millis();

  if (now - lastSensorAt >= SENSOR_INTERVAL_MS) {
    lastSensorAt = now;
    updateSensor();
  }

  updateWateringState();

  if (now - lastReportAt >= REPORT_INTERVAL_MS) {
    lastReportAt = now;
    reportStatus();
  }
}