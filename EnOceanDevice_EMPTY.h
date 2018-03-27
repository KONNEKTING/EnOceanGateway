/*
 * EnOceanDevice_EMPTY.h
 *
 *  Created on: 09.03.2018
 *      Author: Jens
 */

#ifndef EnOceanDevice_EMPTY_H_
#define EnOceanDevice_EMPTY_H_

#include "EnOcean.h"

class EnOceanDevice_EMPTY: public IEnOceanDevice
{
	private:

	public:
		EnOceanDevice_EMPTY()
		{
			numberOfComObjects = 0;
			numberOfParameters = 0;
		}

		// pre-configure device, set sender-id etc.
		void init(uint8_t startAtComObj, uint8_t startAtParameter)
		{
			firstComObj = startAtComObj;
			firstParameter = startAtParameter;
			// we only store the first comObj as this information is sufficient to identify all the com-objects

			// regarding configuration:
		}

		// things that need to be done regularly
		void task()
		{
			// nothing to be done regularly for the button-set that is listened to
		}

		// something happened on the bus, let's react
#pragma GCC diagnostic push // If you don't want to be warned because you are not using index, include that pragma stuff
#pragma GCC diagnostic ignored "-Wunused-parameter"
		void handleKnxEvents(byte index)
		{
			// enter your KNX handling here
		}
#pragma GCC diagnostic pop // I don't want a warning, just because we don't do anything here




		// decode EnOcean message. Fail fast!
#pragma GCC diagnostic push // If you don't want to be warned because you are not using index, include that pragma stuff
#pragma GCC diagnostic ignored "-Wunused-parameter"
		bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st)
		{
			return false;
		}
#pragma GCC diagnostic pop // I don't want a warning, just because we don't do anything here

};

#endif /* EnOceanDevice_EMPTY_H_ */
