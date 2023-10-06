// This code is based on the Adafruit example code. 
// It receives commands over Bluetooth from a phone that will turn on a water gun.
/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
const int ledPin = 13;
const int TriggerPin = 12;

#include <string.h>
#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"

#define FACTORYRESET_ENABLE         1   // set to 0 if no reset wanted
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

#define BLUEFRUIT_HWSERIAL_NAME      Serial1  // this is I/O's 0 and 1 on a Teensy LC
#define BLUEFRUIT_UART_MODE_PIN        2    // I/O 2 is wired to mode pin. Set to -1 if mode pin is not wired

#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false  // If set to 'true' enables debug output
#define BLE_READPACKET_TIMEOUT         500   // Timeout in ms waiting to read a response

Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

void setup() {
  // initialize the digital pins as outputs.
  pinMode(TriggerPin, INPUT); // Let pull down turn off the NFET
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(1000);

  if ( !ble.begin(VERBOSE_MODE) )
  {
//    
  }

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    if ( ! ble.factoryReset() ){
//
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);
}

void loop() {
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */

  // Buttons are the only packets looked at
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    if (buttnum == 1) {
      if (pressed) {
        digitalWrite(ledPin, HIGH);   // set the LED on
        pinMode(TriggerPin, OUTPUT); // Make output
        digitalWrite(TriggerPin, HIGH); // drive the gun trigger on
      }
      else {  // released
        digitalWrite(ledPin, LOW);   // set the LED off
        pinMode(TriggerPin, INPUT); // Let pull down turn off the NFET and no gun trigger
      }
    }
  }
}

