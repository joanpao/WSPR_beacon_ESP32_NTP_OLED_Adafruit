# WSPR TX Beacon – ESP32 + SI5351 (NTP Version)
Low-cost multiband WSPR transmitter using an ESP32 with OLED (Lilygo ESP32 T3 V1.6.1) and an SI5351 clock generator  
(coverage: **8 kHz – 160 MHz**, output power: **≈7 dBm / 5 mW**).

This version does **not** require GPS.  
Instead, it uses **Wi-Fi + NTP** for timing accuracy.

It is based on:
- GPS version: https://github.com/joanpao/WSPR_beacon_ESP32_GPS  
- NTP version: https://github.com/joanpao/WSPR_beacon_ESP32_NTP_OLED  

The key difference is that this implementation uses **Adafruit’s SI5351 libraries** (https://github.com/adafruit/Adafruit_Si5351_Library) instead of Etherkit’s, 
allowing **lower-level control**, such as PLL frequency configuration in the **600–900 MHz** range.

---

## ✨ Features

- Multiband WSPR transmitter (80 m → 10 m confirmed).
- No GPS required — full synchronization through Wi-Fi + NTP.
- Optional OLED display for real-time status.
- Serial console monitoring.

---

## 🛠 Hardware Requirements

- ESP32 development board  (5 mW output = 7 dBm  suitable for experimental WSPR beacons  )
- SI5351 clock generator module
- Antenna for the desired amateur band (e.g. End-Fed 6m & UnUN 1:49) 
- Optional:
  OLED 128×64 display (SSD1306)  
  LPF 30MHz (e.g https://es.aliexpress.com/item/1005006331048805.html) It's highly recommended
  Attenuator 10 dB (e.g. https://es.aliexpress.com/item/1005006398965437.html) + LNA 20 dB (e.g. https://es.aliexpress.com/item/1005008935439596.html) = 17 dBm  50 mW


---

## 📡 Recommended Additional Equipment

- An RF receiver or SDR to verify frequency and transmission.  
- A WSPR receiver or internet WSPR reporting system to observe SPOTS.  

---

## 📁 Optional OLED Library Files

These files are **not required** for WSPR operation, only if you want to use the OLED screen:

- `Adafruit_SSD1306.cpp`
- `Adafruit_SSD1306.h`
- `Fixed8x16.h`

---

## ⚙️ Operation

- The ESP32 acquires network time via **NTP**.
- WSPR frames are generated in sequence (no following the international WSPR standard).
- The SI5351 clock generator is configured using Adafruit's library with precision PLL settings.
- The OLED (if installed) displays:
  - Current band
  - TX frequency
  - Time sync status
  - TX slot countdown  

Serial output provides identical operational information.

---

## 🔧 Software Configuration

1. Configure your Wi-Fi credentials in `config.h`.
2. Select your desired WSPR band and frequency and ajust index table
3. Upload the firmware to the ESP32.
4. Monitor via:
   - OLED screen,
   - Serial port, or
   - External SDR + WSPR spotting network.

---

## 📜 Legal Notice

**Operating a WSPR beacon requires an amateur radio license.**  
Operation without a license is illegal in most countries.

If you don't have a license, obtaining one is recommended — in many places it is easier to get than a driver’s license.

---

## 🌐 More Information (Spanish)

A detailed Spanish article is available here:  
https://ea5jtt.blogspot.com/2025/09/wspr-tx-beacon-esp32-si5351-version-ntp.html
https://ea5jtt.blogspot.com/2025/09/si5351-primeros-pasos.html

---

## 📄 License
MIT License (or your preferred license)

---

## 🙌 Contributions
Pull requests and improvements are welcome!
