/*
    SIGFOX NFC

    Demonstrate how to use NFC on the SmartEverything dev kit

    created 18 May 2016
    Authors: Louis Moreau (@luisomoreau)

    This example is in the public domain
    https://github.com/luisomoreau
*/


#include <Arduino.h>
#include <Wire.h>
#include "nfc.h"
NFC NFC(NFC::NFC_I2C_2K, 4, 7, 0x55);

void setup(){
    SerialUSB.begin(115200);
    SerialUSB.println("start");
    if(!NFC.begin()){
        SerialUSB.println("Can't find NFC");
    }
    delay(2000);
    detectPassiveTag();
    //SerialUSB.println(NFC.readerPresent());
    //getSerialUSBNumber();
    //testUserMem();
    //testRegisterAccess();
    //testSramMirror();
    //testSram();
    
}

void loop(){
  if(isButtonOnePressed()){
    readRegister();
  }
  if(isButtonTwoPressed()){
    getUid();
  }
  
  delay(500);
}

//void testWriteAdapter(){
//    NdefMessage message = NdefMessage();
//    message.addUriRecord("http://www.google.fr");
//    if(NFCAdapter.write(message)){
//        SerialUSB.println("Message written to tag.");
//    }
//}


void getUid(){
  byte uid[7];
  if(NFC.getUid(uid, 7)){
     showBlockInHex(uid,7);
  }
}

void detectPassiveTag(){
    byte data[16];
    for(byte i=0;i<16;i++){
        data[i]=0x00;
    }
    SerialUSB.println("cleaning SRAM block 0xF8");
    if(!NFC.writeSram(0,data,16)){
        return;
    }
    SerialUSB.println("Reading SRAM block 0xF8");
    if(NFC.readSram(0,data,16)){
        showBlockInHex(data,16);
    }
    if(NFC.setFd_ReaderHandshake()){
       SerialUSB.print("Success setting Fd");
    }else{
      SerialUSB.print("Error setting Fd");
    }
    NFC.setSramMirrorRf(true, 0x03);
}

void testSram(){
    byte data[16];
    SerialUSB.println("Reading SRAM block 0xF8");
    if(NFC.readSram(0,data,16)){
        showBlockInHex(data,16);
    }
    for(byte i=0;i<16;i++){
        data[i]=0xF0 | i;
    }
    SerialUSB.println("Writing dummy data to SRAM block 0xF8");
    if(!NFC.writeSram(0,data,16)){
        return;
    }
    SerialUSB.println("Reading SRAM block 0xF8 again");
    if(NFC.readSram(0,data,16)){
        showBlockInHex(data,16);
    }
}

void readRegister(){
    byte data;
    SerialUSB.print("NC_REG: ");
    if(NFC.readRegister(NFC::NC_REG, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("LAST_NDEF_BLOCK: ");
    if(NFC.readRegister(NFC::LAST_NDEF_BLOCK, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("SRAM_MIRROR_BLOCK: ");
    if(NFC.readRegister(NFC::SRAM_MIRROR_BLOCK, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("WDT_LS: ");
    if(NFC.readRegister(NFC::WDT_LS, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("WDT_MS: ");
    if(NFC.readRegister(NFC::WDT_MS, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("I2C_CLOCK_STR: ");
    if(NFC.readRegister(NFC::I2C_CLOCK_STR, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
    SerialUSB.print("NS_REG: ");
    if(NFC.readRegister(NFC::NS_REG, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println();
}

void testSramMirror(){
    byte readeeprom[16];
    byte data;

    if(!NFC.setSramMirrorRf(false,1))return;
    SerialUSB.println("\nReading memory block 1, no mirroring of SRAM");
    if(NFC.readEeprom(0,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
    SerialUSB.println("\nReading SRAM block 1");
    if(NFC.readSram(0,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
    if(!NFC.setSramMirrorRf(true,1))return;
    SerialUSB.print("NC_REG: ");
    if(NFC.readRegister(NFC::NC_REG, data)){
        SerialUSB.println(data, HEX);
    }
    SerialUSB.println("Use an NFC-reader to verify that the SRAM has been mapped to the memory area that the reader will access by default.");
}

void testRegisterAccess(){
    byte data;
    SerialUSB.println(NFC.readRegister(NFC::NC_REG,data));
    SerialUSB.println(data,HEX);
    SerialUSB.println(NFC.writeRegister(NFC::NC_REG,0x0C,0x0A));
    SerialUSB.println(NFC.readRegister(NFC::NC_REG,data));
    SerialUSB.println(data,HEX);
}

void testUserMem(){
    byte eepromdata[2*16];
    byte readeeprom[16];

    for(byte i=0;i<2*16;i++){
        eepromdata[i]=0x80 | i;
    }

    SerialUSB.println("Writing block 1");
    if(!NFC.writeEeprom(0,eepromdata,16)){
        SerialUSB.println("Write block 1 failed");
    }
    SerialUSB.println("Writing block 2");
    if(!NFC.writeEeprom(16,eepromdata+16,16)){
        SerialUSB.println("Write block 2 failed");
    }
    SerialUSB.println("\nReading memory block 1");
    if(NFC.readEeprom(0,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
    SerialUSB.println("Reading memory block 2");
    if(NFC.readEeprom(16,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
    SerialUSB.println("Reading bytes 10 to 20: partly block 1, partly block 2");
    if(NFC.readEeprom(10,readeeprom,10)){
        showBlockInHex(readeeprom,10);
    }
    SerialUSB.println("Writing byte 15 to 20: partly block 1, partly block 2");
    for(byte i=0;i<6;i++){
        eepromdata[i]=0x70 | i;
    }
    if(NFC.writeEeprom(15,eepromdata,6)){
        SerialUSB.println("Write success");
    }
    SerialUSB.println("\nReading memory block 1");
    if(NFC.readEeprom(0,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
    SerialUSB.println("Reading memory block 2");
    if(NFC.readEeprom(16,readeeprom,16)){
        showBlockInHex(readeeprom,16);
    }
}

void showBlockInHex(byte* data, byte size){
    for(int i=0;i<size;i++){
        SerialUSB.print(data[i],HEX);
        SerialUSB.print(" ");
    }
    SerialUSB.println();
}
