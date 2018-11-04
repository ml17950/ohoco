
//                                 |   1    |    1   |   0    |    1   |    1   |   0    |   0    |   0    |   0    |   0    |   0    |    1   |
int HO5500RE_POWER_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,450,1220,450,1220,450,1220,1240,405}; // 11011 0000001
int HO5500RE_SPEED_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,450,1220,450,1220,1240,405,450,1220}; // 11011 0000010
int HO5500RE_SCENE_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,450,1220,1240,405,450,1220,450,1220}; // 11011 0000100
int HO5500RE_TIMER_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,1240,405,450,1220,450,1220,450,1220}; // 11011 0001000
int HO5500RE_MOVER_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,1240,405,450,1220,450,1220,450,1220,450,1220}; // 11011 0010000

int HO5500RE_TEST1_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,450,1220,450,1220,450,1220,450,1220}; // 11011 0000000
int HO5500RE_TEST2_RAW[26] = {0,830,1240,405,1240,405,450,1220,1240,405,1240,405,450,1220,450,1220,450,1220,450,1220,450,1220,1240,405,1240,405}; // 11011 0000011

int HO5500RE_POWER_CODE[12] = {1,1,0,1,1,0,0,0,0,0,0,1};
int HO5500RE_SPEED_CODE[12] = {1,1,0,1,1,0,0,0,0,0,1,0};
int HO5500RE_SCENE_CODE[12] = {1,1,0,1,1,0,0,0,0,1,0,0};
int HO5500RE_TIMER_CODE[12] = {1,1,0,1,1,0,0,0,1,0,0,0};
int HO5500RE_MOVER_CODE[12] = {1,1,0,1,1,0,0,1,0,0,0,0};

const int SEND_LOOPS = 4;

void HO5500RE_send_pulse_code(int pulsearr[], int len) {
  ohoco.println("FAN >> HO5500RE_send_pulse_code");
  
  for (int i = 0; i < len; i++) {
    pulse_micros(pulsearr[i++]);
    space_micros(pulsearr[i]);
  }
}

void HO5500RE_send_bin_code(int binarr[], int len, int loops) {
  ohoco.println("FAN >> HO5500RE_send_bin_code");
  
  // pulsearr = 26 items -> 1 init delay + 12 pulses
  int pulsearr[26] = {0,830};
  int pos = 2;
  
  for (int i = 0; i < len; i++) {
    if (binarr[i] == 1) {
      pulsearr[pos++] = 1240;
      pulsearr[pos++] = 405;
    }
    else {
      pulsearr[pos++] = 450;
      pulsearr[pos++] = 1220;
    }
  }

  space_micros(6000);
  for (int l=0; l<loops; l++) {
    HO5500RE_send_pulse_code(pulsearr, 26);
  }
}

void HO5500RE_TEST1() {
  ohoco.println("FAN >> HO5500RE_TEST1");

  int sizeArray = sizeof(HO5500RE_TEST1_RAW) / sizeof(int);
  
  space_micros(6000);
  
  for (int i = 0; i < sizeArray; i++) {
    pulse_micros(HO5500RE_TEST1_RAW[i++]);
    space_micros(HO5500RE_TEST1_RAW[i]);
  }
}

void HO5500RE_TEST2() {
  ohoco.println("FAN >> HO5500RE_TEST2");

  int sizeArray = sizeof(HO5500RE_TEST2_RAW) / sizeof(int);
  
  space_micros(6000);
  
  for (int i = 0; i < sizeArray; i++) {
    pulse_micros(HO5500RE_TEST2_RAW[i++]);
    space_micros(HO5500RE_TEST2_RAW[i]);
  }
}

void HO5500RE_POWER() {
  ohoco.println("FAN >> HO5500RE_POWER");

  int sizeArray = sizeof(HO5500RE_POWER_RAW) / sizeof(int);

  for (int i=0; i<SEND_LOOPS; i++) {
    space_micros(6000);
    
    for (int i = 0; i < sizeArray; i++) {
      pulse_micros(HO5500RE_POWER_RAW[i++]);
      space_micros(HO5500RE_POWER_RAW[i]);
    }
  }
}

void HO5500RE_SPEED() {
  ohoco.println("FAN >> HO5500RE_SPEED");

  int sizeArray = sizeof(HO5500RE_SPEED_RAW) / sizeof(int);

  for (int i=0; i<SEND_LOOPS; i++) {
    space_micros(6000);
    
    for (int i = 0; i < sizeArray; i++) {
      pulse_micros(HO5500RE_SPEED_RAW[i++]);
      space_micros(HO5500RE_SPEED_RAW[i]);
    }
  }
}

void HO5500RE_MOVER() {
  ohoco.println("FAN >> HO5500RE_MOVER");

  int sizeArray = sizeof(HO5500RE_MOVER_RAW) / sizeof(int);

  for (int i=0; i<SEND_LOOPS; i++) {
    space_micros(6000);
    
    for (int i = 0; i < sizeArray; i++) {
      pulse_micros(HO5500RE_MOVER_RAW[i++]);
      space_micros(HO5500RE_MOVER_RAW[i]);
    }
  }
}

void HO5500RE_TIMER() {
  ohoco.println("FAN >> HO5500RE_TIMER");

  int sizeArray = sizeof(HO5500RE_TIMER_RAW) / sizeof(int);

  for (int i=0; i<SEND_LOOPS; i++) {
    space_micros(6000);
    
    for (int i = 0; i < sizeArray; i++) {
      pulse_micros(HO5500RE_TIMER_RAW[i++]);
      space_micros(HO5500RE_TIMER_RAW[i]);
    }
  }
}

void HO5500RE_SCENE() {
  ohoco.println("FAN >> HO5500RE_SCENE");

  int sizeArray = sizeof(HO5500RE_SCENE_RAW) / sizeof(int);

  for (int i=0; i<SEND_LOOPS; i++) {
    space_micros(6000);
    
    for (int i = 0; i < sizeArray; i++) {
      pulse_micros(HO5500RE_SCENE_RAW[i++]);
      space_micros(HO5500RE_SCENE_RAW[i]);
    }
  }
}

