#include <Arduino.h>
#include "SBUS.h"

void SBus::begin() {
  uint8_t loc_sbusData[25] = {
    0x0f, 0x01, 0x04, 0x20, 0x00, 0xff, 0x07, 0x40, 0x00, 0x02, 0x10, 0x80, 0x2c, 0x64, 0x21, 0x0b, 0x59, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80, 0x00, 0x00
  };
  int16_t loc_channels[18]  = {
    1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 0, 0
  };

  port.begin(BAUDRATE,SERIAL_8E1);

  memcpy(sbusData, loc_sbusData, 25);
  memcpy(channels, loc_channels, 18);

  failsafe_status = NOCONNECTION;
  bufferIndex = 0;
  feedState = 0;
  timer.startRepeat(TIMEOUT);
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

  if (Parse())
  {

    channels[0]  = ((sbusData[1] | sbusData[2] << 8) & 0x07FF);
    channels[1]  = ((sbusData[2] >> 3 | sbusData[3] << 5) & 0x07FF);
    channels[2]  = ((sbusData[3] >> 6 | sbusData[4] << 2 | sbusData[5] << 10) & 0x07FF);
    channels[3]  = ((sbusData[5] >> 1 | sbusData[6] << 7) & 0x07FF);
    channels[4]  = ((sbusData[6] >> 4 | sbusData[7] << 4) & 0x07FF);
    channels[5]  = ((sbusData[7] >> 7 | sbusData[8] << 1 | sbusData[9] << 9) & 0x07FF);
    channels[6]  = ((sbusData[9] >> 2 | sbusData[10] << 6) & 0x07FF);
    channels[7]  = ((sbusData[10] >> 5 | sbusData[11] << 3) & 0x07FF); // & the other 8 + 2 channels if you need them
#ifdef ALL_CHANNELS
    channels[8]  = ((sbusData[12] | sbusData[13] << 8) & 0x07FF);
    channels[9]  = ((sbusData[13] >> 3 | sbusData[14] << 5) & 0x07FF);
    channels[10] = ((sbusData[14] >> 6 | sbusData[15] << 2 | sbusData[16] << 10) & 0x07FF);
    channels[11] = ((sbusData[16] >> 1 | sbusData[17] << 7) & 0x07FF);
    channels[12] = ((sbusData[17] >> 4 | sbusData[18] << 4) & 0x07FF);
    channels[13] = ((sbusData[18] >> 7 | sbusData[19] << 1 | sbusData[20] << 9) & 0x07FF);
    channels[14] = ((sbusData[20] >> 2 | sbusData[21] << 6) & 0x07FF);
    channels[15] = ((sbusData[21] >> 5 | sbusData[22] << 3) & 0x07FF);
#endif

    // Failsafe
    if (sbusData[23] & (1 << 2)) {
       digitalWrite(13,!digitalRead(13)); 
      failsafe_status = LOST;
    }
    else if (sbusData[23] & (1 << 3)) {
      failsafe_status = FAILSAFE;
    }
    else failsafe_status = OK;
  }
  return failsafe_status;


}
bool SBus::Parse()
{
  bool toChannels = 0;

  switch (receiveState) 
  {
    case 0: // we are waiting for the first byte to arrive
      if (port.available() > 0) 
      {
        inData = port.read(); // read one byte
        if (inData != 0x0f) 
        {
          while (port.available() > 0) 
          { //read the contents of in buffer and discard it, whe are out of sync
            inData = port.read();
          }
        }
        else {
          bufferIndex = 0;
          inBuffer[bufferIndex] = inData;
          inBuffer[24] = 0xff;
          feedState = 1;
          timer.startOneShot(TIMEOUT);// we have 5 msec to get the data....
        }
      }
      break;
    case 1: // we are reading the bytes
      while (port.available() > 0) 
      {
        inData = port.read(); // read one byte
        bufferIndex ++;
        inBuffer[bufferIndex] = inData;
        
        if (bufferIndex == 24) {
          feedState = 0;
          // check if the last byte = 0 , if thats the case its probably ok
          if (inBuffer[0] == 0x0f && inBuffer[24] == 0x00) {
            memcpy(sbusData, inBuffer, 25);
            toChannels = 1;
          }
        }

      }
      if (timer.isElapsed())
      {
        // we have a time out, so somethings has gone wrong
        // also there's no more bytes in the buffer, so lets start over again.
        feedState = 0;
      }

      break;
  }

  return toChannels;
}

