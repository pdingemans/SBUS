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
#define port Serial
#define TIMEOUT 4// 10 msec to get a complete message
#define ALL_CHANNELS


class SBus
{
	public:
		enum SIGNAL_STATUS
		{
			OK,// =      0x00,
			LOST,//   =     0x01,
			FAILSAFE,// 		 =  0x03,
			NOCONNECTION,// 		= 0x04,
		};

		void begin(void);
		int16_t* getChannels();
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
		int16_t channels[18];
    	SIGNAL_STATUS  failsafe_status;
		int bufferIndex;
		
		Timer timer;
		
		

};

#endif
