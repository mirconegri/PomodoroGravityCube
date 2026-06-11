#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;

const int pinRosso = 5;
const int pinVerde = 6;
const int pinBlu   = 9;

int  facciaAttuale      = -1;
int  facciaTimerSalvata =  0;
long tempoRimanente     =  0;
unsigned long ultimoTempo = 0;
bool inPausa = false;

void impostaColore(bool r, bool g, bool b) {
  digitalWrite(pinRosso, r ? HIGH : LOW);
  digitalWrite(pinVerde, g ? HIGH : LOW);
  digitalWrite(pinBlu,   b ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("--- AVVIO ---"));
  
  pinMode(pinRosso, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinBlu,   OUTPUT);
  impostaColore(0, 0, 0);

  Serial.println(F("Accendo OLED..."));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERRORE OLED!"));
    impostaColore(1, 0, 0); 
    while (1); 
  }
  
  display.clearDisplay();
  display.display();

  Serial.println(F("Accendo MPU..."));
  if (!mpu.begin()) {
    Serial.println(F("ERRORE MPU!"));
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.println(F("No MPU!"));
    display.display();
    impostaColore(1, 0, 0); 
    while (1);
  }

  Serial.println(F("Tutto OK!"));
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(15, 20);
  display.print(F("CUBO OK!"));
  display.display();
  delay(1500);

  ultimoTempo = millis();
}

void loop() {
  unsigned long tempoCorrente = millis();
  unsigned long delta = tempoCorrente - ultimoTempo;
  ultimoTempo = tempoCorrente;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  int nuovaFaccia = -1;
  int rotazione   =  0;

  if      (a.acceleration.z >  7.0) { nuovaFaccia = 0; rotazione = 0; }
  else if (a.acceleration.x >  7.0) { nuovaFaccia = 1; rotazione = 1; }
  else if (a.acceleration.x < -7.0) { nuovaFaccia = 2; rotazione = 3; }
  else if (a.acceleration.y >  7.0) { nuovaFaccia = 3; rotazione = 2; }
  else if (a.acceleration.y < -7.0) { nuovaFaccia = 4; rotazione = 0; }

  if (nuovaFaccia != -1 && nuovaFaccia != facciaAttuale) {
    facciaAttuale = nuovaFaccia;
    if (facciaAttuale == 0) {
      inPausa = true;
    } else {
      if (facciaAttuale == facciaTimerSalvata && inPausa) {
        inPausa = false;
      } else {
        facciaTimerSalvata = facciaAttuale;
        inPausa = false;
        switch (facciaAttuale) {
          case 1: tempoRimanente =  5L * 60L * 1000L; break;
          case 2: tempoRimanente = 15L * 60L * 1000L; break;
          case 3: tempoRimanente = 25L * 60L * 1000L; break;
          case 4: tempoRimanente = 45L * 60L * 1000L; break;
        }
      }
    }
  }

  if (!inPausa && tempoRimanente > 0 && facciaAttuale != 0) {
    tempoRimanente -= (long)delta;
    if (tempoRimanente < 0) tempoRimanente = 0;
  }

  display.setRotation(rotazione);
  display.clearDisplay();
  display.setTextColor(WHITE);
  bool blink = (millis() / 500) % 2 == 0;

  if (inPausa) {
    impostaColore(0, 0, 1);
    display.setTextSize(2);
    display.setCursor(20, 24);
    display.print(F("PAUSA"));

  } else if (tempoRimanente == 0 && facciaTimerSalvata != 0) {
    impostaColore(blink ? 1 : 0, 0, 0);
    
    // Centratura "FINITO!" in base alla rotazione
    if (rotazione == 1 || rotazione == 3) {
      display.setTextSize(1); // Deve essere più piccolo in verticale
      display.setCursor(11, 60);
    } else {
      display.setTextSize(2);
      display.setCursor(22, 24);
    }
    display.print(F("FINITO!"));

  } else if (tempoRimanente > 0) {
    int minuti  = (tempoRimanente / 1000) / 60;
    int secondi = (tempoRimanente / 1000) % 60;
    
    if (tempoRimanente <= 30000)
      impostaColore(blink ? 1 : 0, blink ? 1 : 0, 0);
    else
      impostaColore(0, 1, 0);
      
    // Centratura perfetta per lo schermo rotato
    if (rotazione == 1 || rotazione == 3) {
      display.setTextSize(2); 
      // X = 2 (sfrutta i 64px massimi), Y = 56 (centrato a metà dei 128px di altezza)
      display.setCursor(2, 56); 
    } else {
      display.setTextSize(3); 
      // X = 19 (centrato nei 128px), Y = 20 (centrato nei 64px)
      display.setCursor(19, 20); 
    }
    
    if (minuti  < 10) display.print(F("0"));
    display.print(minuti);
    display.print(F(":"));
    if (secondi < 10) display.print(F("0"));
    display.print(secondi);

  } else {
    impostaColore(0, 0, 0);
    display.setTextSize(1);
    display.setCursor(16, 28);
    display.print(F("GIRA IL CUBO"));
  }

  display.display();
  delay(100); 
}
