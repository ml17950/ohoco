// RCT = Remote Control Transmitter

void turnPowerSocketON(String socket_id) {
  ohoco.debug("RCT >> turnPowerSocketON => " + socket_id);

       if (socket_id == "socket1") { myRCSendSwitch.sendTriState("FFFF00FFFF0F"); }
  else if (socket_id == "socket2") { myRCSendSwitch.sendTriState("FFFF0F0FFF0F"); }
  else if (socket_id == "socket3") { myRCSendSwitch.sendTriState("FFFF0FF0FF0F"); }
  else if (socket_id == "socket4") { myRCSendSwitch.sendTriState("FFFF0FFF0F0F"); }

  else if (socket_id == "socket5") { myRCSendSwitch.sendTriState("FFF0F0FFFF0F"); }
  else if (socket_id == "socket6") { myRCSendSwitch.sendTriState("FFF0FF0FFF0F"); }
  else if (socket_id == "socket7") { myRCSendSwitch.sendTriState("FFF0FFF0FF0F"); }
  else if (socket_id == "socket8") { myRCSendSwitch.sendTriState("FFF0FFFF0F0F"); }

  else if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFF0F"); }

  else if (socket_id == "socket10") { myRCSendSwitch.send("001110101000000100000001"); }

  else { ohoco.debug("unknown socket"); }
}

void turnPowerSocketOFF(String socket_id) {
  ohoco.debug("RCT >> turnPowerSocketOFF => " + socket_id);

       if (socket_id == "socket1") { myRCSendSwitch.sendTriState("FFFF00FFFFF0"); }
  else if (socket_id == "socket2") { myRCSendSwitch.sendTriState("FFFF0F0FFFF0"); }
  else if (socket_id == "socket3") { myRCSendSwitch.sendTriState("FFFF0FF0FFF0"); }
  else if (socket_id == "socket4") { myRCSendSwitch.sendTriState("FFFF0FFF0FF0"); }
    
  else if (socket_id == "socket5") { myRCSendSwitch.sendTriState("FFF0F0FFFFF0"); }
  else if (socket_id == "socket6") { myRCSendSwitch.sendTriState("FFF0FF0FFFF0"); }
  else if (socket_id == "socket7") { myRCSendSwitch.sendTriState("FFF0FFF0FFF0"); }
  else if (socket_id == "socket8") { myRCSendSwitch.sendTriState("FFF0FFFF0FF0"); }
    
  else if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFFF0"); }

  else if (socket_id == "socket10") { myRCSendSwitch.send("001110101000000100000100"); }

  else { ohoco.debug("unknown socket"); }
}

static char * decimal2binary(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}

static const char* binary2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

void PowerSocketCommandReceived(unsigned long decimal, unsigned int length) {
  const char* b = decimal2binary(decimal, length);
  const char* t = binary2tristate(b);
  char* cSocket = "";
  char* cAction = "";
  bool updStatus = false;

  ohoco.debug("RCT >> PowerSocketCommandReceived => " + (String)t);
  
  if      (strcmp(t, "FFFF00FFFF0F") == 0) { cSocket = "socket1"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFFF00FFFFF0") == 0) { cSocket = "socket1"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFFF0F0FFF0F") == 0) { cSocket = "socket2"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFFF0F0FFFF0") == 0) { cSocket = "socket2"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFFF0FF0FF0F") == 0) { cSocket = "socket3"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFFF0FF0FFF0") == 0) { cSocket = "socket3"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFFF0FFF0F0F") == 0) { cSocket = "socket4"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFFF0FFF0FF0") == 0) { cSocket = "socket4"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFF0F0FFFF0F") == 0) { cSocket = "socket5"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFF0F0FFFFF0") == 0) { cSocket = "socket5"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFF0FF0FFF0F") == 0) { cSocket = "socket6"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFF0FF0FFFF0") == 0) { cSocket = "socket6"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFF0FFF0FF0F") == 0) { cSocket = "socket7"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFF0FFF0FFF0") == 0) { cSocket = "socket7"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFF0FFFF0F0F") == 0) { cSocket = "socket8"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFF0FFFF0FF0") == 0) { cSocket = "socket8"; cAction = "off"; updStatus = true; }
  else if (strcmp(t, "FFF000FFFF0F") == 0) { cSocket = "socket9"; cAction = "on";  updStatus = true; }
  else if (strcmp(t, "FFF000FFFFF0") == 0) { cSocket = "socket9"; cAction = "off"; updStatus = true; }

  if (updStatus) {
//    ohoco.debuginline("selectSocket  cSocket: ");
//    ohoco.debug(cSocket);
//    ohoco.debuginline("selectSocket  cAction: ");
//    ohoco.debug(cAction);

    ohoco.set_sensor_value(cSocket, cAction, "");
  
    ohoco.led_flash(4, 100);
  }
}

