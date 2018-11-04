// RCT = Remote Control Transmitter

void turnPowerSocketON(String socket_id) {
  ohoco.println("RCT >> turnPowerSocketON => " + socket_id);

  if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFF0F"); }
  else { ohoco.println("unknown socket"); }
}

void turnPowerSocketOFF(String socket_id) {
  ohoco.println("RCT >> turnPowerSocketOFF => " + socket_id);

  if (socket_id == "socket9") { myRCSendSwitch.sendTriState("FFF000FFFFF0"); }
  else { ohoco.println("unknown socket"); }
}

