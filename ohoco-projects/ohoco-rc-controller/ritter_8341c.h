// 5 = start-sequence
// 9 = end-sequence
int SOCKET_1_ON[35]  = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,1,2,2,2,2,2,9};
int SOCKET_2_ON[35]  = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,1,2,2,2,3,1,9};
int SOCKET_3_ON[35]  = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,1,2,2,3,1,2,9};
//                                                                             |ON | | ADR |
int SOCKET_1_OFF[35] = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,2,1,2,2,2,2,9};
int SOCKET_2_OFF[35] = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,2,1,2,2,3,1,9};
int SOCKET_3_OFF[35] = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,2,2,1,2,3,1,2,9};
//                                                                             |OFF| | ADR |
int SOCKET_A_ON[35]  = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,1,2,2,2,2,2,2,9};
int SOCKET_A_OFF[35] = {5,1,3,1,2,2,2,3,1,2,2,2,3,2,2,2,1,2,2,2,2,2,2,3,2,1,3,1,3,1,2,2,2,2,9};

void ritter_start_bit() {
//  Serial.println("ritter_start_bit"); return;
  
  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(2720);
}
void ritter_send_1() {
//  Serial.println("ritter_send_1"); return;
  
  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(1280);
}
void ritter_send_2() {
//  Serial.println("ritter_send_2"); return;
  
  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(245);

  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(1330);
}
void ritter_send_3() {
//  Serial.println("ritter_send_3"); return;
  
  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(245);

  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(245);

  digitalWrite(TRANSMITTER_PIN, HIGH);
  delayMicroseconds(270);
  digitalWrite(TRANSMITTER_PIN, LOW);
  delayMicroseconds(1280);
}
void ritter_end_gap() {
//  Serial.println("ritter_end_gap"); return;
  
  delayMicroseconds(10600);
}

void ritter_socket_on(int id) {
  int signalBuffer[35];
  
  switch (id) {
    case 1: memcpy(signalBuffer, SOCKET_1_ON, sizeof(SOCKET_1_ON)); break;
    case 2: memcpy(signalBuffer, SOCKET_2_ON, sizeof(SOCKET_2_ON)); break;
    case 3: memcpy(signalBuffer, SOCKET_3_ON, sizeof(SOCKET_3_ON)); break;
    default: return;
  }
  
  Serial.print("ritter socket ");
  Serial.print(id);
  Serial.println(" -> on");

  int sizeArray = sizeof(signalBuffer) / sizeof(int);

  for (int t = 0; t < 3; t++) {
    for (int i = 0; i < sizeArray; i++) {
      switch (signalBuffer[i]) {
          case 1: ritter_send_1(); break;
          case 2: ritter_send_2(); break;
          case 3: ritter_send_3(); break;
          case 5: ritter_start_bit(); break;
          case 9: ritter_end_gap(); break;
      }
    }
  }
}

void ritter_socket_off(int id) {
  int signalBuffer[35];
  
  switch (id) {
    case 1: memcpy(signalBuffer, SOCKET_1_OFF, sizeof(SOCKET_1_OFF)); break;
    case 2: memcpy(signalBuffer, SOCKET_2_OFF, sizeof(SOCKET_2_OFF)); break;
    case 3: memcpy(signalBuffer, SOCKET_3_OFF, sizeof(SOCKET_3_OFF)); break;
    default: return;
  }
  
  Serial.print("ritter socket ");
  Serial.print(id);
  Serial.println(" -> off");

  int sizeArray = sizeof(signalBuffer) / sizeof(int);

  for (int t = 0; t < 3; t++) {
    for (int i = 0; i < sizeArray; i++) {
      switch (signalBuffer[i]) {
          case 1: ritter_send_1(); break;
          case 2: ritter_send_2(); break;
          case 3: ritter_send_3(); break;
          case 5: ritter_start_bit(); break;
          case 9: ritter_end_gap(); break;
      }
    }
  }
}

void ritter_socket_init() {
  pinMode(TRANSMITTER_PIN, OUTPUT);
  digitalWrite(TRANSMITTER_PIN, LOW);
}

