/*
    Copyright (C) <2020>  <Mike Roberts>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "EcodanDecoder.h"

ECODANDECODER::ECODANDECODER(void)
{
  memset(&RxMessage, 0, sizeof(MessageStruct));
  memset(&Status, 0, sizeof(EcodanStatus));

  Preamble[0] = 0x02;
  Preamble[1] = 0x7a;
}


uint8_t ECODANDECODER::Process(uint8_t c)
{
  uint8_t ReturnValue = false;

  if (BuildRxMessage(&RxMessage , c))
  {
    ReturnValue = true;
    if (RxMessage.PacketType == GET_RESPONSE)
    {
      switch (RxMessage.Payload[0])
      {
        case 0x01 :
          Process0x01(RxMessage.Payload, &Status);
          break;
        case 0x04 :
          Process0x04(RxMessage.Payload, &Status);
          break;
        case 0x05 :
          Process0x05(RxMessage.Payload, &Status);
          break;
        case 0x07 :
          Process0x07(RxMessage.Payload, &Status);
          break;
        case 0x09 :
          Process0x09(RxMessage.Payload, &Status);
          break;
        case 0x0b :
          Process0x0B(RxMessage.Payload, &Status);
          break;
        case 0x0c :
          Process0x0C(RxMessage.Payload, &Status);
          break;
        case 0x0d :
          Process0x0D(RxMessage.Payload, &Status);
          break;
        case 0x0e :
          Process0x0E(RxMessage.Payload, &Status);
          break;
        case 0x13 :
          Process0x13(RxMessage.Payload, &Status);
          break;
        case 0x14 :
          Process0x14(RxMessage.Payload, &Status);
          break;
        case 0x26 :
          Process0x26(RxMessage.Payload, &Status);
          break;
        case 0x28 :
          Process0x28(RxMessage.Payload, &Status);
          break;
        case 0x29 :
          Process0x29(RxMessage.Payload, &Status);
          break;
        case 0xa1 :
          Process0xA1(RxMessage.Payload, &Status);
          break;
        case 0xa2 :
          Process0xA2(RxMessage.Payload, &Status);
          break;
      }
    }
  }
  return ReturnValue;
}

uint8_t ECODANDECODER::BuildRxMessage(MessageStruct *Message, uint8_t c)
{
  static uint8_t Buffer[COMMANDSIZE];
  static uint8_t BufferPos = 0;
  static uint8_t PayloadSize = 0;
  uint8_t i;

  if (BufferPos < HEADERSIZE)
  {
    switch (BufferPos)
    {
      case 0 :
        if ( c != PACKET_SYNC ) return false;
        break;

      case 1:
        switch (c)
        {
          case SET_REQUEST :
            break;
          case SET_RESPONSE :
            break;
          case GET_REQUEST :
            break;
          case GET_RESPONSE :
            break;
          case CONNECT_REQUEST:
            break;
          case CONNECT_RESPONSE:
            break;
          case EXCONNECT_REQUEST:
            break;
          case EXCONNECT_RESPONSE:
            break;
          default:
            //Serial.println("Unknown PacketType");
            BufferPos = 0;
            return false;  // Unknown Packet Type
        }
        break;

      case 2:
        if ( c != Preamble[0] )
        {
          //Serial.println("Preamble 1 Error");
          BufferPos = 0;
          return false;
        }
        break;

      case 3:
        if ( c != Preamble[1] )
        {
          //Serial.println("Preamble 1 Error");
          BufferPos = 0;
          return false;
        }
        break;

      case 4:
        PayloadSize = c;
        if (c > MAXDATABLOCKSIZE)
        {
          //Serial.println("Oversize Payload");
          BufferPos = 0;
          return false;
        }
        break;
    }

    Buffer[BufferPos] = c;
    BufferPos ++;
    return false;
  }
  else if (BufferPos < (PayloadSize + HEADERSIZE))
  {
    Buffer[BufferPos] = c;
    BufferPos++;
  }

  else if (BufferPos == (PayloadSize + HEADERSIZE))
  {
    Buffer[BufferPos] = c;
    BufferPos = 0;
    if (CheckSum(Buffer, PayloadSize + HEADERSIZE) == c)
    {
      //Serial.println("CS OK");
      Message->SyncByte = Buffer[0];
      Message->PacketType = Buffer[1];
      Message->Preamble[0] = Buffer[2];
      Message->Preamble[1] = Buffer[3];
      Message->PayloadSize = Buffer[4];
      Message->Checksum = c;
      memcpy(Message->Payload, &Buffer[5], Message->PayloadSize);
      return true;
    }
    else
    {
      //Serial.println("Checksum Fail");
      return false;
    }
  }
  return false;
}


void ECODANDECODER::Process0x01(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->DateTimeStamp.tm_year = Buffer[1];
  Status->DateTimeStamp.tm_mon = Buffer[2];
  Status->DateTimeStamp.tm_mday = Buffer[3];
  Status->DateTimeStamp.tm_hour = Buffer[4];
  Status->DateTimeStamp.tm_min = Buffer[5];
  Status->DateTimeStamp.tm_sec = Buffer[6];
}

void ECODANDECODER::Process0x02(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->Defrost = Buffer[3];
}

void ECODANDECODER::Process0x04(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->CompressorFrequency = Buffer[1];
}

void ECODANDECODER::Process0x05(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->HotWaterBoostActive = Buffer[7];
  Status->UnknownMSG5 = Buffer[5];
}
void ECODANDECODER::Process0x07(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->OutputPower = Buffer[6];
}

void ECODANDECODER::Process0x09(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->Zone1TemperatureSetpoint = ((float)ExtractUInt16(Buffer, 1) / 100);
  Status->Zone2TemperatureSetpoint = ((float)ExtractUInt16(Buffer, 3) / 100);
  Status->Zone1FlowTemperatureSetpoint = ((float)ExtractUInt16(Buffer, 5) / 100);
  Status->Zone2FlowTemperatureSetpoint = ((float)ExtractUInt16(Buffer, 7) / 100);
  Status->LegionellaSetpoint = ((float)ExtractUInt16(Buffer, 9) / 100);
  Status->HotWaterMaximumTempDrop = ((float)(Buffer[11] - 40)) / 2;
  Status->FlowTempMax = ((float)(Buffer[12] - 40)) / 2;
  Status->FlowTempMin = ((float)(Buffer[13] - 40)) / 2;
}

void ECODANDECODER::Process0x0B(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->Zone1Temperature = ((float)ExtractUInt16(Buffer, 1) / 100);
  Status->Zone2Temperature = ((float)ExtractUInt16(Buffer, 7) / 100);
  Status->OutsideTemperature = ((float)Buffer[11] / 2) - 40;
}

void ECODANDECODER::Process0x0C(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->HeaterOutputFlowTemperature = ((float)ExtractUInt16(Buffer, 1) / 100);
  Status->HeaterReturnFlowTemperature =   ((float)ExtractUInt16(Buffer, 4) / 100);
  Status->HotWaterTemperature = ((float)ExtractUInt16(Buffer, 7) / 100);
}

void ECODANDECODER::Process0x0D(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->ExternalBoilerFlowTemperature = ((float)ExtractUInt16(Buffer, 1) / 100);
  Status->ExternalBoilerReturnTemperature = ((float)ExtractUInt16(Buffer, 4) / 100);
}

void ECODANDECODER::Process0x0E(uint8_t * Buffer, EcodanStatus *Status)
{
  //Unknown = ((float)ExtractUInt16(Buffer, 1) / 100)
  //Unknown = ((float)ExtractUInt16(Buffer, 4) / 100)
  //Unknown = ((float)ExtractUInt16(Buffer, 7) / 100)
  //Unknown = ((float)ExtractUInt16(Buffer, 10) / 100)
}

void ECODANDECODER::Process0x13(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->RunHours = 100 * (Buffer[4] << 8 + Buffer[5]) + Buffer[3];
}


void ECODANDECODER::Process0x14(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->PrimaryFlowRate = Buffer[12];
}

void ECODANDECODER::Process0x26(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->SystemPowerMode = Buffer[3];
  Status->SystemOperationMode = Buffer[4];
  Status->HotWaterControlMode = Buffer[5];
  Status->HeatingControlMode = Buffer[6];
  Status->HotWaterSetpoint = ((float)ExtractUInt16(Buffer, 8) / 100);
  Status->HeaterFlowSetpoint = ((float)ExtractUInt16(Buffer, 10) / 100);
}

void ECODANDECODER::Process0x28(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->HotWaterTimerActive = Buffer[5];
  Status->HolidayModeActive = Buffer[4];
}

void ECODANDECODER::Process0x29(uint8_t * Buffer, EcodanStatus *Status)
{
  float fZone1, fZone2;

  fZone1 = ((float)ExtractUInt16(Buffer, 4) / 100);
  fZone2 = ((float)ExtractUInt16(Buffer, 6) / 100);
}

void ECODANDECODER::Process0xA1(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->ConsumedDateTimeStamp.tm_year = Buffer[1];
  Status->ConsumedDateTimeStamp.tm_mon = Buffer[2];
  Status->ConsumedDateTimeStamp.tm_mday = Buffer[3];

  Status->ConsumedHeatingEnergy = ExtractEnergy(Buffer, 4);
  Status->ConsumedCoolingEnergy = ExtractEnergy(Buffer, 7);
  Status->ConsumedHotWaterEnergy = ExtractEnergy(Buffer, 10);
}

void ECODANDECODER::Process0xA2(uint8_t * Buffer, EcodanStatus *Status)
{
  Status->DeliveredDateTimeStamp.tm_year = Buffer[1];
  Status->DeliveredDateTimeStamp.tm_mon = Buffer[2];
  Status->DeliveredDateTimeStamp.tm_mday = Buffer[3];

  Status->DeliveredHeatingEnergy = ExtractEnergy(Buffer, 4);
  Status->DeliveredCoolingEnergy = ExtractEnergy(Buffer, 7);
  Status->DeliveredHotWaterEnergy = ExtractEnergy(Buffer, 10);
}

float ECODANDECODER::ExtractEnergy(uint8_t *Buffer, uint8_t index)
{
  float Energy;

  Energy = (float) Buffer[index + 2];
  Energy = Energy / (float)100;
  Energy += (float)ExtractUInt16(Buffer, index);

  return Energy;
}

uint16_t ECODANDECODER::ExtractUInt16(uint8_t *Buffer, uint8_t Index)
{
  uint16_t Value;

  Value = (Buffer[Index] << 8) + Buffer[Index + 1];

  return Value;
}


void ECODANDECODER::CreateBlankTxMessage(uint8_t PacketType, uint8_t PayloadSize)
{
  CreateBlankMessageTemplate(&TxMessage, PacketType, PayloadSize);
}

void ECODANDECODER::CreateBlankMessageTemplate(MessageStruct *Message, uint8_t PacketType, uint8_t PayloadSize)
{
  uint8_t i;

  memset((void *)Message, 0, sizeof(MessageStruct));

  Message->SyncByte = PACKET_SYNC;
  Message->PacketType = PacketType;
  Message->PayloadSize = PayloadSize;
  for (i = 0; i < PREAMBLESIZE; i++)
  {
    Message->Preamble[i] = Preamble[i];
  }
}

void ECODANDECODER::SetPayloadByte(uint8_t Data, uint8_t Location)
{
  TxMessage.Payload[Location] = Data;
}

uint8_t ECODANDECODER::PrepareTxCommand(uint8_t *Buffer)
{
  return PrepareCommand(&TxMessage, Buffer);
}

uint8_t ECODANDECODER::PrepareCommand(MessageStruct *Message, uint8_t *Buffer)
{
  uint8_t MessageChecksum;
  uint8_t MessageSize;
  uint8_t i;

  Buffer[0] = Message->SyncByte;
  Buffer[1] = Message->PacketType;

  Buffer[2] = Message->Preamble[0];
  Buffer[3] = Message->Preamble[1];

  Buffer[4] = Message->PayloadSize;

  memcpy(&Buffer[5], Message->Payload, Message->PayloadSize);

  MessageSize = HEADERSIZE + Message->PayloadSize;

  MessageChecksum = CheckSum(Buffer, MessageSize);

  Buffer[MessageSize] = MessageChecksum;

  return MessageSize + 1;
}


uint8_t ECODANDECODER::CheckSum(uint8_t *Buffer, uint8_t len)
{
  uint8_t sum = 0;
  uint8_t i;

  for (i = 0; i < len; i++)
  {
    sum += Buffer[i];
  }

  sum = 0xfc - sum;
  sum = sum & 0xff;

  return sum;
}

void ECODANDECODER::EncodeSystemUpdate(uint8_t Flags, float Zone1TempSetpoint, float Zone2TempSetpoint,
                                       uint8_t Zones,
                                       float HotWaterSetpoint,
                                       uint8_t HeatingControlModeZ1, uint8_t HeatingControlModeZ2,
                                       uint8_t HotWaterMode, uint8_t Power)
{
  uint8_t UpperByte, LowerByte;
  uint16_t ScaledTarget;

  TxMessage.Payload[0] = TX_MESSAGE_SETTINGS;
  TxMessage.Payload[1] = Flags;

  TxMessage.Payload[2] = Zones;
  TxMessage.Payload[3] = Power;
  //TxMessage.Payload[4] = ???;


  if ( (Flags & SET_HOT_WATER_MODE) == SET_HOT_WATER_MODE)
  {
    TxMessage.Payload[5] = HotWaterMode;
  }

  if ( (Flags & SET_HEATING_CONTROL_MODE) == SET_HEATING_CONTROL_MODE)
  {
    TxMessage.Payload[6] = HeatingControlModeZ1;
    TxMessage.Payload[7] = HeatingControlModeZ2;
  }


  if ( (Flags & SET_HOT_WATER_SETPOINT) == SET_HOT_WATER_SETPOINT)
  {
    ScaledTarget = HotWaterSetpoint;
    ScaledTarget *= 100;
    UpperByte = (uint8_t)(ScaledTarget >> 8) ;
    LowerByte = (uint8_t)(ScaledTarget & 0x00ff);

    TxMessage.Payload[8] = UpperByte;
    TxMessage.Payload[9] = LowerByte;
  }

  if ( (Flags & SET_ZONE_SETPOINT) == SET_ZONE_SETPOINT)
  {
    ScaledTarget = Zone1TempSetpoint;
    ScaledTarget *= 100;
    UpperByte = (uint8_t)(ScaledTarget >> 8) ;
    LowerByte = (uint8_t)(ScaledTarget & 0x00ff);

    TxMessage.Payload[10] = UpperByte;
    TxMessage.Payload[11] = LowerByte;

    ScaledTarget = Zone2TempSetpoint;
    ScaledTarget *= 100;
    UpperByte = (uint8_t)(ScaledTarget >> 8) ;
    LowerByte = (uint8_t)(ScaledTarget & 0x00ff);

    TxMessage.Payload[12] = UpperByte;
    TxMessage.Payload[13] = LowerByte;
  }
}
