/*
 * EnOceanDevice_RPS_sender.h
 *
 *  Created on: 10.03.2018
 *      Author: Jens
 */

#ifndef ENOCEANDEVICE_RPS_SENDER_H_
#define ENOCEANDEVICE_RPS_SENDER_H_

#include "EnOcean.h"

#define SIMULATE_NOTHING 0
#define SIMULATE_PUSH    1
#define SIMULATE_RELEASE 2
#define SIMULATE_PAUSE_BEFORE_RELEASE 100

class EnOceanDevice_RPS_sender: public IEnOceanDevice
{
	private:
		uint8_t lui8_SendeID_p[4];
		uint8_t buttonStateSimulation = SIMULATE_NOTHING;
		uint32_t buttonLastPushTime = 0;bool buttonMessage = false;

		void send_RPS_Taster(uint8_t* fui8_BaseID_p, boolean state, boolean pressed)
		{
			PACKET_SERIAL_TYPE l_TestPacket_st;
			uint8_t l_TestBuf_p[7];
			l_TestPacket_st.u16DataLength = 0x0007;
			l_TestPacket_st.u8OptionLength = 0x00;
			l_TestPacket_st.u8Type = u8RADIO_ERP1;
			l_TestPacket_st.u8DataBuffer = &l_TestBuf_p[0];

			l_TestBuf_p[0] = u8RORG_RPS;

			if (state == true)
			{
				if (pressed == true)
				{
					l_TestBuf_p[1] = 0x10;
					l_TestBuf_p[6] = 0x30;
				}
				else
				{
					l_TestBuf_p[1] = 0x00;
					l_TestBuf_p[6] = 0x20;
				}
			}
			else
			{
				if (pressed == true)
				{
					l_TestBuf_p[1] = 0x30;
					l_TestBuf_p[6] = 0x30;
				}
				else
				{
					l_TestBuf_p[1] = 0x00;
					l_TestBuf_p[6] = 0x20;
				}
			}

			for (int i = 0; i < 4; i++)
			{
				l_TestBuf_p[i + 2] = fui8_BaseID_p[i];
			}

			enOcean.sendPacket(&l_TestPacket_st);
		}

	public:
		EnOceanDevice_RPS_sender()
		{
			numberOfComObjects = 1;

			// We ignore the one parameter
			numberOfParameters = 1;
		}

		// pre-configure device, set sender-id etc.
		void init(uint8_t startAtComObj, uint8_t startAtParameter)
		{
			firstComObj = startAtComObj;
			firstParameter = startAtParameter;
			// we only store the first comObj as this information is sufficient to identify all the com-objects

			// regarding configuration:
			enOcean.obtainSenderId(&lui8_SendeID_p[0]);
		}

		// things that need to be done regularly
		void task()
		{
			if (buttonStateSimulation == SIMULATE_PUSH)
			{
				send_RPS_Taster(&lui8_SendeID_p[0], buttonMessage, true);
				buttonLastPushTime = millis();
				buttonStateSimulation = SIMULATE_RELEASE;
#ifdef KDEBUG
				Debug.println(F("Triggered %u Push"), firstComObj);
#endif
			}

			if (buttonStateSimulation == SIMULATE_RELEASE)
			{
				if (millis() - buttonLastPushTime >= SIMULATE_PAUSE_BEFORE_RELEASE)
				{
					send_RPS_Taster(&lui8_SendeID_p[0], buttonMessage, false);
					buttonStateSimulation = SIMULATE_NOTHING;
#ifdef KDEBUG
				Debug.println(F("Triggered %u Release"), firstComObj);
#endif
				}
			}

			// nothing to be done regularly for the button-set that is listened to
		}

		// something happened on the bus, let's react
		void handleKnxEvents(byte index)
		{
			if (index != firstComObj)
				return;

			if (Knx.read(index))
			{
				buttonStateSimulation = SIMULATE_PUSH;
				buttonMessage = true;
			}
			else
			{
				buttonStateSimulation = SIMULATE_PUSH;
				buttonMessage = false;
			}
#ifdef KDEBUG
			Debug.println(F("Triggered %u Prepare-Push"), firstComObj);
#endif
		}

		// decode EnOcean message. Fail fast!
#pragma GCC diagnostic push // I don't want a warning, just because we don't do anything here
#pragma GCC diagnostic ignored "-Wunused-parameter"
		bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st)
		{
			// nothing to do here, this module just sends
			return false;
		}
#pragma GCC diagnostic pop
};

#endif /* ENOCEANDEVICE_RPS_SENDER_H_ */
