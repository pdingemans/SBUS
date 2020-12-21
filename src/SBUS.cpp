#include "SBus.h"



void SBus::begin() {

  int16_t loc_channels[18]  = {
    1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 0, 0
  };

  port.begin(BAUDRATE,SERIAL_8E1);


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
    //     channels[0]  = ((sbusData[1] | sbusData[2] << 8) & 0x07FF);
    //     channels[1]  = ((sbusData[2] >> 3 | sbusData[3] << 5) & 0x07FF);
    //     channels[2]  = ((sbusData[3] >> 6 | sbusData[4] << 2 | sbusData[5] << 10) & 0x07FF);
    //     channels[3]  = ((sbusData[5] >> 1 | sbusData[6] << 7) & 0x07FF);
    //     channels[4]  = ((sbusData[6] >> 4 | sbusData[7] << 4) & 0x07FF);
    //     channels[5]  = ((sbusData[7] >> 7 | sbusData[8] << 1 | sbusData[9] << 9) & 0x07FF);
    //     channels[6]  = ((sbusData[9] >> 2 | sbusData[10] << 6) & 0x07FF);
    //     channels[7]  = ((sbusData[10] >> 5 | sbusData[11] << 3) & 0x07FF); // & the other 8 + 2 channels if you need them
    // #ifdef ALL_CHANNELS
    //     channels[8]  = ((sbusData[12] | sbusData[13] << 8) & 0x07FF);
    //     channels[9]  = ((sbusData[13] >> 3 | sbusData[14] << 5) & 0x07FF);
    //     channels[10] = ((sbusData[14] >> 6 | sbusData[15] << 2 | sbusData[16] << 10) & 0x07FF);
    //     channels[11] = ((sbusData[16] >> 1 | sbusData[17] << 7) & 0x07FF);
    //     channels[12] = ((sbusData[17] >> 4 | sbusData[18] << 4) & 0x07FF);
    //     channels[13] = ((sbusData[18] >> 7 | sbusData[19] << 1 | sbusData[20] << 9) & 0x07FF);
    //     channels[14] = ((sbusData[20] >> 2 | sbusData[21] << 6) & 0x07FF);
    //     channels[15] = ((sbusData[21] >> 5 | sbusData[22] << 3) & 0x07FF);
    // #endif
    
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
      if (port.available() > 0) 
      {
        inData = port.read(); // read one byte
        if (inData != 0x0f) // we will throw away any byte that is received that is not the start byte
        {
          while (port.available() > 0) 
          { //read the contents of in buffer and discard it, whe are out of sync
            inData = port.read();
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
      while (port.available() > 0) // while is allowed as we read until it is empty
      {
        inData = port.read(); // read one byte
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

