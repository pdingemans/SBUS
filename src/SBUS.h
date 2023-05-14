  /*
    arduino sbus decoder
    Copyright (C) 2020  P.Dingemans

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef SBus_h
#define SBus_h

#include <Arduino.h>
#include "Timer.h"


#define BAUDRATE 100000
//#define port Serial

#define ALL_CHANNELS

#define SBUS_STARTCHAR 0x0f
#define SBUS_ENDCHAR 0x0
#define SBUSTIMEOUT 5000 //usecs=5msec to get a complete message


class SBus
{
	public:
		SBus(HardwareSerial& serialport);
		enum SIGNAL_STATUS
		{
			OK,// =      0x00,
			LOST,//   =     0x01,
			FAILSAFE,// 		 =  0x03,
			NOCONNECTION,// 		= 0x04,
		};
		void begin();	
		uint16_t* getChannels();
		uint16_t getChannelValue (uint8_t nr);
		SIGNAL_STATUS getStatus(void);
		SIGNAL_STATUS UpdateChannels(void);
	
	private:
		enum RECEIVER_STATE
		{
			WAITINGFORSTART,//   =      0x00,
			RECEIVING,//   =     0x01,
			RECEIVED,
			ERROR
		} receiveState;
		RECEIVER_STATE Parse();
		uint8_t sbusData[25];
		uint16_t channels[18];
    	SIGNAL_STATUS  failsafe_status;
		int bufferIndex;
		HardwareSerial* port;
		Timer timer;
		
		

};

#endif
