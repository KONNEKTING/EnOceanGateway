#ifndef ENOCEANDEVICE_NODON_2CH_H_
#define ENOCEANDEVICE_NODON_2CH_H_

#include "EnOcean.h"

class EnOceanDevice_NodOn_2Ch: public IEnOceanDevice
{
	private:
		uint32_t actorId = 0;
		uint8_t actorId_Arr[4] = { 0, 0, 0, 0 };

		uint32_t nextPollTime = 0;
		uint32_t pollFrequencyMillis = (uint32_t)600000; // 10 minutes

    bool status_request = false;

	public:
		EnOceanDevice_NodOn_2Ch()
		{
			numberOfComObjects = 2;
			numberOfParameters = 2;
		}

		// pre-configure device, set sender-id etc.
		void init(uint8_t startAtComObj, uint8_t startAtParameter)
		{
			firstComObj = startAtComObj;
			firstParameter = startAtParameter;
			// we only store the first comObj as this information is sufficient to identify all the com-objects

			// regarding configuration:
			actorId = ((uint32_t) Konnekting.getUINT32Param(startAtParameter));
			actorId_Arr[0] = actorId >> 24;
			actorId_Arr[1] = actorId >> 16;
			actorId_Arr[2] = actorId >> 8;
			actorId_Arr[3] = actorId;

#ifdef KDEBUG
			Debug.print(F("I am: "));
			for (int i = 0; i < BASEID_BYTES; i++)
			{
				Debug.print(F("%X"), actorId_Arr[i]);
			}
			Debug.println(F(""));
#endif

			// ignore second config-item for the moment
		}

		void getStatusActors(uint8_t* mySenderId)
		{
			PACKET_SERIAL_TYPE lRdBaseIDPkt_st;

			uint8_t lu8SndBuf[15];

			lRdBaseIDPkt_st.u16DataLength = 0x0008;
			lRdBaseIDPkt_st.u8OptionLength = 0x07;
			lRdBaseIDPkt_st.u8Type = u8RADIO_ERP1;
			lRdBaseIDPkt_st.u8DataBuffer = &lu8SndBuf[0];

			lu8SndBuf[0] = u8RORG_VLD;
			lu8SndBuf[1] = VLD_CMD_ID_03;
			lu8SndBuf[2] = 0x1e;

			lu8SndBuf[3] = mySenderId[0];
			lu8SndBuf[4] = mySenderId[1];
			lu8SndBuf[5] = mySenderId[2];
			lu8SndBuf[6] = mySenderId[3];
			lu8SndBuf[7] = 0x00;
			// optional data
			lu8SndBuf[8] = 0x03;
			lu8SndBuf[9] = 0xFF;
			lu8SndBuf[10] = 0xFF;
			lu8SndBuf[11] = 0xFF;
			lu8SndBuf[12] = 0xFF;
			lu8SndBuf[13] = 0x00;
			lu8SndBuf[14] = 0x00;

			if (!enOcean.sendPacket(&lRdBaseIDPkt_st))
			{
#ifdef KDEBUG
				DEBUGSERIAL.println("Sending telegram failed.");
#endif
			}
			else
			{
#ifdef KDEBUG
				Debug.println(F("Requested status from NodOn %X"), actorId);
#endif
			}

			nextPollTime = millis() + pollFrequencyMillis;
		}

		// things that need to be done regularly
		void task()
		{
      if (status_request == false)
      {
        status_request = true;
        getStatusActors(enOcean.getBaseId());
      }
			if (millis() > nextPollTime)
			getStatusActors(enOcean.getBaseId());	
		}

		// something happened on the bus, let's react
#pragma GCC diagnostic push // I don't want a warning, just because we don't do anything here
#pragma GCC diagnostic ignored "-Wunused-parameter"
		void handleKnxEvents(byte index)
		{
			// nothing to listen for from Knx
			return;
		}
#pragma GCC diagnostic pop

		// decode EnOcean message. Fail fast!
		bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st)
		{
			if (f_Pkt_st->u8DataBuffer[0] != u8RORG_VLD)
				return false;

			if (f_Pkt_st->u8DataBuffer[1] != VLD_CMD_ID_04)
			{
#ifdef KDEBUG
				DEBUGSERIAL.println("Command ID not implemented yet.");
#endif
				return false;
			}

			{
				// swap two bytes
				uint8_t temp;
				temp = f_Pkt_st->u8DataBuffer[2];
				f_Pkt_st->u8DataBuffer[2] = f_Pkt_st->u8DataBuffer[3];
				f_Pkt_st->u8DataBuffer[3] = temp;
			}
			VLD_D2_01_TELEGRAM_CMD_04_TYPE* ActStatResp = (VLD_D2_01_TELEGRAM_CMD_04_TYPE*) &(f_Pkt_st->u8DataBuffer[2]);

			// Get rid of messages not intended for us
			if (ActStatResp->u8SenderId_p[0] != actorId_Arr[0])
				return false;
			if (ActStatResp->u8SenderId_p[1] != actorId_Arr[1])
				return false;
			if (ActStatResp->u8SenderId_p[2] != actorId_Arr[2])
				return false;
			if (ActStatResp->u8SenderId_p[3] != actorId_Arr[3])
				return false;

#ifdef KDEBUG
			DEBUGSERIAL.print("detect: Aktor_");
			DEBUGSERIAL.println(actorId);
#endif
			if (ActStatResp->u16VldTelActResp.IOChannel == 0) // Abfrage ob CH1
			{
				if (ActStatResp->u16VldTelActResp.outputValue == 0) // Abfrage ob CH1 = OFF
				{
					Knx.write(firstComObj, 0);
				}
				else
					if (ActStatResp->u16VldTelActResp.outputValue > 0)  // Abfrage ob CH1 = ON
					{
						Knx.write(firstComObj, 1);
					}
			}
			else
				if (ActStatResp->u16VldTelActResp.IOChannel == 1) // Abfrage ob CH2
				{
					if (ActStatResp->u16VldTelActResp.outputValue == 0) // Abfrage ob CH2 = OFF
					{
						Knx.write(firstComObj + 1, 0);
					}
					else
						if (ActStatResp->u16VldTelActResp.outputValue > 0) // Abfrage ob CH2 = ON
						{
							Knx.write(firstComObj + 1, 1);
						}
				}

			return false;
		}

};

#endif /* ENOCEANDEVICE_NODON_2CH_H_ */
