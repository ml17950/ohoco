unsigned char carrierFreq = 0;
unsigned char period = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodHigh = 0; //calculated once for each signal sent in initSoftPWM
unsigned char periodLow = 0; //calculated once for each signal sent in initSoftPWM

void init_software_pwm(unsigned char carrierFreq) {
  period = floor(1000 / carrierFreq);
  periodHigh = period / 2;
  periodLow = period - periodHigh;

  switch (carrierFreq) {
    case 38 : // delivers a carrier frequency of around 38kHz & duty cycle of around 50%
    default :
      periodHigh -= 1; // Trim it based on measurementt from Oscilloscope
      periodLow  -= 2; // Trim it based on measurementt from Oscilloscope
      break;
  }

  ohoco.print("period total (µs): ");
  ohoco.println(period);
  ohoco.print("period high  (µs): ");
  ohoco.println(periodHigh);
  ohoco.print("period low   (µs): ");
  ohoco.println(periodLow);
  
  ohoco.print("pwm initialized @ ");
  ohoco.print(carrierFreq);
  ohoco.println(" kHz");
  ohoco.println("");
}

void space_micros(unsigned int mLen) { //uses sigTime as end parameter
  ohoco.println("IR   >> space_micros for " + String(mLen) + " µs");
  unsigned long now = micros();
  unsigned long dur = now; //allows for rolling time adjustment due to code execution delays
  if (mLen == 0) return;
  while ((micros() - now) < mLen) ; //just wait here until time is up
}

void pulse_micros(unsigned long mLen) {
  if (mLen == 0)
    return;
  ohoco.println("IR   >> pulse_micros for " + String(mLen) + " µs");
  unsigned long now = micros();
  while ((micros() - now) < mLen) {
//    digitalWrite(IR_SEND_PIN, HIGH);
//    delayMicroseconds(periodHigh); //HIGHTIME-6);
//    digitalWrite(IR_SEND_PIN, LOW);
//    delayMicroseconds(periodLow); //LOWTIME-7);
    digitalWrite(IR_SEND_PIN, LOW);
    delayMicroseconds(periodHigh); //HIGHTIME-6);
    digitalWrite(IR_SEND_PIN, HIGH);
    delayMicroseconds(periodLow); //LOWTIME-7);
  }
}
