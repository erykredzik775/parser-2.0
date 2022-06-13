#include "tsCommon.h"
#include "tsTransportStream.h"
#include <iostream>


int main(int argc, char *argv[ ], char *envp[ ])
{
  const int size = 188;
  
  // TODO - open file
  xTS_PacketHeader    TS_PacketHeader;
  xTS_AdaptationField TS_AdaptationField;
  PES_ASSEMBLER pes_assembler;
  PES PES_packet;
  uint8_t* bufor = new uint8_t[size];
  int counter=0;
  int sizePayload=184;
  int PesPayloadLength = 0;

  uint8_t* Payload = new uint8_t[sizePayload];
  //fstream output;
  //FILE* output = fopen("PID136.mp2", "wb");
  FILE * pFile = fopen( "../example_new.ts", "rb" );
  if( pFile == NULL) 
  {
    std::cout << "Error";
    return 0;
  }

  int32_t TS_PacketId = 0;
  while( !feof( pFile ) )
  {
    fread( bufor, sizeof( uint8_t ), size, pFile );

    TS_PacketHeader.Reset();
    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Parse(bufor);
    TS_PacketHeader.Print();
    TS_AdaptationField.Reset();
    if(TS_PacketHeader.hasAF){
      TS_AdaptationField.ParseAF(bufor+4);
      sizePayload = 188-4- 1 -TS_AdaptationField.getAF(); 
      TS_AdaptationField.PrintAF();
      for(int i=0;i<sizePayload;i++){
            Payload [i] = bufor[ i + 5 + TS_AdaptationField.getAF() ];
          }
    } 
    else {
      sizePayload = 184;
      for(int i=0;i<sizePayload;i++){            
            Payload [i] = bufor[i + 4];
          }
    }
     
  
    
    PES_ASSEMBLER::eResult Result = pes_assembler.AbsorbPacket(bufor,&TS_PacketHeader, &TS_AdaptationField);
    switch(Result){
      case PES_ASSEMBLER::eResult::StreamPackedLost : {
            printf("\tPacketLost");
            break;
          }
          case PES_ASSEMBLER::eResult::AssemblingStarted : {
            printf("\tStarted: ");
            pes_assembler.PrintPesHeader();
            break;
          }
          case PES_ASSEMBLER::eResult::AssemblingContinue: {
            printf("\tContinue");
            break;
          }
          case PES_ASSEMBLER::eResult::AssemblingFinished: {
            printf("\tFinished");
            break;
          }
          case PES_ASSEMBLER::eResult::UnexpectedPID:{
            printf("\tUnexpectedPID");
            break;
          }
          default: break; 
    }
    printf("\n");
    TS_PacketId++;
    counter++;
    if(counter==30) return 0;
  }
  fclose(pFile);
  //fclose(output);

  delete [] bufor;
  return 0;
}