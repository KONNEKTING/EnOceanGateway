/*
   EnOcean.h

    Created on: 08.03.2018
        Author: Jens
*/

#ifndef ENOCEAN_H_
#define ENOCEAN_H_

#include "Arduino.h"
#include "DebugUtil.h"

// important to change this to the number of devices defined.
#define MAX_NUMBER_OF_DEVICES     47

// Comment this to avoid debugging messages
//#define KDEBUG






// No need to change anything below this point



#define ENOCEAN_OK                 0
#define ENOCEAN_OUT_OF_RANGE      21
#define ENOCEAN_NOT_VALID_CHKSUM   7
#define ENOCEAN_NO_RX_TEL          6
#define ENOCEAN_NEW_RX_BYTE        3

struct RPS_TEL_DATA_TYPE
{
  uint8_t SA : 1; // 0: No 2nd action; 1: Valid second action
  uint8_t R2 : 3; // Rocker second action.
  uint8_t EB : 1; // 0: Released; 1: Pressed
  uint8_t R1 : 3; // Rocker first action. See
};

struct ONEBS_TEL_DATA_TYPE
{
  uint8_t State : 1; // 0: open 1: closed
};

struct VLD_TEL_MEASUREMENT_TYPE
{
  uint8_t type : 3; // NOT USED
  uint8_t dummy : 5; //
};

/*
  	Packet type
*/

#define u8RADIO_ERP1 0x01
#define u8RESPONSE   0x02

/*
  	EEP type
*/

// Receive
#define u8RORG_1BS 0xD5
#define u8RORG_RPS 0xF6
#define u8RORG_VLD 0xD2

// Send
#define u8RORG_COMMON_COMMAND 0x05
// COMANDS

#define u8CO_RD_IDBASE 0x08

#define RPS_BUTTON_CHANNEL_AI 0
#define RPS_BUTTON_CHANNEL_AO 1
#define RPS_BUTTON_CHANNEL_BI 2
#define RPS_BUTTON_CHANNEL_BO 3

#define RPS_BUTTON_2NDACT_NO    0
#define RPS_BUTTON_2NDACT_VALID 1

#define VLD_CMD_ID_01 0x01
#define VLD_CMD_ID_02 0x02
#define VLD_CMD_ID_03 0x03
#define VLD_CMD_ID_04 0x04
#define VLD_CMD_ID_05 0x05
#define VLD_CMD_ID_06 0x06

#define BASEID_BYTES 4
#define DATBUF_SZ 100

#define SER_SYNCH_CODE 0x55
#define SER_HEADER_NR_BYTES 4

struct VLD_D2_01_TELEGRAM_CMD_03_ACTS_TYPE
{
  uint8_t outputCH : 5; // Channel
  uint8_t notused : 3;  // NOT USED
};


struct VLD_D2_01_TELEGRAM_CMD_04_ACTRESP_TYPE
{
  uint16_t outputValue : 7; // in %
  uint16_t LC : 1; //
  uint16_t IOChannel : 5; //
  uint16_t EL : 2; //
  uint16_t OC : 1; // NOT USED
};

struct VLD_D2_04_00_TELEGRAM_CMD_SENRESP_TYPE
{
  uint8_t nouse : 4;   // not use
  uint8_t daynight : 1; // Day / Night
  uint8_t batt : 3;    // Battery autonomy
  

};

struct VLD_D2_01_TELEGRAM_CMD_03_TYPE
{
  VLD_D2_01_TELEGRAM_CMD_03_ACTS_TYPE u8VldTelActSta;
  uint8_t u8SenderId_p[4];
  uint8_t u8Status;
  uint8_t u8SubTelegramm;
  uint8_t u8DestinationId_p[4];
};

struct VLD_D2_01_TELEGRAM_CMD_04_TYPE
{
  VLD_D2_01_TELEGRAM_CMD_04_ACTRESP_TYPE u16VldTelActResp;
  uint8_t u8SenderId_p[4];
  uint8_t u8Status;
};

struct VLD_D2_04_00_TELEGRAM
{
  uint8_t voc;
  uint8_t hum;
  uint8_t temp;
  VLD_D2_04_00_TELEGRAM_CMD_SENRESP_TYPE u8VldTelSenSta;
  uint8_t u8SenderId_p[4];
  uint8_t u8Status;
};


struct RPS_TELEGRAM_TYPE
{
  RPS_TEL_DATA_TYPE u8RpsTelData;
  uint8_t u8SenderId_p[4];
  uint8_t u8Status;
};

struct ONEBS_TELEGRAM_TYPE
{
  ONEBS_TEL_DATA_TYPE u81bsTelData;
  uint8_t u8SenderId_p[4];
  uint8_t u8Status;
};

//! uart_getPacket state machine states.
enum STATES_GET_PACKET
{
  //! Waiting for the synchronisation byte 0x55
  GET_SYNC_STATE = 0,
  //! Copying the 4 after sync byte: raw data length (2 bytes), optional data length (1), type (1).
  GET_HEADER_STATE,
  //! Checking the header CRC8 checksum. Resynchronisation test is also done here
  CHECK_CRC8H_STATE,
  //! Copying the data and optional data bytes to the paquet buffer
  GET_DATA_STATE,
  //! Checking the info CRC8 checksum.
  CHECK_CRC8D_STATE,
};

//! Packet structure (ESP3)
struct PACKET_SERIAL_TYPE
{
  // Amount of raw data bytes to be received. The most significant byte is sent/received first
  uint16_t u16DataLength;
  // Amount of optional data bytes to be received
  uint8_t u8OptionLength;
  // Packetype code
  uint8_t u8Type;
  // Data buffer: raw data + optional bytes
  uint8_t *u8DataBuffer;
};

class IEnOceanDevice
{
  protected:
    uint8_t firstComObj = 0;
    uint8_t firstParameter = 0;
  public:
    uint8_t numberOfComObjects;
    uint8_t numberOfParameters;

    virtual ~IEnOceanDevice()
    {
    }
    virtual void init(uint8_t startAtComObj, uint8_t startAtParameter) = 0;
    virtual void task() = 0;
    virtual bool handleEnOceanPacket(PACKET_SERIAL_TYPE* f_Pkt_st) = 0;
    virtual void handleKnxEvents(byte index) = 0;
};

class EnOcean
{
    EnOcean();
    virtual ~EnOcean();

  public:
    static EnOcean Eno;

    void init();
    void task();

    bool sendPacket(PACKET_SERIAL_TYPE *pPacket);
    void obtainSenderId(uint8_t* senderId);
    void handleKnxEvents(byte index);

    void configureDevice(IEnOceanDevice &device);

    uint8_t* getBaseId();
  private:
    bool isInited;

    uint8_t u8CRC;
    uint8_t u8RxByte;
    uint8_t u8RetVal;

    uint8_t lastParam;
    uint8_t lastComObj;
    uint8_t lastDevice;
    uint8_t lastSenderIdOffset = 0;

    uint8_t u8datBuf[DATBUF_SZ];

    IEnOceanDevice* deviceRegistry[MAX_NUMBER_OF_DEVICES] { NULL };

    STATES_GET_PACKET u8State;
    PACKET_SERIAL_TYPE m_Pkt_st;

    static uint8_t u8CRC8Table[256];

    uint8_t lui8_BaseID_p[BASEID_BYTES];

    void readBaseId(uint8_t* fui8_BaseID_p);
    void getEnOceanMSG(uint8_t u8RetVal, PACKET_SERIAL_TYPE* f_Pkt_st);

    uint8_t uart_getPacket(PACKET_SERIAL_TYPE *pPacket, uint16_t u16BufferLength);
    uint8_t uart_sendPacket(PACKET_SERIAL_TYPE *pPacket);
};
extern EnOcean& enOcean;

#endif /* ENOCEAN_H_ */
