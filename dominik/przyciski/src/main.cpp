#include <Arduino.h>
#include <string.h>

#define P1 25
#define P2 26
#define P3 19
#define P4 18
#define RELAY 35


int wcisniete = -1;
int wcisniete_policz = 0;
int prawidlowa_kombinacja[6] = {1, 1, 1, 2, 3, 4};
int wpisana_kombinacja[6] = {0, 0, 0, 0, 0, 0};
void resetKombinacji() {
  for (int i = 0; i < 6; i++) {
    wpisana_kombinacja[i] = 0;
  }
  wcisniete_policz = 0;
}

bool sprawdzKombinacje() {
  for (int i = 0; i < 6; i++) {
    if (wpisana_kombinacja[i] != prawidlowa_kombinacja[i]) {
      return false;
    }
  }
  return true;
}
 

void setup() {
  Serial.begin(115200);
 Serial.print("test");
  Serial.println("dziala?");

  pinMode(P1, INPUT_PULLUP);
  pinMode(P2, INPUT_PULLUP);
  pinMode(P3, INPUT_PULLUP);
  pinMode(P4, INPUT_PULLUP);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
}

void loop() {

  
  if (digitalRead(P1) == LOW) { wcisniete = 1; delay(500); }
  else if (digitalRead(P2) == LOW) { wcisniete = 2; delay(500); }
  else if (digitalRead(P3) == LOW) { wcisniete = 3; delay(500); }
  else if (digitalRead(P4) == LOW) { wcisniete = 4; delay(500); }

  
  if (wcisniete != -1) {

    Serial.print("Wpisano przycisk: ");
    Serial.println(wcisniete);

    wpisana_kombinacja[wcisniete_policz] = wcisniete;
    wcisniete_policz++;

    wcisniete = -1;
  }

  if (wcisniete_policz == 6) {

    if (sprawdzKombinacje()) {
      Serial.println("Wpisano poprawna kombinacje.");
      digitalWrite(RELAY, HIGH);
      delay(5000);
      digitalWrite(RELAY, LOW);
    } else {
      Serial.println("Wpisano zla kombinacje.");
    }

    resetKombinacji();
  }
}
