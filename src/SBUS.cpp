/* 
* code for shifting bits into channel used from 
* https://os.mbed.com/users/Digixx/code/SBUS-Library_16channel//file/83e415034198/FutabaSBUS/FutabaSBUS.cpp/
* mbed R/C Futaba SBUS Library
* Copyright (c) 2011-2012 digixx
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*/
#include "SBus.h"


SBus::SBus(HardwareSerial& serialport)
{
  port = &serialport;
  int16_t loc_channels[18]  = {
    1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 0, 0
  };

  port->begin(BAUDRATE,SERIAL_8E1);
  memcpy(channels, loc_channels, 18);

  failsafe_status = NOCONNECTION;
  bufferIndex = 0;
  receiveState = WAITINGFORSTART;
  timer.startRepeat(50);
}

int16_t* SBus::getChannels() {
  // Read channel data
  return channels;
  /* if ((ch > 0) && (ch <= 16)) {
    return channels[ch - 1];
  }
  else {
    return 1023;
  } */
}



SBus::SIGNAL_STATUS SBus::getStatus(void) {
  return failsafe_status;
}

SBus::SIGNAL_STATUS SBus::UpdateChannels(void)
{
  RECEIVER_STATE state = Parse();
  
  switch (state)
  {
    case RECEIVING:
      //Serial.println("receiving");
      break;
    case ERROR:
      //Serial.println("in error state");
      failsafe_status = LOST;
      break;
    case RECEIVED:
      {
        //clear all the bytes first as we are or-ing them later on
        memset(channels,0,16); 
        // reset counters
        uint8_t byte_in_sbus = 1;
        uint8_t bit_in_sbus = 0;
        uint8_t ch = 0;
        uint8_t bit_in_channel = 0;

        // process actual sbus data
        for (uint8_t i=0; i<176; i++) 
        {
          if (sbusData[byte_in_sbus] & (1<<bit_in_sbus)) 
          {
            channels[ch] |= (1<<bit_in_channel);
          }
          bit_in_sbus++;
          bit_in_channel++;

          if (bit_in_sbus == 8) 
          {
            bit_in_sbus =0;
            byte_in_sbus++;
          }
          if (bit_in_channel == 11)
          {
            bit_in_channel =0;
            ch++;
          }
        }
        failsafe_status = SIGNAL_STATUS::OK;
        // Failsafe
        if (sbusData[23] & (1 << 2)) 
        {
            //Serial.print("in lost ");
            //Serial.println(millis());
          failsafe_status = SIGNAL_STATUS::LOST;
        }
        else if (sbusData[23] & (1 << 3)) 
        {
          //Serial.println("in failsafe state");
          failsafe_status = FAILSAFE;
        }
      }
      break;
    default:
      //Serial.println("in default state");
      failsafe_status=LOST;
      break;
  }
  return failsafe_status;
}

SBus::RECEIVER_STATE SBus::Parse()
{
  uint8_t inData;
  RECEIVER_STATE status = RECEIVING; // we assume we will fail

  switch (receiveState) 
  {
    case WAITINGFORSTART: // we are waiting for the first byte to arrive
      if (port->available() > 0) 
      {
        inData = port->read(); // read one byte
        if (inData != 0x0f) // we will throw away any byte that is received that is not the start byte
        {
          while (port->available() > 0) 
          { //read the contents of in buffer and discard it, whe are out of sync
            inData = port->read();
          }
        }
        else {
          bufferIndex = 0;
          sbusData[bufferIndex] = inData;
          sbusData[24] = 0xff; // load it with 0xFF as correct value will set it to 0.
          receiveState=RECEIVING;
          timer.startOneShot(TIMEOUT);// we have 5 msec to get the data....
        }
      }
      if (timer.isElapsed())
      {
        // we didnt receive anything within time so I guess its an error.
        status = ERROR;
        timer.startOneShot(50);// 50 msecs for anothe try
      }
      break;
    case RECEIVING: // we are reading the bytes
      while (port->available() > 0) // while is allowed as we read until it is empty
      {
        inData = port->read(); // read one byte
        bufferIndex ++;
        sbusData[bufferIndex] = inData;
        
        if (bufferIndex == 24) {
          receiveState = WAITINGFORSTART;
           timer.startOneShot(50); // we want to have something again within 50 msecs
          // check if the last byte = 0 , if thats the case its probably ok
          if (sbusData[24] == 0x00) {
            status = RECEIVED;
          }
          else 
          {
            status = ERROR;
          }
        }

      }
      if (timer.isElapsed())
      {
        // we have a time out, so somethings has gone wrong
        // also there's no more bytes in the buffer, so lets start over again.
        receiveState = WAITINGFORSTART;
        status = ERROR;
      }

      break;
    default:
      break;
  }

  return status;
}

