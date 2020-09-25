/*
  This application is sampling analog input A0 of ESP8266
  so it then be displayed it on-line in e.g a web brower 

  Flie: WebSocket-Scope.ino by krzychb at gazeta.pl
  Rev: 1.0.0 on 4-Mar-2016
  
  To make it run you need another application 
  to send request for data using web sockets

  Copyright (c) 2016 Krzysztof Budzynski. All rights reserved.
  This file is part of OnlineHumidifier project - https://github.com/krzychb/OnlineHumidifier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ESP8266WiFi.h>

const char* ssid = "********";
const char* password = "********";

//
// using Library Manager in Arduino IDE
// Install WebSocketes by Markus Sattler
// https://github.com/Links2004/arduinoWebSockets
//
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t socketNumber;
unsigned long messageNumber;

// tracking of number of Wi-Fi reconnects
// and total connection time
unsigned long numberOfReconnects;
unsigned long millisConnected;

//
// Continous samplig rate of A0 in this application
// is about 12 samples/ms
// Wi-Fi connection gets stuck if continous A0 sampling 
// is longer than 60ms
// therefore maximum of 720 samples can be made
// 
// number of samples taken
#define NUMBER_OF_SAMPLES 720
unsigned int samples[NUMBER_OF_SAMPLES];
unsigned int numberOfSamples = NUMBER_OF_SAMPLES;

// take samples for initial reading
// greater than trigger value below
#define SAMPLE_TRESHOLD 50
unsigned int sampleTreshold = SAMPLE_TRESHOLD;

// Naximum waiting time in ms for a trigger
#define TRIGGER_TIMEOUT 500

// state machine states
unsigned int state;
#define SEQUENCE_IDLE 0x00
#define GET_SAMPLE 0x10
#define GET_SAMPLE__WAITING 0x12

unsigned long millisLastSample;
unsigned long millisWaitingForTrigger;


void analogSample(void)
{
  if (state == SEQUENCE_IDLE)
  {
    return;
  }
  else if (state == GET_SAMPLE)
  {
    millisWaitingForTrigger = millis();
    state = GET_SAMPLE__WAITING;
    return;
  }
  else if (state == GET_SAMPLE__WAITING)
  {
    if (millis() > millisWaitingForTrigger + TRIGGER_TIMEOUT)
    {
      String message = "# " + String(messageNumber) + " -1";
      webSocket.sendTXT(socketNumber, message);
      state = SEQUENCE_IDLE;
      return;
    }
    // ninimum sample period every 1ms
    else if (millis() > millisLastSample)
    {
      samples[0] = analogRead(A0);
      if (samples[0] > sampleTreshold)
      {
        for (int i = 1; i < numberOfSamples; i++)
        {
          samples[i] = analogRead(A0);
        }
        String message = "# " + String(messageNumber) + " ";
        for (int i = 0; i < numberOfSamples; i++)
        {
          message = message + String(samples[i]) + ";";
        }
        message[message.length() - 1] = '\0';
        webSocket.sendTXT(socketNumber, message);
        state = SEQUENCE_IDLE;
        millisLastSample = millis();
      }
    }
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      //
      state = SEQUENCE_IDLE;
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message back to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      //
      // Format of message to process
      // # MESSAGE_NUMBER SAMPLE_TRESHOLD NUMBER_OF_SAMPLES
      // other fromats are ignored
      //
      if (payload[0] == '#')
      {
        char* token = strtok((char*) &payload[2], " ");
        messageNumber = (unsigned long) strtol(token, '\0', 10);
        token = strtok('\0', " ");
        sampleTreshold = (unsigned int) strtol(token, '\0', 10);
        token = strtok('\0', " ");
        numberOfSamples = (unsigned int) strtol(token, '\0', 10);
        //
        // do not exceed the size of table to store samples
        //
        if(numberOfSamples > NUMBER_OF_SAMPLES)
        {
          numberOfSamples = NUMBER_OF_SAMPLES;
        }
        socketNumber = num;
        state = GET_SAMPLE;
      }
      else
      {
        Serial.printf("[%u] get Text: %s\n", num, payload);
      }
      break;
    case WStype_BIN:
      Serial.printf("[%u] get binary lenght: %u\n", num, lenght);
      hexdump(payload, lenght);
      // send message to client
      // webSocket.sendBIN(num, payload, lenght);
      break;
  }
}


void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("WebSocket-Scope.ino");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSockets started");
}



void keepAlive(void)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("not connected");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(500);
    }
    numberOfReconnects++;
    millisConnected = millis();
  }
}


void loop(void)
{
  keepAlive();
  webSocket.loop();
  analogSample();
}

