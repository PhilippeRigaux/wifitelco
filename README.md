# wifitelco
AirConditioner wifi remote, it is basically intented to be used with Jeedom or any smart house system that can send HTTP request.

It get IP address with DHCP so you should fix the served IP to ease the smart home system configuration.

Currently supported commands:

http://xxx.xxx.xxx.xxx/16HEAT -> set the airco to Heating 16°C

http://xxx.xxx.xxx.xxx/17HEAT -> set the airco to Heating 17°C

http://xxx.xxx.xxx.xxx/OFF -> Turn Off Airco


Hardware needed:

1 Arduino ( tested on Arduino Mega 2560 )

1 Serial Wifi ( tested with ESP8266 Serial WIFI Wireless Transceiver )

1 IR LED ( transmitter )


Knowledge needed:

Need to get the original IR codes from the manufacturer remote..

See: http://www.instructables.com/id/Air-Conditioning-web-controlled-by-Arduino/step2/Getting-the-codes/


