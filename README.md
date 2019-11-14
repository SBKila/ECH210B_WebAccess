# ECH210B_WebAccess
WebInterface to read ECH210B PAC controler

# Installation
You will have to update config.h file to setup Serial pin and Force setup pin.
by default 
  * MODBUS RX_PIN is 13 and TX_PIN is 14
  * The force settings pin is 12.

# Setup
To entrer setup mode, Setup pin should be set to GND and then restart the module.
If no setup as been perform, no need use Setup Pin, module will switch automaticaly to setup mode

In setup mode, module is configured as Access Point.

Using your mobile phone, connect to the AP called **"echmonitor"**, password is **"password"**
then connect to 192.168.4.1 and you will get a setup page.

You can setup:
* your WiFi SSID
* your WiFi password
* the Modbus id of your ECH210B module
* the pulling period of data reading
* the hostname of the module once connected to your WiFi
