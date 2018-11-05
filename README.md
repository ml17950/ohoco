# ohoco
My (o)wn (ho)me (co)ntrol startet with a Raspberry Pi, a 433 MHz sender module and some 433 MHz power sockets.
This was running fine, but after a while I startet with ESP8266 boards and some sensors.

In the first projects, there was always repeating code (connect to wifi, send some values, etc.) and I deceided to create my own libary for this "every-project-code". This lib was designed to work hand in hand with my control-webapp on my Raspberry Pi [ohoco-ui](https://github.com/ml17950/ohoco/tree/master/ohoco-ui)

In this repository want to share this libary and some of the ESP8266 projects.

# installing the libary (Windows)
Just run the **\_win-link-libary.bat** with administrator rights. This will create softlinks in the filesystem. Then you can include the lib with **#include <OHoCo.h>** in your projects.

# installing the web-ui (Linux)
1. download code via `git clone https://github.com/ml17950/ohoco`
2. go to your webserver-root via `cd /var/www/`
3. create a directory via `mkdir ohoco`
4. copy all files from the ohoco-ui directory to the created folder `cp -R ~/ohoco/ohoco-ui /var/www/ohoco`
5. change rights of the config folder via `sudo chmod -R 0777 /var/www/ohoco/config`
6. edit the **_telegram.php** and insert your Telegram Api-Key/Chat-ID
7. add the cron.php to your crontab via `sudo crontab -e` and add following line `* * * * *       php /var/www/ohoco/cron.php`
8. open the ui in your browser
