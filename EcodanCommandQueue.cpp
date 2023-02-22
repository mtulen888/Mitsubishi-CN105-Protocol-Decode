#include EcodanCommandQueue.h

ECODANCOMMANDQUEUE::ECODANCOMMANDQUEUE(void)
{
  ConnectSerial();
  Connect();
}
void ECODANCOMMANDQUEUE::ConnectSerial(Stream *HeatPumpStream)
{
  DeviceStream = HeatPumpStream;
  DeviceStream.begin(SERIAL_BAUD, SERIAL_CONFIG, D3, D4);
  pinMode(D3, INPUT_PULLUP);
}
bool ECODANCOMMANDQUEUE::Connect()
{
  uint8_t c;
  DeviceStream->write(Init3, 8);

  while (DeviceStream->available())
  {
    c = DeviceStream->read();

    if (ECODANDECODER::Process(c))
    {
      Connected = true;
    }
  }
}

void ECODANCOMMANDQUEUE::loop()
{
  if (!Connected)
  {
    Connect();
  } else {
    Write_serial();
    Read_Serial();
  }
}
bool ECODANCOMMANDQUEUE::pushTxMsg(MessageStruct* msg)
{

}
bool ECODANCOMMANDQUEUE::popRxMsg(MessageStruct* msg)
{

}
void ECODANCOMMANDQUEUE::Write_serial() // looped
{
	if (!sending)
  {
    sending = true; //simple semaphore to only allow one send command at a time, semaphore ends when answered data is received
    // pop Tx Msg
    byte chk = calcChecksum(command, length);
    int bytesSent = DeviceStream.write(command, length); //first send command
    bytesSent += Serial.write(chk); //then calculcated checksum byte afterwards

    sendCommandReadTime = millis(); //set sendCommandReadTime when to timeout the answer of this command
    return true;
  }
}
void ECODANCOMMANDQUEUE::Read_Serial() // looped
{
  if (sending && ((unsigned long)(millis() - sendCommandReadTime) > SERIALTIMEOUT)) {
    if (data_length == 0) {
      timeoutread++;
      totalreads++; //at at timeout we didn't receive anything but did expect it so need to increase this for the stats
    } else {
      tooshortread++;
    }
    sending = false; //receiving the answer from the send command timed out, so we are allowed to send a new command
  }



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
    if (data[1] == 0x7a){
      connectedToHP = true;
      Telnet.println("Connected successfully");
    }
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

  Stream *DeviceStream;

  // uint8_t Connected;
	// bool sending;
	// queue< MessageStruct > TxQueue;
	// queue< MessageStruct > RxQueue;
}
