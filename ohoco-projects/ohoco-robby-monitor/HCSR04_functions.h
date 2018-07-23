long HCSR04_read_distance() {
  long duration, distance;

  digitalWrite(HCSR04_TRIG_PIN, LOW);  
  delayMicroseconds(2); 

  digitalWrite(HCSR04_TRIG_PIN, HIGH);
  delayMicroseconds(10); 

  digitalWrite(HCSR04_TRIG_PIN, LOW);
  duration = pulseIn(HCSR04_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  
  return distance;
}


