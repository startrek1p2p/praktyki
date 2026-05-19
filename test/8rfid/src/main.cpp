#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

// --- DFPlayer ---
DFRobotDFPlayerMini myDFPlayer;
HardwareSerial mySerial(2);

// --- RFID / PN532 ---
const byte numReaders = 8;
const byte ssPins[] = {26, 21, 22, 15, 27, 25, 32, 33}; // Piny SS dla czytników PN532 26 21 22 15 27 25 32 33
/*
miso 19
mosi 23
sck 18

*/
PN532_SPI *pn532spi[numReaders];
PN532 *nfc[numReaders];

// // --- Dodatkowe GPIO ---
// #define dymarka 4 // wyjście (np. przekaźnik dmuchawy)
// #define drzwi 25  // wejście „drzwi”, AKTYWNE NISKO

// --- czasy antyzakłóceniowe / detekcji drzwi ---
const uint16_t DOOR_LOW_MS = 1000;  // ile ms stan niski musi trwać, by zadziałać
const uint16_t DOOR_HIGH_MS = 2000; // ile ms stan wysoki musi trwać, by zresetować

// --- UID-y (Twoje) ---
/*
Przykładowe logi:
Reader #0 on Pin #32 detected tag: b3367f8
Reader #1 on Pin #33 detected tag: b2dcd1
Reader #2 on Pin #22 detected tag: 377fdec6
Reader #3 on Pin #26 detected tag: 1ea4c81
Reader #4 on Pin #27 detected tag: d951f983
*/
const String correctIDs[] = {
    "2341bb14", "e3baab2c", "936da914", "1320b314", "d951f983"};
const String correctIDs_emergency[] = {
    "b9505f98", "9c77432", "b6bce42b", "c6cba02b", "655a2b3"};

String currentIDs[numReaders] = {"", "", "", ""};

// ======= Funkcje pomocnicze =======

String dump_byte_array(uint8_t *buffer, byte bufferSize)
{
  String read_rfid = "";
  for (byte i = 0; i < bufferSize; i++)
    read_rfid += String(buffer[i], HEX);
  return read_rfid;
}

// Zwraca indeks pozycji, dla której tag jest poprawny (spośród 0..numReaders-1) lub -1.
int findTagPosition(const String &tag)
{
  for (int i = 0; i < numReaders; i++)
  {
    if (tag == correctIDs[i] || tag == correctIDs_emergency[i])
    {
      // UWAGA: nie gramy tutaj! (żeby nie zapętlać)
      return i;
    }
  }
  return -1;
}

// Czy tag pasuje konkretnie do pozycji 'pos'
bool isCorrectAtPosition(uint8_t pos, const String &tag)
{
  if (pos >= numReaders)
    return false;
  return (tag == correctIDs[pos] || tag == correctIDs_emergency[pos]);
}

// ======= SETUP =======
void setup()
{
  Serial.begin(115200);
  Serial.println(F("Serial komunikacja startuje"));

  // pinMode(dymarka, OUTPUT);
  // digitalWrite(dymarka, LOW);

  // // DRZWI: podciągnięcie do Vcc, aktywne NISKO
  // pinMode(drzwi, INPUT_PULLUP);
  delay(500);
  SPI.begin();
  for (uint8_t i = 0; i < numReaders; i++)
  {
    pn532spi[i] = new PN532_SPI(SPI, ssPins[i]);
    nfc[i] = new PN532(*pn532spi[i]);
    nfc[i]->begin();

    uint32_t versiondata = nfc[i]->getFirmwareVersion();
    if (!versiondata)
    {
      Serial.print("Didn't find PN53x board on reader ");
      Serial.println(i + 1);
      // brak halt – kontynuujemy
    }
    else
    {
      Serial.print("Found chip PN5 on reader ");
      Serial.print(i + 1);
      Serial.print(" Firmware ver. ");
      Serial.print((versiondata >> 16) & 0xFF, DEC);
      Serial.print('.');
      Serial.println((versiondata >> 8) & 0xFF, DEC);
    }

    nfc[i]->SAMConfig();
    delay(100);
    Serial.println("------------");
  }

  // DFPlayer na Serial2 (GPIO16 RX, GPIO17 TX)
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  if (!myDFPlayer.begin(mySerial))
  {
    Serial.println("Błąd inicjalizacji DFPlayer!");
  }
  myDFPlayer.volume(25);
  Serial.println("DFPlayer Mini gotowy!");
  Serial.println("===== Koniec setupu =====");

  delay(1000);
}

// ======= LOOP =======
void loop()
{

  uint8_t correctCount = 0;
  bool puzzleSolved = true;
  bool changedValue = false;

  static bool canPlaySolvedMusic = true;
  static uint32_t belowThresholdTimestamp = 0;
  static bool wasSolved = false;

  // PAMIĘĆ poprzednich UID-ów – dla logiki „zagraj raz na zmianę”
  static String lastIDs[numReaders] = {"", "", "", ""};

  // 2.1 Odczyt wszystkich czytników
  for (uint8_t i = 0; i < numReaders; i++)
  {
    String readRFID = "";
    uint8_t uid[7] = {0};
    uint8_t uidLength = 0;

    if (nfc[i]->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 500))
    {
      readRFID = dump_byte_array(uid, uidLength);
      currentIDs[i] = readRFID;
      changedValue = true;
    }
    else
    {
      currentIDs[i] = ""; // brak karty
    }
  }

  // 2.3 Walidacja pozycji tagów (bez grania)
  for (uint8_t i = 0; i < numReaders; i++)
  {
    String readRFID = currentIDs[i];
    int correctPosition = findTagPosition(readRFID);

    if (correctPosition == -1)
    {
      puzzleSolved = false;
    }
    else if (correctPosition == i)
    {
      correctCount++;
    }
    else
    {
      puzzleSolved = false;
    }
    delay(5);
  }

  // 2.4 Logika „nagrody” / ognia
  if (correctCount == 4 && canPlaySolvedMusic)
  {
    Serial.println("Rozwiązane");
    // myDFPlayer.play(2); // jednorazowy utwór za rozwiązanie (opcjonalnie)
    // digitalWrite(dymarka, LOW);

    wasSolved = true;
  }

  // Odblokowanie „nagrody” po spadku poniżej 4 tagów przez 20 s
  if (correctCount < 4)
  {
    if (wasSolved)
    {
      belowThresholdTimestamp = millis();
      wasSolved = false;
    }
    if (belowThresholdTimestamp > 0 && millis() - belowThresholdTimestamp > 20000)
    {
      canPlaySolvedMusic = true;
      belowThresholdTimestamp = 0;
    }
  }
  else if (correctCount >= 4 && correctCount < 5)
  {
    belowThresholdTimestamp = 0; // reset gdy wróci >=4 ale jeszcze nie 5
  }

  // Debug RFID
  if (changedValue)
  {
    for (uint8_t i = 0; i < numReaders; i++)
    {
      Serial.print(F("Reader #"));
      Serial.print(String(i));
      Serial.print(F(" on Pin #"));
      Serial.print(String(ssPins[i]));
      Serial.print(F(" detected tag: "));
      Serial.println(currentIDs[i]);
    }
    Serial.println(F("---"));
  }
}
