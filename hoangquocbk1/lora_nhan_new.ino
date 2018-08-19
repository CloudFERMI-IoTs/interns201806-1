#include <SPI.h>
#include <LoRa.h>
String LORA_Status = "Not Initialized";
String LORA_Lastreceived_Msg ="--No data--";

#define SCK     18    // GPIO18  -- SX1278's SCK
#define MISO    36   // GPIO36 -- SX1278's MISO
#define MOSI    5   // GPIO5 -- SX1278's MOSI
#define SS      23   // GPIO23 -- SX1278's CS
#define RST     -1   // GPIO33 -- SX1278's RESET
#define DIO0    39   // GPIO39 -- SX1278's IRQ(Interrupt Request)
#define CR      4     // 4/5
#define LORA_PREAMBLE_LENGTH  10
void setup() {
  Serial.begin(9600);
  while (!Serial);//Doi ket noi voi serial
Serial.println("LoRa Receiver");
SPI.begin(SCK,MISO,MOSI,SS);
LoRa.setPins(SS,RST,DIO0);
Serial.println ("1");
/*Khoi tao gia tri tan so*/
if ( !LoRa.begin (434E6)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  } 
/*cai he so lan truyen*/  
LoRa.setSpreadingFactor(8);
Serial.println ("6");
 LoRa.setCodingRate4(CR);
 Serial.println ("2");
 /*cai bang thong tin hieu cua radio*/  
LoRa.setSignalBandwidth(20.8E3);
LoRa.setPreambleLength(LORA_PREAMBLE_LENGTH);
LoRa.disableCrc();
delay(1000);
Serial.println ("3");
/*Khoi tao gia tri tan so*/
/*if ( !LoRa.begin (433E6)) {
    Serial.println ("Starting LoRa failed!");
    while (1);
  } */



  Serial.println ("4");
}
void loop() {
     // Serial.println ("5");
  // try to parse packet
  int packetSize = LoRa.parsePacket();//Kiem ta xem 1 goi da duoc nhan chua? Tra ve kich thuoc goi theo byte or 0 neu ko nhan dc goi nao
  if (packetSize) {
    // received a packet
    Serial.print("Received packet ");
    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
      Serial.print("Nhiet Do: ");
    }
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
/*
String receiveLoRaMessage() {
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return "";          // if there's no packet, return*/

  // read packet header bytes:
  /*
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  */
  /*
  String incoming = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    incoming += (char)LoRa.read();      // add bytes one by one
  }*/

  /*
  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:   
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);  
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  */
/*
  LORA_Lastreceived_Msg = incoming;
  return incoming;
}
void loop() {  
  // Working
  Serial.println ("2");
  receiveAndForwardLoRaMessage();
}
void receiveAndForwardLoRaMessage(){
  String message = receiveLoRaMessage();
}*/
