#include <Arduino.h>
#include <WiFi.h>
#include <cstring>

// dane Wi-Fi
const char* ssid = "atlantyda";
const char* password = "kotl1234";
const IPAddress LOCAL_IP(192, 168, 1, 220);
const IPAddress GATEWAY(192, 168, 1, 1);
const IPAddress SUBNET(255, 255, 255, 0);
const IPAddress PRIMARY_DNS(192, 168, 1, 1);
const IPAddress SECONDARY_DNS(8, 8, 8, 8);

// Definicje pinow
#define PRZEKAZNIK 13

// Definicje timingowe
constexpr unsigned long RELAY_LATCH_MS = 10000;

unsigned long relayLatchStartedAt = 0;
bool relayLatched = false;
unsigned long lastMillis = 0;
unsigned long relay_on_time = 10000; 

WiFiServer wifiServer(100);

void setup() {
  Serial.begin(115200);

  pinMode(PRZEKAZNIK, OUTPUT);
  digitalWrite(PRZEKAZNIK, LOW);

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

  if (relayLatched && now - relayLatchStartedAt >= RELAY_LATCH_MS) {
    digitalWrite(PRZEKAZNIK, LOW);
    relayLatched = false;
    Serial.println("Relay latch expired");
  }

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
      digitalWrite(PRZEKAZNIK, HIGH);
      relayLatchStartedAt = now;
      relayLatched = true;
      Serial.println("Relay latched ON for 10s");
    } 
    else if (request.indexOf("OFF") > -1) {
      digitalWrite(PRZEKAZNIK, LOW);
      relayLatched = false;
      Serial.println("Relay turned OFF");
    }
    else {
      Serial.println("Unknown command");
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
