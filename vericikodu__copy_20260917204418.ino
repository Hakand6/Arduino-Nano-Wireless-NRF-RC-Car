#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE: D9, CSN: D10
const byte address[6] = "RC775";

// Analog Pin Tanımlamaları
const int POT_ISIK   = A0; // Pot 2 (Alt LED Parlaklığı)
const int POT_HIZ    = A1; // Pot 1 (Maksimum Hız Limiti)
const int JOY_YON    = A2; // Joystick 2 Direksiyon (Ters Invert)
const int JOY_GAZ    = A3; // Joystick 1 Gaz

const int LED_POWER  = 2;  // Güç Ledi (D2)
const int LED_CONN   = 3;  // Bağlantı Ledi (D3)

// Gönderilecek Veri Paketi
struct Data_Package {
  int gazVal = 512;    // 0 - 1023 (Ham Analog Değer)
  int yonVal = 512;    // 0 - 1023 (Ham Analog Değer)
  byte maxHiz = 255;   // 0 - 255 (Pot 1)
  byte isikHiz = 0;    // 0 - 255 (Pot 2)
};

Data_Package data;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_CONN, OUTPUT);
  digitalWrite(LED_POWER, HIGH); // Kumanda çalıştı, Güç LED'i yansın

  Serial.println("\n================================================");
  Serial.println("   GELISMIS RC KUMANDA SİSTEMİ BAŞLATILDI     ");
  Serial.println("================================================\n");

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX); // Yüksek menzil gücü
  radio.stopListening();
}

void loop() {
  // 1. ANALOG SENSÖR OKUMALARI
  data.gazVal  = analogRead(JOY_GAZ);
  
  // Joystick ters monte edildiği için yazılımsal olarak yönü düzeltiyoruz
  data.yonVal  = 1023 - analogRead(JOY_YON); 
  
  data.maxHiz  = map(analogRead(POT_HIZ), 0, 1023, 0, 255);
  data.isikHiz = map(analogRead(POT_ISIK), 0, 1023, 0, 255);

  // 2. VERİ PAKETİNİ ALICIYA GÖNDERME
  bool baglantiVar = radio.write(&data, sizeof(Data_Package));

  // 3. BAĞLANTI LEDİ KONTROLÜ
  if (baglantiVar) {
    digitalWrite(LED_CONN, HIGH); // Alıcıdan onay geldikçe D3 Ledi YANAR
  } else {
    digitalWrite(LED_CONN, LOW);  // Alıcı kapalıysa/sinyal yoksa D3 SÖNER
  }

  // 4. CANLI TELEMETRİ VE NRF DURUM ÇIKTISI (SERİ PORT)
  Serial.print("POT1(Hız Limit): "); Serial.print(data.maxHiz);
  Serial.print(" | POT2(Işık): "); Serial.print(data.isikHiz);

  Serial.print(" | GAZ(A3): ");
  if(data.gazVal > 540)      Serial.print("[İLERİ KADEMELİ]");
  else if(data.gazVal < 480) Serial.print("[GERİ KADEMELİ]");
  else                        Serial.print("[DUR]");

  Serial.print(" | YÖN(A2): ");
  if(data.yonVal > 540)      Serial.print("[SAĞ KADEMELİ]");
  else if(data.yonVal < 480) Serial.print("[SOL KADEMELİ]");
  else                        Serial.print("[DÜZ]");

  if (baglantiVar) {
    Serial.println(" | NRF: [ÇALIŞIYOR & ALICI ONAYLADI]");
  } else {
    Serial.println(" | NRF: [ÇİP AKTİF / ALICI YANIT VERMİYOR]");
  }

  delay(20); // 50 Hz yenileme hızı (Hızlı ve hassas tepki süresi)
}