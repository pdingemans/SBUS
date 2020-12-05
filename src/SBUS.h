#ifndef SBus_h
#define SBus_h

#include <Arduino.h>
#include "Timer.h"


#define BAUDRATE 100000
#define port Serial
#define TIMEOUT 5// 10 msec to get a complete message
#define ALL_CHANNELS


class SBus
{
	public:
		enum SIGNAL_STATUS
		{
		OK    =      0x00,
		LOST   =     0x01,
		FAILSAFE  =  0x03,
		NOCONNECTION = 0x04,
		RECEIVEINPROGRESS = 0x05
		};

		void begin(void);
		int16_t* getChannels();
		SIGNAL_STATUS getStatus(void);
		SIGNAL_STATUS UpdateChannels(void);
	
	private:
		bool Parse();
		uint8_t sbusData[25];
		int16_t channels[18];
    	SIGNAL_STATUS  failsafe_status;
		uint8_t byte_in_sbus;
		uint8_t bit_in_sbus;
		uint8_t ch;
		uint8_t bit_in_channel;
		uint8_t bit_in_servo;
		uint8_t inBuffer[25];
		int bufferIndex;
		uint8_t inData;
		int feedState;
		Timer timer;
		enum RECEIVER_STATE
		{
		WAITINGFORSTART    =      0x00,
		RECEIVING   =     0x01,
		} receiveState;
		

};

#endif
