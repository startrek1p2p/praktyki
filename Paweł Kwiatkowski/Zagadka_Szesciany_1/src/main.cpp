#include <Arduino.h>

// Definicje pinow
#define KOSTKA_1 18
#define KOSTKA_2 13
#define KOSTKA_3 15
#define LED_BUILTIN 2

const int cube_count = 3;
const int pin_count = 4;

// Zostawiona wspólna inicjalizacja kostek + LED
int pins[pin_count] = {KOSTKA_1, KOSTKA_2, KOSTKA_3, LED_BUILTIN};

// Ustawienia kodu
const unsigned long check_interval = 300;

// Zmienne globalne
unsigned long last_check_time = 0;

void setup()
{
  Serial.begin(115200);

  // Inicjalizacja wejść dla kostek
  for (int i = 0; i < cube_count; i++)
  {
    pinMode(pins[i], INPUT_PULLUP);
  }

  // LED jako wyjście
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{

  bool cube_inserted[cube_count] = {false};

  // Odczyt stanu kostek
  for (int i = 0; i < cube_count; i++)
  {
    if (digitalRead(pins[i]) == LOW)
    {
      cube_inserted[i] = true;
    }
  }

  // Wykonywanie co określony czas
  if (millis() - last_check_time >= check_interval)
  {

    last_check_time = millis();

    bool all_inserted = true;

    for (int i = 0; i < cube_count; i++)
    {

      if (cube_inserted[i])
      {
        Serial.println("-Cube " + String(i + 1) + " INSERTED");
      }
      else
      {
        Serial.println("-Cube " + String(i + 1) + " NOT INSERTED");
        all_inserted = false;
      }
    }

    if (all_inserted)
    {
      Serial.println("---All cubes inserted!\n");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      Serial.println("---Not all cubes inserted\n");
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}