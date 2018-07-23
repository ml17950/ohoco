# ohoco
My (o)wn (ho)me (co)ntrol startet with a Raspberry Pi, a 433 MHz sender module and some 433 MHz power sockets.
This was running fine, but after a while I startet with ESP8266 boards and some sensors.

In the first projects, there was always repeating code (connect to wifi, send some values, etc.) and I deceided to create my own libary for this "every-project-code". This lib was designed to work hand in hand with my control-webapp on my Raspberry Pi [RIFT3](https://github.com/ml17950/rift3)

In this repository want to share this libary and some of the ESP8266 projects.

# installing the libary (Windows)
Just run the **\_win-link-libary.bat** with administrator rights. This will create softlinks in the filesystem. Then you can include the lib with **#include <OHoCo.h>** in your projects.
