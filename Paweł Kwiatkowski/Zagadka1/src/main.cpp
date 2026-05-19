#include <Arduino.h>
#include <string.h>

// Definicje pinów
#define B1 25
#define B2 26
#define B3 32
#define B4 33

#define RLAY 5

// Definicje zmiennych
int8_t pressed = -1;
int8_t pressed_count = 0;
int i = -1;
int prawidlowa_kombinacja [6] = {1, 2, 1, 4, 3, 2};
int wpisana_kombinacja [6];

void setup() {
  Serial.begin(115200);

  // Konfiguracja wyprowadzen
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  pinMode(B4, INPUT_PULLUP);
  pinMode(RLAY, OUTPUT); digitalWrite(RLAY, LOW);
}

void loop() {
  if(digitalRead(B1) == LOW){
      pressed = 1;
      Serial.println("Wpisano przycisk: 1"); delay(200);
    } else if(
      digitalRead(B2) == LOW){
      pressed = 2;
      Serial.println("Wpisano przycisk: 2"); delay(200);
    } else if(
      digitalRead(B3) == LOW){
      pressed = 3;
      Serial.println("Wpisano przycisk: 3"); delay(200);
    } else if(
      digitalRead(B4) == LOW){
      pressed = 4;
      Serial.println("Wpisano przycisk: 4"); delay(200);
    }
  
  switch(pressed){
    case 1: wpisana_kombinacja[pressed_count] = 1; pressed_count++; pressed = -1; break;
    case 2: wpisana_kombinacja[pressed_count] = 2; pressed_count++; pressed = -1; break;
    case 3: wpisana_kombinacja[pressed_count] = 3; pressed_count++; pressed = -1; break;
    case 4: wpisana_kombinacja[pressed_count] = 4; pressed_count++; pressed = -1; break;
  }

  if((pressed_count == 6) && memcmp(prawidlowa_kombinacja, wpisana_kombinacja, sizeof(prawidlowa_kombinacja)) == 0){
    Serial.println("Wpisano prawidlowa kombinacje!");
    digitalWrite(RLAY, HIGH);

    // Reset kombinacji po poprawnym wpisaniu jest tylko do testu kodu
    delay(5000); digitalWrite(RLAY, LOW);
    int wpisana_kombinacja [6] = {0, 0, 0, 0, 0, 0};
    pressed_count = 0;
  } 
  else if(pressed_count == 6){
      Serial.println("Wpisano nieprawidlowa kombinacje - sprobuj ponownie.");
      int wpisana_kombinacja [6] = {0, 0, 0, 0, 0, 0};
      pressed_count = 0;
  }
}