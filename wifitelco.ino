/* ====== WIFITELCO ======
 * Wifi enabled AIRCO remote
 * (Updated 14 Feb 2016)
 * ==========================
 *
 * Change SSID and PASS to match your WiFi settings.
 * The IP address is displayed to soft serial upon successful connection.
 *
 * Philippe Rigaux
 * Tested On Arduino Mega 2560 with ESP8266 serial Wifi adapter
 */

#define BUFFER_SIZE 512

// Wifi Setup
#define SSID  "salon"      // change this to match your WiFi SSID
#define PASS  "B00CB00CB00CB00C"  // change this to match your WiFi password
#define PORT  "8080"           // using port 8080 by default
//always high
int CH_PD_8266 = 53;

// IR SETUP
#include "IRremote.h"

IRsend irsend;
int khz=38; //NB Change this default value as neccessary to the correct modulation frequency
// ON and 16 C° with AUTO FAN heat
unsigned heat16[] = {8900,4450,600,1600,600,1600,600,500,550,550,550,550,500,550,550,1650,600,1600,600,1600,650,1550,650,1600,600,450,550,550,550,550,550,1650,600,500,550,500,600,500,550,550,550,550,550,550,550,1650,600,1600,600,1600,600,500,550,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,550,550,550,500,550,550,550,550,550,1650,600,500,550,1650,600,500,550,500,600,500,550,550,550,550,550,500,600,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,550,500,600,1650,600,450,600,500,550,550,550,550,550,500,600,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,550,500,600,500,550,550,550,550,550,550,550,500,600,500,550,1650,550,1650,550,550,550,550,550,550,550,500,550,550,550,550,550,550,550,500,600,500,550,550,550,1650,550,550,550,1650,550,550,550,500,600,500,550,550,550,550,550,1650,550,1650,550,1650,550,1650,550,1650,550,1650,550,550,550,550,600};
// OFF the A/C
unsigned off[] = {8950,4400,600,1600,650,1550,650,450,600,500,600,500,600,500,600,1600,600,1600,600,1600,600,1600,600,1600,600,500,600,500,600,450,600,1600,600,500,600,500,600,500,600,450,650,450,600,500,600,1600,600,1600,600,1600,600,500,600,500,600,500,600,450,650,450,600,500,600,500,600,450,650,450,600,500,600,500,600,500,600,450,650,1550,650,450,600,1600,600,500,600,500,600,500,600,450,650,450,600,500,600,500,600,450,650,450,600,500,600,500,600,500,600,450,600,500,600,1600,600,1600,600,500,600,500,600,500,600,450,600,500,600,500,600,500,600,450,650,450,600,500,600,500,600,500,600,450,650,450,600,500,600,500,600,450,650,450,600,500,600,500,600,500,600,450,650,450,600,500,600,500,600,450,650,450,600,500,600,500,600,500,600,450,650,450,600,1600,600,500,600,1600,600,500,600,500,600,450,650,450,600,500,600,1600,600,1600,600,1600,600,1600,550,550,600,500,600,1600,600,500,600};
// ON and 17 C° with AUTO FAN heat
unsigned heat17[] = {8900,4450,550,1650,550,1650,600,550,500,550,550,550,550,550,550,1650,550,1650,550,1650,550,1650,550,1650,550,1650,550,550,550,550,550,1650,550,550,550,550,500,550,550,550,550,550,550,550,500,1650,550,1650,550,1650,600,550,500,600,500,550,550,550,550,500,550,600,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,1650,550,500,600,1650,550,500,550,600,500,550,550,550,550,550,550,550,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,500,600,550,500,1650,550,550,550,550,550,550,550,500,600,500,550,550,550,550,550,500,600,500,550,550,550,550,550,550,550,500,600,500,550,550,550,550,550,500,600,500,550,550,550,550,550,1650,550,1650,550,550,550,550,550,500,600,500,550,550,550,550,550,500,550,550,550,550,550,550,550,1650,550,550,550,1650,550,500,600,500,550,550,550,550,550,550,550,1650,550,1650,550,1650,550,550,550,500,550,550,550,1650,550,550,600};


char buffer[BUFFER_SIZE];

// Use the port below
#define dbg Serial    // use Serial for debug
#define esp Serial3   // use Serial2 to talk to esp8266

// By default we are looking for OK\r\n
char OKrn[] = "OK\r\n";
byte wait_for_esp_response(int timeout, char* term=OKrn) {
  unsigned long t=millis();
  bool found=false;
  int i=0;
  int len=strlen(term);
  // wait for at most timeout milliseconds
  // or if OK\r\n is found
  while(millis()<t+timeout) {
    if(esp.available()) {
      buffer[i++]=esp.read();
      if(i>=len) {
        if(strncmp(buffer+i-len, term, len)==0) {
          found=true;
          break;
        }
      }
    }
  }
  buffer[i]=0;
  dbg.print("'");
  dbg.print(buffer);
  dbg.print("'");
  return found;
}

void setup() {
  pinMode(CH_PD_8266, OUTPUT);
  digitalWrite(CH_PD_8266, HIGH);

  // assume esp8266 operates at 115200 baud rate
  // change if necessary to match your modules' baud rate
  esp.begin(115200);
  
  dbg.begin(115200);
  dbg.println("begin.");
    
  setupWiFi();

  // print device IP address
  dbg.print("device ip addr:");
  esp.println("AT+CIFSR");
  wait_for_esp_response(1000);
}

bool read_till_eol() {
  static int i=0;
  if(esp.available()) {
    buffer[i++]=esp.read();
    if(i==BUFFER_SIZE)  i=0;
    if(i>1 && buffer[i-2]==13 && buffer[i-1]==10) {
      buffer[i]=0;
      i=0;
      dbg.print(buffer);
      return true;
    }
  }
  return false;
}

void loop() {
  int ch_id, packet_len;
  char *pb;  
  if(read_till_eol()) {
    if(strncmp(buffer, "+IPD,", 5)==0) {
      // request: +IPD,ch,len:data
      sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
      if (packet_len > 0) {
        // read serial until packet_len character received
        // start from :
        pb = buffer+5;
        while(*pb!=':') pb++;
        pb++;
        if (strncmp(pb, "GET /16HEAT", 11) == 0) {
          wait_for_esp_response(1000);
          dbg.println("-> 16HEAT");
          serve_cmd(ch_id,"16HEAT");
        }
        else if (strncmp(pb, "GET /17HEAT", 11) == 0) {
          wait_for_esp_response(1000);
          dbg.println("-> 17HEAT");
          serve_cmd(ch_id,"17HEAT");
        }
        else if (strncmp(pb, "GET /OFF", 8) == 0) {
          wait_for_esp_response(1000);
          dbg.println("-> OFF");
          serve_cmd(ch_id,"OFF");
        }
        else {
          dbg.println("-> UNKNOW COMMAND");
          serve_cmd(ch_id,"UNKNOWN");
        }
      }
    }
  }
}

void serve_cmd(int ch_id,String cmd) {
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n";

  String content="";
  // Output Some informations and actually send the IR command
  if (cmd=="16HEAT") {
    content += "Air Conditioner set to HEAT 16°C<br />\n";
    irsend.sendRaw(heat16, sizeof(heat16)/sizeof(int), khz);
    delay (1000);
    irsend.sendRaw(heat16, sizeof(heat16)/sizeof(int), khz);
    dbg.println("-> 16HEAT cmd sent");
  }
  else if (cmd=="17HEAT") {
    content += "Air Conditioner set to HEAT 17°C<br />\n";
    irsend.sendRaw(heat17, sizeof(heat17)/sizeof(int), khz);
    delay (1000);
    irsend.sendRaw(heat17, sizeof(heat17)/sizeof(int), khz);
    dbg.println("-> 17HEAT cmd sent");
  }
   else if (cmd=="OFF") {
    content += "Air Conditioner turned off<br />\n";
    irsend.sendRaw(off, sizeof(off)/sizeof(int), khz);
    delay (1000);
    irsend.sendRaw(off, sizeof(off)/sizeof(int), khz);
    dbg.println("-> OFF cmd sent");
  }
  else
  {
    content += "UNKNOW COMMAND!<br />\n";
  }
  

  header += "Content-Length:";
  header += (int)(content.length());
  header += "\r\n\r\n";
  esp.print("AT+CIPSEND=");
  esp.print(ch_id);
  esp.print(",");
  esp.println(header.length()+content.length());
  if(wait_for_esp_response(2000, "> ")) {
    esp.print(header);
    esp.print(content);
  } else {
    esp.print("AT+CIPCLOSE=");
    esp.println(ch_id);
  }
}


void setupWiFi() {
  // try empty AT command
  esp.println("AT");
  wait_for_esp_response(1000);

  // set mode 1 (client)
  esp.println("AT+CWMODE=1");
  wait_for_esp_response(1000);  

  // reset WiFi module
  esp.print("AT+RST\r\n");
  wait_for_esp_response(1500);
  delay(3000);
 
  // join AP
  esp.print("AT+CWJAP=\"");
  esp.print(SSID);
  esp.print("\",\"");
  esp.print(PASS);
  esp.println("\"");
  // this may take a while, so wait for 5 seconds
  wait_for_esp_response(15000);
  
  esp.println("AT+CIPSTO=30");  
  wait_for_esp_response(1000);

  // start server
  esp.println("AT+CIPMUX=1");
  wait_for_esp_response(1000);
  
  esp.print("AT+CIPSERVER=1,"); // turn on TCP service
  esp.println(PORT);
  wait_for_esp_response(1000);
  
    
}
