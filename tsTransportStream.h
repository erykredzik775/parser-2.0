#pragma once
#include "tsCommon.h"
#include <iomanip>
#include <iostream>
#include <fstream>
using namespace std;
/*
MPEG-TS packet:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |                             Header                            | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   4 |                  Adaptation field + Payload                   | `
`     |                                                               | `
` 184 |                                                               | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `


MPEG-TS packet header:
`        3                   2                   1                   0  `
`      1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0  `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `
`   0 |       SB      |E|S|T|           PID           |TSC|AFC|   CC  | `
`     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ `

Sync byte                    (SB ) :  8 bits
Transport error indicator    (E  ) :  1 bit
Payload unit start indicator (S  ) :  1 bit
Transport priority           (T  ) :  1 bit
Packet Identifier            (PID) : 13 bits
Transport scrambling control (TSC) :  2 bits
Adaptation field control     (AFC) :  2 bits
Continuity counter           (CC ) :  4 bits
*/


//=============================================================================================================================================================================

class xTS
{
public:
  static constexpr uint32_t TS_PacketLength = 188;
  static constexpr uint32_t TS_HeaderLength = 4;

  static constexpr uint32_t PES_HeaderLength = 6;

  static constexpr uint32_t BaseClockFrequency_Hz         =    90000; //Hz
  static constexpr uint32_t ExtendedClockFrequency_Hz     = 27000000; //Hz
  static constexpr uint32_t BaseClockFrequency_kHz        =       90; //kHz
  static constexpr uint32_t ExtendedClockFrequency_kHz    =    27000; //kHz
  static constexpr uint32_t BaseToExtendedClockMultiplier =      300;
};

//=============================================================================================================================================================================

class xTS_PacketHeader
{
public:
  enum class ePID : uint16_t
  {
    PAT  = 0x0000,
    CAT  = 0x0001,
    TSDT = 0x0002,
    IPMT = 0x0003,
    NIT  = 0x0010, //DVB specific PID
    SDT  = 0x0011, //DVB specific PID
    NuLL = 0x1FFF,
  };
  
protected:
  //TODO - header fields
  uint32_t SB; // 8 bitów
  uint32_t E; // 1 bit
  uint32_t S; // 1bit
  uint32_t TP; // 1bit
  uint32_t PID; // 13 bitów
  uint32_t TSC; // 2bity
  uint32_t AFC; // 2 bity
  uint32_t CC; //4 bity 

public:
  int32_t getAFC() const {return AFC;}
  int32_t getS()const{return S;}
  int32_t getPID()const {return PID;}
  int32_t getCC()const{return CC;}

  bool StartOfPES = false; 
  bool hasAF = false;
  void     Reset();
  int32_t  Parse(const uint8_t* Input);
  void     Print() const;

public:
  //TODO
  //bool     hasAdaptationField() const { /*TODO*/ }
  //bool     hasPayload        () const { /*TODO*/ }
};

//=============================================================================================================================================================================

class xTS_AdaptationField{
  protected:
    uint32_t AFL = 0;
    uint32_t DC;
    uint32_t RA;
    uint32_t SP;
    uint32_t PR;
    uint32_t OR;
    uint32_t SPF;
    uint32_t TP;
    uint32_t EX;   
  public:
  void Reset();
    int32_t getAF()const {return AFL;}
    void PrintAF()const ;
    int32_t ParseAF(const uint8_t* Input); 
};

class PES{
  protected:
  uint32_t PSC;
  uint32_t SID;
  uint16_t PES_PACKET_LENGTH;
  uint32_t PES_total_length;

  uint32_t Ten;
  uint32_t PES_S_C;
  uint32_t PES_P;
  uint32_t DATA_A_I;
  uint32_t copy;
  uint32_t O_C;
  uint32_t PTS_DTS_F;
  uint32_t ESCR_F;
  uint32_t ES_R_F;
  uint32_t DSM_TMF;
  uint32_t ACIF;
  uint32_t CRCF;
  uint32_t EF;

  
  uint8_t headerLength;
  uint32_t payload;
  public:
    PES(){};
    int32_t getPacketLength() const {return PES_PACKET_LENGTH;}
    int32_t getHeaderLength()const{return headerLength;}
    int32_t ParsePES(const uint8_t* Input);
    void PrintPES() const;
};

class PES_ASSEMBLER{
  public:
  enum class eResult : int32_t {
      UnexpectedPID = 1,
      StreamPackedLost ,
      AssemblingStarted ,
      AssemblingContinue,
      AssemblingFinished,
  };
protected:

int32_t m_PID;
uint8_t* m_Buffer;
uint32_t m_BufferSize;
uint32_t m_pesPayloadData;
int8_t m_LastContinuityCounter;
bool m_Started;
  
public:
void resetBufferSize() { m_BufferSize = 0;}
int32_t getBufferSize() {return m_BufferSize;}
PES mpes;
void PrintPesHeader(){mpes.PrintPES();}
PES_ASSEMBLER (){
      m_BufferSize = 65536;
      m_Buffer = new uint8_t[m_BufferSize];
      m_pesPayloadData = 0;
    }
~PES_ASSEMBLER (){};
void Init (int32_t PID);
eResult AbsorbPacket(const uint8_t* Input, const xTS_PacketHeader* PacketHeader, xTS_AdaptationField* AdaptationField);


};