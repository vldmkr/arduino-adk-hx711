#include <adk.h>
#include "hx711.h"

#define HX711_DATA      2
#define HX711_CLK       3

#define LED_PIN         4

#define SEND_PERIOD_MS  500
#define RESET_DELAY_MS  500

/**
 * ProMini: Reboot loop when using watchdog
 * https://github.com/arduino/Arduino/issues/4492
 * 
 * asm volatile ("jmp 0") restarts program from beginning but does not reset the peripherals and registers
 */
void softReset() {
  digitalWrite(LED_PIN, LOW);
  uint32_t resetTime = millis() + RESET_DELAY_MS;
  while (resetTime > millis()); //wait until the required delay expires
  asm volatile ("jmp 0");  
}

Hx711 hx711(HX711_DATA, HX711_CLK);
USB usb;
ADK adk(&usb, "Arduino",                                // Manufacturer Name
              "HX711",                                  // Model Name
              "Example sketch for the USB Host Shield", // Description (user-visible string)
              "1.0",                                    // Version
              "https://github.com/vldmkr",              // URL (web page to visit if no installed apps support the accessory)
              "123456789");                             // Serial Number (optional)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  hx711.init();
  Serial.begin(115200);
  
  if (usb.Init() == -1) {
    Serial.println(F("Failed to start. Restarting."));
    softReset();
  }
  Serial.println(F("Started."));
}

void loop() {
  static uint32_t oldMillis = 0;
  static bool isConnected = false;
  usb.Task();

  if (adk.isReady()) {
    if ( ! isConnected) {
      isConnected = true;
      Serial.println(F("Connected."));
    }

    uint8_t received = 0;
    uint16_t len = sizeof(received);
    uint8_t rcode = adk.RcvData(&len, &received);
    bool isError = rcode && rcode != hrNAK;
    if ( ! isError && len > 0) { 
      Serial.print(F("LED level: "));
      Serial.println(received);
      digitalWrite(LED_PIN, received);
    }

    if ((millis() - oldMillis) >= SEND_PERIOD_MS) {
      oldMillis = millis();

      uint32_t value = hx711.getValue();
      adk.SndData(sizeof(value), (uint8_t*) &value);
    }
  } else if (isConnected) {
    isConnected = false;
    Serial.println(F("Disconnected. Restarting."));
    softReset();
  }
}
