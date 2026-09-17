#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10); // CE: D9, CSN: D10
const byte address[6] = "RC775";

// Pin Tanımlamaları
const int MOSFET_ALT_LED   = 2;  // IRLZ44N Alt Işık PWM (D2)
const int MOSFET_MOTOR_PWM = 3;  // HW-517 Motor Hız PWM (D3)
const int ROLE_IN1         = 4;  // Çift Kanal Röle 1 (D4)
const int ROLE_IN2         = 5;  // Çift Kanal Röle 2 (D5)
const int SERVO_PIN        = 6;  // Direksiyon Servosu (D6)
const int LED_SYSTEM       = 7;  // Alıcı Sistem/Bağlantı LED'i (D7)

Servo direksiyon;

// Vericiden Gelen Paket
struct Data_Package {
  int gazVal = 512;
  int yonVal = 512;
  byte maxHiz = 255;
  byte isikHiz = 0;
};

Data_Package data;
unsigned long sonSinyalZamani = 0;
static int sonYon = 0; // 0: Nötr, 1: İleri, 2: Geri

void setup() {
  pinMode(MOSFET_MOTOR_PWM, OUTPUT);
  pinMode(MOSFET_ALT_LED, OUTPUT);
  pinMode(LED_SYSTEM, OUTPUT);
  
  pinMode(ROLE_IN1, OUTPUT);
  pinMode(ROLE_IN2, OUTPUT);
  
  // Başlangıç durumları
  analogWrite(MOSFET_MOTOR_PWM, 0);
  digitalWrite(ROLE_IN1, HIGH);
  digitalWrite(ROLE_IN2, HIGH);
  digitalWrite(LED_SYSTEM, LOW);

  direksiyon.attach(SERVO_PIN);
  direksiyon.write(90); // Başlangıçta tekerler düz

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    sonSinyalZamani = millis();
    digitalWrite(LED_SYSTEM, HIGH); // Sinyal varken D7 LED'i SABİT YANAR
  }

  // FAIL-SAFE: 1 saniyeden fazla sinyal koparsa güvenliğe geç
  if (millis() - sonSinyalZamani > 1000) {
    analogWrite(MOSFET_MOTOR_PWM, 0);
    analogWrite(MOSFET_ALT_LED, 0);
    direksiyon.write(90);
    
    // Sinyal yokken D7 Ledi yanıp söner (Blink uyarısı)
    digitalWrite(LED_SYSTEM, (millis() / 300) % 2); 
    return;
  }

  // 1. ALT LED PARLAKLIK KONTROLÜ (IRLZ44N)
  analogWrite(MOSFET_ALT_LED, data.isikHiz);

  // 2. ORANSAL DİREKSİYON (SERVO) KONTROLÜ
  int servoAcisi = map(data.yonVal, 0, 1023, 60, 120);
  servoAcisi = constrain(servoAcisi, 60, 120);
  direksiyon.write(servoAcisi);

  // 3. KADEMELİ GAZ VE RÖLE KONTROLÜ
  int gaz = data.gazVal;
  
  // İLERİ SÜRÜŞ
  if (gaz > 540) {
    if (sonYon == 2) { 
      analogWrite(MOSFET_MOTOR_PWM, 0); // Ani yön değişim koruması
      delay(150);
    }
    digitalWrite(ROLE_IN1, LOW);
    digitalWrite(ROLE_IN2, HIGH);

    int hedefHiz = map(gaz, 541, 1023, 0, data.maxHiz);
    analogWrite(MOSFET_MOTOR_PWM, hedefHiz);
    sonYon = 1;
  } 
  // GERİ SÜRÜŞ
  else if (gaz < 480) {
    if (sonYon == 1) { 
      analogWrite(MOSFET_MOTOR_PWM, 0); // Ani yön değişim koruması
      delay(150);
    }
    digitalWrite(ROLE_IN1, HIGH);
    digitalWrite(ROLE_IN2, LOW);

    int hedefHiz = map(gaz, 479, 0, 0, data.maxHiz);
    analogWrite(MOSFET_MOTOR_PWM, hedefHiz);
    sonYon = 2;
  } 
  // DURMA / NÖTR BÖLGE
  else {
    analogWrite(MOSFET_MOTOR_PWM, 0);
    digitalWrite(ROLE_IN1, HIGH);
    digitalWrite(ROLE_IN2, HIGH);
    sonYon = 0;
  }
}