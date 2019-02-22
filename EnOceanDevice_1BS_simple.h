/*
   EnOceanDevice_1BS_sender.h

    Created on: 17.09.2018
        Author: Matthias
*/

#ifndef ENOCEANDEVICE_1BS_SIMPLE_H_
#define ENOCEANDEVICE_1BS_SIMPLE_H_

#include "EnOcean.h"

// ################################################
// ### DEBUG CONFIGURATION
// ################################################
// DEBUG-MODE
//#define KDEBUG // comment this line to disable DEBUG mode

#define SIMULATE_NOTHING 0
#define SIMULATE_PUSH    1
#define SIMULATE_RELEASE 2
#define SIMULATE_PAUSE_BEFORE_RELEASE 100

class EnOceanDevice_1BS_simple: public IEnOceanDevice
{
  private:
    uint8_t sensorId_Arr[4] = { 0, 0, 0, 0 };
    uint32_t sensorId = 0;

    uint32_t nextPollTime = 0;
    uint32_t pollFrequencyMillis = (uint32_t)600000; // 10 minutes

    uint8_t getSensorId(uint8_t pos) {
      return sensorId_Arr[pos];
    }


  public:
    EnOceanDevice_1BS_simple()
    {
      // We have only state as object
      numberOfComObjects = 1;

      // We have only the Id of the push-button
      numberOfParameters = 2;
    }

    void getStatusSensor(uint8_t* mySenderId)
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
      lu8SndBuf[9] = getSensorId(0);
      lu8SndBuf[10] = getSensorId(1);
      lu8SndBuf[11] = getSensorId(2);
      lu8SndBuf[12] = getSensorId(3);
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
        Debug.println(F("Requested status from Fensterkontakt %X"), sensorId);
        Debug.print(F("To %X"), mySenderId[0]);
        Debug.print(F(" %X"), mySenderId[1]);
        Debug.print(F(" %X"), mySenderId[2]);
        Debug.println(F(" %X"), mySenderId[3]);
#endif
      }

      nextPollTime = millis() + pollFrequencyMillis;
    }

    // things that need to be done regularly
    void task()
    {

      if (millis() > nextPollTime) {
#ifdef KDEBUG
        Debug.println("get Status");
#endif
        getStatusSensor(enOcean.getBaseId());

      }
    }

    // pre-configure device, set sender-id etc.
    void init(uint8_t startAtComObj, uint8_t startAtParameter)
    {
      firstComObj = startAtComObj;
      firstParameter = startAtParameter;

      // we only store the first comObj as this information is sufficient to identify all the com-objects

      // regarding configuration:
      // in this case here we only have one configuration, which is the Enocean-ID of the button-set.
      sensorId = ((uint32_t) Konnekting.getUINT32Param(startAtParameter));
      sensorId_Arr[0] = sensorId >> 24;
      sensorId_Arr[1] = sensorId >> 16;
      sensorId_Arr[2] = sensorId >> 8;
      sensorId_Arr[3] = sensorId;

#ifdef KDEBUG
      Debug.print(F("I am: "));
      for (int i = 0; i < BASEID_BYTES; i++)
      {
        Debug.print(F("%X"), sensorId_Arr[i]);
      }
      Debug.println(F(""));
#endif
    }





    // decode EnOcean message. Fail fast!
#pragma GCC diagnostic push // I don't want a warning, just because we don't do anything here
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void handleKnxEvents(byte index)
    {
      // nothing to listen for from Knx
      return;
    }

    bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st)
    {
      if (f_Pkt_st->u8DataBuffer[0] != u8RORG_1BS)
        return false;

      // swap two bytes hier nicht notwendig, da nur ein Byte geschickt wird !!!
      //      uint8_t temp;
      //      temp = f_Pkt_st->u8DataBuffer[2];
      //      f_Pkt_st->u8DataBuffer[2] = f_Pkt_st->u8DataBuffer[3];
      //      f_Pkt_st->u8DataBuffer[3] = temp;

      //      VLD_D2_01_TELEGRAM_CMD_03_TYPE* ActStatResp = (VLD_D2_01_TELEGRAM_CMD_03_TYPE*) & (f_Pkt_st->u8DataBuffer[2]);
      //      RPS_TELEGRAM_TYPE* lRpsTlg_p = (RPS_TELEGRAM_TYPE*) &(f_Pkt_st->u8DataBuffer[1]);

      ONEBS_TELEGRAM_TYPE* l1bsTlg_p = (ONEBS_TELEGRAM_TYPE*) & (f_Pkt_st->u8DataBuffer[1]);

      // Get rid of messages not intended for us
      if (l1bsTlg_p->u8SenderId_p[0] != sensorId_Arr[0])
        return false;
      if (l1bsTlg_p->u8SenderId_p[1] != sensorId_Arr[1])
        return false;
      if (l1bsTlg_p->u8SenderId_p[2] != sensorId_Arr[2])
        return false;
      if (l1bsTlg_p->u8SenderId_p[3] != sensorId_Arr[3])
        return false;

      // Here we only have Telegrams that are intended for us

#ifdef KDEBUG
      Debug.println(F("detected: 1BS ID %X"), sensorId);
      Debug.println(F("detected: 1BS State: %X"), l1bsTlg_p->u81bsTelData.State);
#endif

      if (l1bsTlg_p->u81bsTelData.State == true)
      {
#ifdef KDEBUG
        Debug.println(F("Send KNX: 1BS Obj %d"), firstComObj);
#endif
        Knx.write(firstComObj, 1);
      }
      else if (!l1bsTlg_p->u81bsTelData.State)
      {
        Knx.write(firstComObj, 0);
      }
      else
      {
        // nothing to do here, this module just sends
        return false;
      }
    }
#pragma GCC diagnostic pop
};

#endif /* ENOCEANDEVICE_1BS_SENDER_H_ */
