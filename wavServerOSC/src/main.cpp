/*
 * main.cpp
 * 
 * Copyright 2019 Badgitz <Badgitz@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <Arduino.h>
#include <ArduinoOSC.h>

// WiFi stuff
const char* ssid = "GitzNet";
const char* pwd = "GitzNet321";
static int ledToggle = 1;
static bool getMode = false;

// for ArduinoOSC
OscWiFi osc;
const char* host = "192.168.43.126";
const int recv_port = 8888;
const int send_port = 9999;

String fileName;

/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Vorlage-------------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void onOscReceived(OscMessage& m)
{
    Serial.print("callback : ");
    Serial.print(m.ip()); Serial.print(" ");
    Serial.print(m.port()); Serial.print(" ");
    Serial.print(m.size()); Serial.print(" ");
    Serial.print(m.address()); Serial.print(" ");
    Serial.print(m.arg<int>(0)); Serial.print(" ");
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Reads the Serial Connection-----------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

int getSerialCommand() {
  if (Serial.available() > 0) {
    int command = Serial.read();
    return command;
  }
  else return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Executes given Commands---------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void commandExecution(int command) {
  uint8_t waitForFile = 1;
  uint8_t ledToggle2 = 1;
  switch(command) {
    case 0x74:
      Serial.println("Sending Test");
      osc.send(host, send_port, "/test", "Test Message!");
      break;
    case 0x67:
      Serial.println("Get Song started");
      while (waitForFile) {                         //waiting for dsp board to send filename
        digitalWrite(2,  ledToggle2 ^= 1);
        if (Serial.available() > 0) {
          digitalWrite(2, HIGH);
          fileName = Serial.readString();
          osc.send(host, send_port, "/getFile", fileName);      //requesting server
          Serial.println(fileName);
          waitForFile = 0;
          getMode = true; 
        }
      }
      break;
    case 0x66:
      Serial.println("Get FileList started");
      osc.send(host, send_port, "/getList", 1);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Setup---------------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(500000);

    // WiFi stuff
    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
    Serial.println("");
    Serial.println("WiFi connected, IP : "); Serial.println(WiFi.localIP());

    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    pinMode(5, INPUT);
    pinMode(16, OUTPUT);
    // ArduinoOSC
    osc.begin(recv_port);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/If hello Message is received----------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

    osc.subscribe("/hello", [](OscMessage& m)
    {
      Serial.println(m.arg<int>(1));
      if (m.arg<int>(1) == 0xff10ff) {
        Serial.println("FFFFFFFF"); //32 BIT Zahl als Ascii
      }
    });

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/if files Message is received----------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

    osc.subscribe("/files", [](OscMessage& m)
    {
      int index = m.arg<int>(0);
      Serial.println(index);
      for (int i = 1; i < index; i++) {
        Serial.println(m.arg<String>(i));
      }
    });

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/if data Message is received----------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////    

    osc.subscribe("/data", [](OscMessage& m){
      if (getMode) {
        digitalWrite(16, HIGH);
        Serial.println(m.arg<String>(0));
        digitalWrite(16, LOW);
      }
    });

    osc.subscribe("/info", [](OscMessage& m){
      if (getMode) {
        /*
        Serial.println(m.arg<int>(0));
        Serial.println(m.arg<int>(1));
        Serial.println(m.arg<int>(2));
        */
        char info[8];
        if (m.arg<int>(1) < 16) {
          sprintf(info, "%X0%X0%X", m.arg<int>(0), m.arg<int>(1), m.arg<int>(2));
        }
        else
          sprintf(info, "%X%X0%X", m.arg<int>(0), m.arg<int>(1), m.arg<int>(2));
        Serial.println(info);
      }
    });

     osc.subscribe("/end", [](OscMessage& m){
      getMode = false;
    });

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------/Main Loop-----------------------------------------------------------//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
    osc.parse(); // should be called

    if (Serial.available() > 0) {
      getMode = false;
      int command = Serial.read();
      Serial.println("Incoming");
      Serial.println(command);
      commandExecution(command);
  }
    digitalWrite(0, ledToggle ^= 1);
}
