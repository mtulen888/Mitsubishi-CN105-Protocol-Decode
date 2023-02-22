#include <queue>

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
  struct Ecodanstatus
  {
    String value;
    String lastvalue;
    byte PacketType;
    byte Command;
    byte VarIndex;
    byte VarType;
    DecodeFunctions::DecodeFunction() char VarShortName[5];
    char VarLongName[40];
    char noSpacesName[40];
    enumStr
  };

  Ecodanstatus status[] = { … constructor…};

  Class DecodeFunctions
  {
    DecodeTemperature();
    DecodeEnums();
  }
}

class ECODANCOMMANDQUEUE
{
public:
  ECODANCOMMANDQUEUE();

  void loop();

  bool pushTxMsg(MessageStruct *msg);
  bool popRxMsg(MessageStruct *msg);

private:
  void ConnectSerial(Stream *HeatPumpStream);
  void Write_serial(); // looped
  void Read_Serial();  // looped

  Stream *DeviceStream;

  bool sending;
  queue<MessageStruct> TxQueue;
  queue<MessageStruct> RxQueue;

}

