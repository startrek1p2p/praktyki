#include <Arduino.h>
#include <WiFi.h>
#include <cstring>

// dane Wi-Fi
const char* ssid = "atlantyda";
const char* password = "kotl1234";
const IPAddress LOCAL_IP(192, 168, 1, 222);
const IPAddress GATEWAY(192, 168, 1, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress PRIMARY_DNS(192, 168, 1, 1);
const IPAddress SECONDARY_DNS(8, 8, 8, 8);

// Definicje pinow
constexpr byte LICZBA_PRZYCISKOW = 12;
constexpr byte DLUGOSC_KOMBINACJI = 6;

uint16_t przyciski[LICZBA_PRZYCISKOW] = {12, 33, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26};
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

uint8_t ostatni_przycisk = -1;

uint16_t poprawna_kombinacja[DLUGOSC_KOMBINACJI] = {8, 11, 7, 6, 10, 9};
uint16_t wpisana_kombinacja[DLUGOSC_KOMBINACJI] = {0, 0, 0, 0, 0, 0};
bool poprzedni_stan_przycisku[LICZBA_PRZYCISKOW] = {LOW};

bool czyTablicaZawiera(const uint16_t tablica[], byte rozmiar, uint16_t wartosc) {
  for (byte i = 0; i < rozmiar; i++) {
    if (tablica[i] == wartosc) {
      return true;
    }
  }

  return false;
}

bool czyWpisanaKombinacjaJestPoprawna() {
  for (byte i = 0; i < DLUGOSC_KOMBINACJI; i++) {
    if (!czyTablicaZawiera(poprawna_kombinacja, DLUGOSC_KOMBINACJI, wpisana_kombinacja[i])) {
      return false;
    }
  }

  return true;
}

void wyczyscWpisanaKombinacje() {
  memset(wpisana_kombinacja, 0, sizeof(wpisana_kombinacja));
  przycisk_count = 0;
  ostatni_przycisk = -1;
}

WiFiServer wifiServer(100);

void setup() {
  Serial.begin(115200);

  for (byte i = 0; i < LICZBA_PRZYCISKOW; i++) {
    pinMode(przyciski[i], INPUT);
  }

  pinMode(PRZEKAZNIK_1, OUTPUT);
  digitalWrite(PRZEKAZNIK_1, HIGH);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET, PRIMARY_DNS, SECONDARY_DNS)) {
    Serial.println("Static IP configuration failed");
  }

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the TCP server
  wifiServer.begin();
  Serial.println("Server started");
}

void loop() {
  unsigned long now = millis();

    // Check if a client has connected
  WiFiClient client = wifiServer.available();

  if (client) {
    Serial.println("New Client connected.");

    // Read the data sent by the client
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // If the client sends a newline, the message is complete
        if (c == '\n') {
          break; 
        }
      }
    }

    Serial.print("Received: ");
    Serial.println(request);
    if (request.indexOf("ON") > -1) {
      digitalWrite(PRZEKAZNIK_1, HIGH);
      client.println("LED turned ON");
    } 
    else if (request.indexOf("OFF") > -1) {
      digitalWrite(PRZEKAZNIK_1, LOW);
      client.println("LED turned OFF");
    }
    else {
      client.println("Unknown command");
    }

  client.stop();
  Serial.println("Client disconnected");

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
  }
  lastScanTime = now;

  // Rejestruj nowe nacisniecie
  for (byte i = 0; i < LICZBA_PRZYCISKOW; i++) {
    bool aktualny_stan = digitalRead(przyciski[i]) == HIGH;

    if (aktualny_stan && !poprzedni_stan_przycisku[i] && przycisk_count < DLUGOSC_KOMBINACJI) {
      if (!czyTablicaZawiera(wpisana_kombinacja, przycisk_count, i)) {
        Serial.println("Przycisk " + String(i) + " jest wcisniety");
        wpisana_kombinacja[przycisk_count] = i;
        ostatni_przycisk = i;
        przycisk_count++;
      }
    }

    poprzedni_stan_przycisku[i] = aktualny_stan;

    if (przycisk_count >= DLUGOSC_KOMBINACJI) {
      break;
    }
  }

  if (przycisk_count >= DLUGOSC_KOMBINACJI && czyWpisanaKombinacjaJestPoprawna()) {
    Serial.println("-Wpisano poprawna kombinacje!-");
    digitalWrite(PRZEKAZNIK_1, LOW);
    wyczyscWpisanaKombinacje();
    delay(1000);
    digitalWrite(PRZEKAZNIK_1, HIGH);
    delay(1000);
  } else if (przycisk_count >= DLUGOSC_KOMBINACJI) {
    Serial.println("-Wpisano niepoprawna kombinacje-");
    digitalWrite(PRZEKAZNIK_1, LOW);
    resetPhaseStart = now;
    resetState = RESET_LOW_PHASE;
    wyczyscWpisanaKombinacje();
    }
  }
}
