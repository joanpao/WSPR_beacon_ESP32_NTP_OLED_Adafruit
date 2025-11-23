/*
Programa original de WSPR Beacon by EA5JTT Juanpa 
- Adaptacion a Lilygo ESP32 T3 v1.6.1 OLED, compilar en Arduino IDE TLilygo T-Display
- Adafruit SI5351 library

El Si5351 obtiene
- unos 7 dBm = 5 mW 
- Se ha probado con exito 
    - para WSPR en las bandas de radioaficionado de HF de 80, 60, 40, 30, 20, 18, 15, 12 y 10 m 
    - usando un filtro pasabajos de 30 MHz
    - con un LNA como amplificador de potencia con una tenuador previo para evitar distorision por saturacion
*/

// messages.ino
typedef struct {
  bool sendMsg1 = false;
  // Maidenhead (locator)
  char MH_1[5] = {'A','A','0','0','\0'};
  // Power in dBm
  uint8_t dbm_1 = 23;
  // Time fields
  int Hours = 0;
  int Minutes = 0;
  int Seconds = 0;
  // GPS / NTP time validity flag
  bool GPS_valid = false;
} TGPS;

// ======== Variable global ========
TGPS UGPS;

#include <Adafruit_SI5351.h>
#include <JTEncode.h>
#include <Wire.h>
#include "settings.h"

// OLED display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Fixed8x16.h"

// WiFi y NTP
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Timer
#include "esp_timer.h"

// ---------- WSPR defines ----------
#define WSPR_TONE_SPACING 146    
#define SYMBOL_PERIOD_MS 683     
#define SYMBOL_PERIOD_US 683000UL
#define WSPR_SYMBOL_COUNT 162

// ---------- OLED pins ----------
#define OLED_RST NOT_A_PIN
#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SSD1306 display(OLED_RST);

// ---------- Si5351 ----------
Adafruit_SI5351 clockgen = Adafruit_SI5351();

// ---------- WSPR encoder ----------
JTEncode jtencode;
uint8_t tx_buffer[WSPR_SYMBOL_COUNT];
uint8_t symbol_count = WSPR_SYMBOL_COUNT;

// ---------- WiFi / NTP ----------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// ---------- Globals ----------
volatile bool proceed = false;
unsigned long long freq = 0ULL;   // frecuencia en Hz
// AQUI DEBE PONER SU QTH LOCATOR
char MaidenHeadLocal[5] = { 'I','M','9','9', 0 };
uint8_t dbm_local = DBMPOWER;
esp_timer_handle_t periodic_timer;

uint8_t freqIndex = 0;  

// ---------- Timer callback ----------
void timer_callback(void* arg) {
  proceed = true;
}

// ---------- OLED helper ----------
void showOLED(const char* line1, const char* line2) {
  display.clearDisplay();
  display.setCursor(0, 12);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}

// ---------- Set frequency CLK2 con setupMultisynth ----------
// =====================
// SET FREQUENCY + OFFSET (solo cambia b)
// =====================
void wspr_set_tone(uint64_t base_freq, uint8_t tone)
{
  // MODIFICACION 1 DE LA  FRECUENCIA PLL

    const double pll_freq = 25e6 * (24.0 + 0);
    double f = base_freq + (tone * 1.4648);   // añade offset WSPR
    double div = pll_freq / f;

    uint32_t a = (uint32_t)div;
    double frac = div - a;

    const uint32_t c = 1000000;         // denom fijo
    uint32_t b = (uint32_t)(frac * c);

    if (b == c) { a++; b = 0; }

    // NO apagar outputs
    clockgen.setupMultisynth(2, SI5351_PLL_B, a, b, c);
}


// ---------- Build tx buffer ----------
void set_tx_buffer(const char* callsign, const char* loc, uint8_t dbm) {
  memset(tx_buffer, 0, sizeof(tx_buffer));
  jtencode.wspr_encode(callsign, loc, dbm, tx_buffer);
}

// ---------- Encode & transmit ----------
void encode_and_tx() {
  set_tx_buffer(MYCALL, MaidenHeadLocal, dbm_local);
  clockgen.enableOutputs(true);

  for (uint16_t i = 0; i < symbol_count; i++) {
    double tone_offset = tx_buffer[i] * 1.4648;
    unsigned long long tx_freq_hz = freq + (unsigned long long)(tone_offset + 0.5);
    if (tx_freq_hz == 0ULL) tx_freq_hz = freq;

    // SOLO EN PRUEBAS
    //Serial.print("[TX] Symbol ");
    //Serial.print(i);
    //Serial.print(" tone=");
    //Serial.print(tx_buffer[i]);
    //Serial.print(" freq=");
    //Serial.println(tx_freq_hz);

   wspr_set_tone(freq, tx_buffer[i]);

    display.setCursor((i % 21) * 6, 51);
    display.print(".");
    display.display();

    proceed = false;
    while (!proceed) { delay(1); }
  }

  clockgen.enableOutputs(false);
  Serial.println("[TX] Transmission finished.");
}

// ---------- Check NTP ----------
void CheckNTP() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int m = timeinfo.tm_min;
    int s = timeinfo.tm_sec;

    // slot WSPR cada 2 minutos
    if ((s == 2) && ((m % 2) == 0)) {
        uint8_t numFreqs = sizeof(freqArray) / sizeof(freqArray[0]);
        freq = freqArray[freqIndex];           // selecciona frecuencia actual
        freqIndex = (freqIndex + 1) % numFreqs; // siguiente frecuencia

        Serial.print("[SCHEDULE] TX at minute ");
        Serial.print(m);
        Serial.print(" → freq=");
        Serial.println(freq);

        display.clearDisplay();
        display.setCursor(0,12);
        display.println("WSPR TX - EA5JTT");
        display.setCursor(0,30);
        display.print("Freq: ");
        display.println(freq);
        display.display();

        encode_and_tx();
    }
  } else {
    Serial.println("[NTP] getLocalTime failed.");
  }
}

// ---------- setup() ----------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[WSPR] Inicializando...");

  // WiFi
  //WiFi.begin(ssid, password);
  WiFi.begin(ssid, password);
  Serial.print("[WiFi] Conectando");
  unsigned long wstart = millis();
  while (WiFi.status() != WL_CONNECTED && millis()-wstart<20000) {
    delay(250); Serial.print(".");
  }
  Serial.println(WiFi.status()==WL_CONNECTED ? "\n[WiFi] Conectado" : "\n[WiFi] No conectado");

  // NTP
  configTime(0,0,"pool.ntp.org");
  timeClient.begin();
  timeClient.setTimeOffset(0);

  // OLED init
  display.begin(SSD1306_SWITCHCAPVCC,0x3C,true,OLED_SDA,OLED_SCL);
  display.clearDisplay();
  display.setFont(&Fixed8x16);
  display.setTextColor(WHITE);
  display.setCursor(0,12);
  display.println("WSPR TX - EA5JTT");
  display.setCursor(0,27);
  display.println("Inicializando...");
  display.display();

  // Si5351 init
  if(clockgen.begin()!=ERROR_NONE) {
    Serial.println("[ERROR] No Si5351 detectado.");
    showOLED("Si5351","NO DETECTADO");
    while(1) delay(1000);
  }
  Serial.println("[Si5351] OK");

  // PLLB OJO HAY QUE CAMBIARLO EN DOS LUGARES, AQUI Y EN EL CALCULO
  // N = 24 - 36  F = 600 - 900 
  //| PLL (MHz)   | A  | B | C                         |
  //| ----------- | -- | - | ------------------------- |
  //| **600 MHz** | 24 | 0 | 1 → 25 MHz × 24 = 600 MHz |
  //| **800 MHz** | 32 | 0 | 1 → 25 × 32 = 800 MHz     |
  //| **900 MHz** | 36 | 0 | 1 → 25 × 36 = 900 MHz     |

  // MODIFICACION 2 DE LA  FRECUENCIA PLL
  clockgen.setupPLLInt(SI5351_PLL_B, 24);
  //clockgen.setupPLLInt(SI5351_PLL_B, 36);
  //clockgen.setupPLL(SI5351_PLL_B,24,2,3);
  clockgen.enableOutputs(false);

  // esp_timer
  esp_timer_create_args_t periodic_timer_args = {
    .callback=&timer_callback,
    .arg=NULL,
    .name="wspr_periodic"
  };
  esp_timer_create(&periodic_timer_args,&periodic_timer);
  esp_timer_start_periodic(periodic_timer,SYMBOL_PERIOD_US);

  #ifdef MaidenHead
  strncpy(MaidenHeadLocal,MaidenHead,4);
  #endif
  display.clearDisplay();
  display.setCursor(0,12);
  display.println("WSPR TX - EA5JTT");
  display.setCursor(0,30);
  display.println("Listo, esperando");
  display.display();
  Serial.println("[Setup] Listo.");
}

// ---------- loop() ----------
void loop() {
  timeClient.update();
  CheckNTP();
  delay(200);
}
