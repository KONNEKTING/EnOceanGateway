#ifndef EnOceanDevice_RPS_SIMPLE_H_
#define EnOceanDevice_RPS_SIMPLE_H_

#include "EnOcean.h"

class EnOceanDevice_RPS_simple: public IEnOceanDevice
{
	private:
		uint8_t tasterId_Arr[4] = { 0, 0, 0, 0 };
		boolean buttonState[6][2] = { { 0 } };
	public:
		uint32_t tasterId = 0;

		EnOceanDevice_RPS_simple()
		{
			// We have
			//  A, B, AB
			//  A_release, B_release, AB_release
			numberOfComObjects = 6;

			// We have only the Id of the push-button
			numberOfParameters = 1;
		}

		// pre-configure device, set sender-id etc.
		void init(uint8_t startAtComObj, uint8_t startAtParameter)
		{
			firstComObj = startAtComObj;
			firstParameter = startAtParameter;

			// we only store the first comObj as this information is sufficient to identify all the com-objects

			// regarding configuration:
			// in this case here we only have one configuration, which is the Enocean-ID of the button-set.
			tasterId = ((uint32_t) Konnekting.getUINT32Param(startAtParameter));
			tasterId_Arr[0] = tasterId >> 24;
			tasterId_Arr[1] = tasterId >> 16;
			tasterId_Arr[2] = tasterId >> 8;
			tasterId_Arr[3] = tasterId;
		}

		// things that need to be done regularly
		void task()
		{
			// nothing to be done regularly for the button-set that is listened to
			return;
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

		// decode enocean message. Fail fast!
		bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st)
		{
			// Get rid of messages we can't handle
			if (f_Pkt_st->u8DataBuffer[0] != u8RORG_RPS)
				return false;

			RPS_TELEGRAM_TYPE* lRpsTlg_p = (RPS_TELEGRAM_TYPE*) &(f_Pkt_st->u8DataBuffer[1]);

			// Get rid of messages not intended for us
			if (lRpsTlg_p->u8SenderId_p[0] != tasterId_Arr[0])
				return false;
			if (lRpsTlg_p->u8SenderId_p[1] != tasterId_Arr[1])
				return false;
			if (lRpsTlg_p->u8SenderId_p[2] != tasterId_Arr[2])
				return false;
			if (lRpsTlg_p->u8SenderId_p[3] != tasterId_Arr[3])
				return false;

			// Here we only have Telegrams that are intended for us

#ifdef KDEBUG
			Debug.println(F("detected: RPS %X"), tasterId);
#endif
			// Check welcher Taster gedr�ckt ist

			if (RPS_BUTTON_2NDACT_VALID == lRpsTlg_p->u8RpsTelData.SA)
			{
				switch (lRpsTlg_p->u8RpsTelData.R2) {
					case RPS_BUTTON_CHANNEL_AO:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button DO");
#endif
						break;
					case RPS_BUTTON_CHANNEL_AI:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button DI");
#endif
						break;
					case RPS_BUTTON_CHANNEL_BO:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button CO");
#endif
						if (1 == lRpsTlg_p->u8RpsTelData.EB) //Pressed
						{
							buttonState[2][0] = true;
							buttonState[2][1] = 0;
							Knx.write(firstComObj + 2, 0);
						}
						break;
					case RPS_BUTTON_CHANNEL_BI:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button CI");
#endif
						if (1 == lRpsTlg_p->u8RpsTelData.EB) //Pressed
						{
							buttonState[2][0] = true;
							buttonState[2][1] = 1;
							Knx.write(firstComObj + 2, 1);
						}
						break;
				}
			}
			else
			{
				switch (lRpsTlg_p->u8RpsTelData.R1) {
					case RPS_BUTTON_CHANNEL_AO:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button AO");
#endif
						if (1 == lRpsTlg_p->u8RpsTelData.EB) //ob pressed ist
						{
							buttonState[0][0] = true;
							buttonState[0][1] = 0;
							Knx.write(firstComObj, 0);
						}
						break;
					case RPS_BUTTON_CHANNEL_AI:

						if (0 == lRpsTlg_p->u8RpsTelData.EB)
						{
#ifdef KDEBUG
							DEBUGSERIAL.println("Released");
#endif
							for (int i = 0; i < 6; i++)
							{
								if (buttonState[i][0] == true)
								{
									buttonState[i][0] = false;
									bool state = buttonState[i][1];
									Knx.write(firstComObj + i + 3, state);
								}
							}

						}
						else
							if (1 == lRpsTlg_p->u8RpsTelData.EB) //ob pressed ist
							{
#ifdef KDEBUG
								DEBUGSERIAL.println("Button AI");
#endif
								buttonState[0][0] = true;
								buttonState[0][1] = 1;
								Knx.write(firstComObj, 1);
							}
						break;
					case RPS_BUTTON_CHANNEL_BO:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button BO");
#endif
						if (1 == lRpsTlg_p->u8RpsTelData.EB) //ob pressed ist
						{
							buttonState[1][0] = true;
							buttonState[1][1] = 0;
							Knx.write(firstComObj + 1, 0);
						}
						break;
					case RPS_BUTTON_CHANNEL_BI:
#ifdef KDEBUG
						DEBUGSERIAL.println("Button BI");
#endif
						if (1 == lRpsTlg_p->u8RpsTelData.EB) //ob pressed ist
						{
							buttonState[1][0] = true;
							buttonState[1][1] = 1;
							Knx.write(firstComObj + 1, 1);
						}
						break;
				} //ENDE Switch
			} //Ende Else 2nd Action
			return true;
		}
};

#endif /* EnOceanDevice_RPS_SIMPLE_H_ */
