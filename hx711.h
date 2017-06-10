#pragma once

class Hx711
{
public:
  Hx711(uint8_t dt, uint8_t sck)
  : mDt(dt), mSck(sck) {}

  void init(void) {
    pinMode(mSck, OUTPUT);
    pinMode(mDt, INPUT);
    digitalWrite(mSck, HIGH);
    digitalWrite(mSck, LOW);
  }  

  unsigned long getValue()
  {
    for (uint8_t i = 100; i--;) {
      if(!digitalRead(mDt)){
        uint32_t data = shiftInMsb(mDt, mSck, 24);
        digitalWrite(mSck, HIGH);
        digitalWrite(mSck, LOW);
        return data ^ 0x800000;
      }
    }
    return 0;
  }

  static uint32_t shiftInMsb(uint8_t dataPin, uint8_t clockPin, uint8_t count){
    uint32_t value = 0;
    for(uint8_t i = count; i--;) {
      digitalWrite(clockPin, HIGH);
      value |= (uint32_t) digitalRead(dataPin) << i;
      digitalWrite(clockPin, LOW);
    }
    return value;
  }

private:
  uint8_t mDt;
  uint8_t mSck;
};
