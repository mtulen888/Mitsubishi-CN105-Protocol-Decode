#include <queue>

class DecodeFunctions
{
  DecodeTemperature();
  DecodeEnums();
}

struct Ecodanstatus
{
  String value;
  String lastvalue;
  byte PacketType;
  byte Command;
  byte VarIndex;
  byte VarType;
  DecodeFunctions::DecodeFunction()
  char VarShortName[5];
  char VarLongName[40];
  char noSpacesName[40];
  //enumStr;
};

typedef struct _MessageStruct
{
    uint8_t SyncByte;
    uint8_t PacketType;
    uint8_t Preamble[PREAMBLESIZE];
    uint8_t PayloadSize;
    uint8_t Payload[MAXDATABLOCKSIZE];
    uint8_t Checksum;
} MessageStruct;

class ECODANENCODER
{
public:
  Loop(); // AutoRun (timer)
    // If(!connected) ConnectHeatpump();
    // Encode();
    // ECODANCOMMANDQUEUE::pushCommandMsg(MessageStruct *command);
  CommandProcessor(topic);
    // If(!connected) ConnectHeatpump();
    // Encode();
    // ECODANCOMMANDQUEUE::pushCommandMsg(MessageStruct *command);

private:
  void ConnectHeatpump();
  uint8_t HeatpumpConnected;

  struct commandItem
  {
    byte VarIndex;
    byte VarType;
    EncodeFunctions::EncodeFunction()
        byte SetType;
    char VarLongName[35];
    byte packetMask[packetBufferSize];
  };

  commandItem commandItems[] = { … constructor…};

  Encode(MessageStruct * command);
    // commandItems[MessageStruct.Command];
    // EncodeFunction();
    // ECODANCOMMANDQUEUE::pushCommandMsg(MessageStruct * command)

  class EncodeFunctions()
  EncodeTemperature
  EncodeEnums(…)

}

class ECODANDECODER
{
public:
  Loop(); // timer
    // ECODANCOMMANDQUEUE::PopReceiveMsg(MessageStruct *command)
    // Decode()
  
  GetEcodanstatus();

private:

  Ecodanstatus status[]; // { … constructor…};

}



class ECODANSERIALQUEUE
{
public:
  ECODANSERIALQUEUE(Stream *HeatPumpStream);

  void loop();

  bool pushTxMsg(MessageStruct *msg);
  bool popRxMsg(MessageStruct *msg);

private:
  void Connect();
  void Write_serial(); // looped
  void Read_Serial();  // looped

  Stream *DeviceStream;

  bool sending;
  queue<MessageStruct> TxQueue;
  queue<MessageStruct> RxQueue;

}

ECODANSERIALQUEUE::ECODANSERIALQUEUE(void)
{
  DeviceStream = HeatPumpStream;
  DeviceStream.begin(SERIAL_BAUD, SERIAL_CONFIG, D3, D4);
  pinMode(D3, INPUT_PULLUP);
}

void ECODANSERIALQUEUE::loop()
{
  if (!Connected)
  {
    Connect();
  }
  else 
  {
    Write_serial();
    Read_Serial();
  }
}

bool ECODANSERIALQUEUE::pushTxMsg(MessageStruct* msg)
{

}
bool ECODANSERIALQUEUE::popRxMsg(MessageStruct* msg)
{

}

bool ECODANSERIALQUEUE::Connect()
{
  // Write_serial()
  // if (data[1] == 0x7a){
  //   connectedToHP = true;
  // }
}


{
  bool foundStart = false;
  int dataSum = 0;
  byte checksum = 0;
  byte dataLength = 0;
  while (DeviceStream.available() > 0 && !foundStart)
  {
    data[0] = DeviceStream.read();
    if (data[0] == HEADER[0])
    {
      foundStart = true;
    }
  }
  if (!foundStart)
  {
    return RCVD_PKT_FAIL;
  }
  for (int i = 1; i < 5; i++) //read header
  {
    data[i] = DeviceStream.read();
  }
  dataLength = data[4] + 5;
  for (int i = 5; i < dataLength; i++)
  {
    data[i] = DeviceStream.read(); // read the payload data
  }
  data[dataLength] = DeviceStream.read();   // read checksum byte
  for (int i = 0; i < dataLength; i++) // sum up the header bytes...
  {
    dataSum += data[i];
  }
  checksum = (0xfc - dataSum) & 0xff; // calculate checksum
  if (data[dataLength] == checksum)
  { // we have a correct packet
    return RCVD_PKT_CONNECT_SUCCESS;
  }
  else
  {
    Telnet.println("CRC ERROR");
  }
  return RCVD_PKT_FAIL;
  // push Rx Msg
  sending = false;

}


// unsigned long sendCommandReadTime = 0; //set to millis value during send, allow to wait millis for answer
// Stream *DeviceStream;
// uint8_t Connected;
// bool sending;
// queue< MessageStruct > TxQueue;
// queue< MessageStruct > RxQueue;




