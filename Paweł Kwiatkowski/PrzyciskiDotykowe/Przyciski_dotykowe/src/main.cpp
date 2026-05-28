#include <Arduino.h>

// Definicje pinow
uint16_t przyciski[12] = {12, 33, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26};
byte przycisk_count = 0;

#define PRZEKAZNIK_1 27

// Definicje timingowe
constexpr unsigned long SCAN_INTERVAL_MS = 500;
constexpr unsigned long RESET_PHASE_MS = 1000;

enum ResetState : byte {
  RESET_IDLE,
  RESET_LOW_PHASE,
  RESET_HIGH_PHASE
};

ResetState resetState = RESET_IDLE;
unsigned long lastScanTime = 0;
unsigned long resetPhaseStart = 0;

void setup() {
  Serial.begin(115200);

  for (byte i = 0; i < 12; i++) {
    pinMode(przyciski[i], INPUT);
  }
  pinMode(PRZEKAZNIK_1, OUTPUT);
  digitalWrite(PRZEKAZNIK_1, HIGH);
}

void loop() {
  // Timing
  unsigned long now = millis();

  if (resetState == RESET_LOW_PHASE) {
    if (now - resetPhaseStart >= RESET_PHASE_MS) {
      digitalWrite(PRZEKAZNIK_1, HIGH);
      resetPhaseStart = now;
      resetState = RESET_HIGH_PHASE;
    }
    return;
  }

  if (resetState == RESET_HIGH_PHASE) {
    if (now - resetPhaseStart >= RESET_PHASE_MS) {
      resetState = RESET_IDLE;
    }
    return;
  }

  if (now - lastScanTime < SCAN_INTERVAL_MS) {
    return;
  } lastScanTime = now;

  // Główna pętla
  for (byte i = 0; i < 12; i++) {
    if (digitalRead(przyciski[i]) == HIGH) {
      Serial.println("Przycisk " + String(i) + " jest wciśnięty");
      przycisk_count++;
    }
  }

  if (przycisk_count >= 6) {
    Serial.println("-Reset-");
    digitalWrite(PRZEKAZNIK_1, LOW);

    resetPhaseStart = now;
    resetState = RESET_LOW_PHASE;
    przycisk_count = 0;

  } else {
    Serial.println("-Brak resetu-");
    digitalWrite(PRZEKAZNIK_1, HIGH);

    przycisk_count = 0;
  }
}
