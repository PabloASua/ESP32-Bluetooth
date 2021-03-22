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

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

File myFile;
boolean sendFile = false;
boolean dataReady = false;
String btData;

byte character;

int bufferIndex = 0;

BluetoothSerial SerialBT;


void setup() {
  Serial.begin(9600);

  Serial.print("Initializing SD card...");
  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  pinMode(22, OUTPUT);

  File root = SD.open("/");
  root.rewindDirectory();
  printDirectory(root, 0); //Display the card contents
  root.close();



  SerialBT.begin("WDGTOYOTA_A125336"); //Bluetooth device name Max. 31 Bytes
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
    Serial.println("Sending...");

    // re-open the file for reading:
    myFile = SD.open("/config.json");
    if (myFile) {
      Serial.println("config.json:");
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        character = myFile.read();
        Serial.write(character);
        //SerialBT.println(Serial.write(myFile.read()));
        SerialBT.write(character);
      }
      SerialBT.write('\n');

    }else{Serial.println("File not found.");}
  }


  if (SerialBT.available()) {
    char character = SerialBT.read();
    btData+= character;
    if (character == 10){dataReady = true;} 
  }

  if (dataReady){
    dataReady = false;
    digitalWrite(22,!digitalRead(22));
    Serial.println(btData);
    sendFile = true;
    btData = "";
  }
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