#include <Arduino.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>

//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"


void printDirectory(File dir, int numTabs);
void SendString (String str);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

File myFile;
boolean sendFile = false;
boolean dataReady = false;
String btData;
String fileName;

byte character;

int bufferIndex = 0;

BluetoothSerial SerialBT;



void BTcallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
  }

  if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("Client disconnected");
    
    ESP.restart();
  }
}


void setup() {
  Serial.begin(9600);

/*
  Serial.print("Initializing SD card...");
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
*/

  pinMode(22, OUTPUT);

  //File root = SD.open("/");
  //root.rewindDirectory();
  //printDirectory(root, 0); //Display the card contents
  //root.close();


  SerialBT.register_callback(BTcallback);
  SerialBT.begin("WDGFORKLIFT_A2234436"); //Bluetooth device name Max. 31 Bytes
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  /*
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  */

  if (sendFile){
  
    sendFile = false;
    Serial.print("Sending...");

    // re-open the file for reading:

    /*
    myFile = SD.open(fileName);
    if (myFile) {
      //Serial.println(fileName + ":");
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        character = myFile.read();
        SerialBT.write(character);
      }
      SerialBT.write('\n');
      
      Serial.println("Done!");

    }
    else{Serial.println("File not found.");}
    */
    Serial.println("File not found.");
    SendString("FNF");

  }






  //Test if there is data available to read from input buffer
  if (SerialBT.available()) {
    char character = SerialBT.read();
    btData+= character;
    if (character == 10){
      
      if (btData.substring(0,2) =="cmd"){
      }
      else{
        fileName = "/" + btData;
        Serial.print("Received file: ");
        Serial.println(fileName);
        sendFile = true;
      }
      digitalWrite(22,!digitalRead(22));
      btData ="";
    } 
  }
}




void SendString (String str){

  int strLength = str.length();
  
  for (int index = 0; index < strLength; index ++){
    SerialBT.write(str[index]);
  }

  SerialBT.write('\n');
}



void printDirectory(File dir, int numTabs)
{

    while (true)
    {
        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            break;
        }
        if (numTabs > 0)
        {
            for (uint8_t i = 0; i <= numTabs; i++)
            {
                Serial.print('\t');
            }
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}