#define BUZZER_PIN 8
#define MOTOR_ENA_PIN 9

// Hardcoded accident zone coordinates and speed limit
const float ACCIDENT_LAT = 12.9716;
const float ACCIDENT_LNG = 77.5946;
const float SPEED_LIMIT = 20.0; // km/h
const float WARNING_DISTANCE = 200.0; // meters
const float ACCIDENT_ZONE_RADIUS = 50.0; // meters

// Variables for current GPS data
float currentLat = 0.0;
float currentLng = 0.0;
float currentSpeed = 0.0;

int motorSpeed = 200;  // PWM duty cycle for motor speed
bool warningTriggered = false;  // Prevent repeated buzzers

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_ENA_PIN, OUTPUT);
  analogWrite(MOTOR_ENA_PIN, motorSpeed);
  Serial.println("Speed Limiter System Initialized");
}

void loop() {
  // Receive GPS data from Python
  if (Serial.available() > 0) {
    String gpsData = Serial.readStringUntil('\n');
    gpsData.trim();
    int comma1 = gpsData.indexOf(',');
    int comma2 = gpsData.lastIndexOf(',');
    if (comma1 > 0 && comma2 > comma1) {
      currentLat = gpsData.substring(0, comma1).toFloat();
      currentLng = gpsData.substring(comma1 + 1, comma2).toFloat();
      currentSpeed = gpsData.substring(comma2 + 1).toFloat();
      Serial.print("Received GPS: ");
      Serial.print(currentLat, 6);
      Serial.print(", ");
      Serial.print(currentLng, 6);
      Serial.print(", Speed: ");
      Serial.println(currentSpeed);
    }
  }

  float distance = calculateDistance(currentLat, currentLng, ACCIDENT_LAT, ACCIDENT_LNG);
  Serial.print("Distance to accident zone: ");
  Serial.print(distance);
  Serial.println(" meters");

  // Warning buzzer logic
  if (distance <= WARNING_DISTANCE && distance > ACCIDENT_ZONE_RADIUS) {
    if (!warningTriggered) {
      tone(BUZZER_PIN, 1000, 1000);  // Warning buzzer for 1 second
      Serial.println("Warning buzzer ON");
      warningTriggered = true;
    }
  }
  else if (distance <= ACCIDENT_ZONE_RADIUS) {
    if (currentSpeed > SPEED_LIMIT) {
      tone(BUZZER_PIN, 2000);  // Continuous danger buzzer
      Serial.println("Danger buzzer ON (overspeeding in accident zone)");
      // Gradually reduce speed smoothly
      if (motorSpeed > 100) {
        motorSpeed -= 10;
        Serial.print("Reducing motor speed to: ");
        Serial.println(motorSpeed);
      }
    }
    else {
      noTone(BUZZER_PIN);
      Serial.println("Speed within limit, buzzer OFF");
      motorSpeed = 200; // Reset motor speed
    }
  }
  else {
    noTone(BUZZER_PIN);
    warningTriggered = false;  // Reset warning
    motorSpeed = 200;  // Normal speed
  }

  analogWrite(MOTOR_ENA_PIN, motorSpeed);
  delay(1000);
}

// Haversine formula for distance in meters
float toRadians(float degree) {
  return degree * 3.14159265359 / 180.0;
}

float calculateDistance(float lat1, float lng1, float lat2, float lng2) {
  float R = 6371000.0;  // Earth radius in meters
  float dLat = toRadians(lat2 - lat1);
  float dLng = toRadians(lng2 - lng1);
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(toRadians(lat1)) * cos(toRadians(lat2)) *
            sin(dLng / 2) * sin(dLng / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}
