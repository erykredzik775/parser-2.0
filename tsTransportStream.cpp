#include "tsTransportStream.h"
#include <iostream>


//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================

void xTS_PacketHeader::Reset()
{
    SB = 0;
    E = 0; 
    S = 0; 
    TP = 0;
    PID = 0;
    TSC =0;
    AFC =0;
    CC =0;
    hasAF = false;
}

int32_t xTS_PacketHeader::Parse(const uint8_t* Input)
{   
    uint32_t Tmp = *((uint32_t*) Input);
    Tmp = xSwapBytes32(Tmp);
    uint32_t masks[8] = {   0b11111111000000000000000000000000,
                            0b00000000100000000000000000000000,
                            0b00000000010000000000000000000000,
                            0b00000000001000000000000000000000,
                            0b00000000000111111111111100000000,
                            0b00000000000000000000000011000000,
                            0b00000000000000000000000000110000,
                            0b00000000000000000000000000001111};
    SB = Tmp & masks[0];
    SB = SB >> 24;

    E = Tmp & masks[1];
    E = E >> 23;

    S = Tmp & masks[2];
    S = S >> 22;

    TP = Tmp & masks[3];
    TP = TP >> 21;

    PID = Tmp & masks[4];
    PID = PID >> 8;

    TSC = Tmp & masks[5];
    TSC = TSC >> 6;

    AFC = Tmp & masks[6];
    AFC = AFC >> 4;

    CC = Tmp & masks[7];
    
    if(PID == 136 && (AFC == 2 || AFC == 3)) {
         hasAF = true;
    }
    if( S == 1 && PID == 136) StartOfPES = true;
    else StartOfPES = false;
return 0;
}

void xTS_PacketHeader::Print() const
{   
    printf("TF: \tSB=%d\tE=%d\tS=%d\tTP=%d\tPID=%d\tTSC=%d\tAFC=%d\tCC=%d", SB, E, S, TP, PID, TSC, AFC, CC);
}
//=============================================================================================================================================================================
//ADAPTATION FIELD //
void xTS_AdaptationField::Reset(){
    DC =0;
    AFL =0;
    RA =0;
    SP =0;
    PR =0;
    OR =0;
    SPF =0;
    TP =0;
    EX =0; 
}
int32_t xTS_AdaptationField::ParseAF(const uint8_t* Input){
     uint16_t Tmp = *((uint16_t*) Input);
    
    Tmp = xSwapBytes16(Tmp);
    uint16_t masks[9] = {   0b1111111100000000,
                            0b0000000010000000,
                            0b0000000001000000,
                            0b0000000000100000,
                            0b0000000000010000,
                            0b0000000000001000,
                            0b0000000000000100,
                            0b0000000000000010,
                            0b0000000000000001};
    AFL = Tmp & masks[0];
    AFL = AFL >> 8;

    DC = Tmp & masks[1];
    DC = DC >> 7;

    RA = Tmp & masks[2];
    RA = RA >> 6;

    SP = Tmp & masks[3];
    SP = SP >> 5;

    PR = Tmp & masks[4];
    PR = PR >> 4;

    OR = Tmp & masks[5];
    OR = OR >> 3;

    SPF = Tmp & masks[6];
    SPF = SPF >> 2;

    TP = Tmp & masks[7];
    TP = TP >> 1;

    EX = Tmp & masks[7];

    return 0;
}

void xTS_AdaptationField::PrintAF() const
{
    printf("\tAF: \tAFL=%d\tDC=%d\tRA=%d\tSP=%d\tPR=%d\tOR=%d\tSPF=%d\tTP=%d\tEX=%d", AFL, DC, RA, SP, PR, OR, SPF, TP, EX);
}
//========================

//==================
int32_t PES::ParsePES(const uint8_t* Input){
    uint32_t Tmp = *((uint32_t*) Input);    
    Tmp = xSwapBytes32(Tmp);
    uint32_t masks[2] = {   0b11111111111111111111111100000000,
                            0b00000000000000000000000011111111};
    PSC = Tmp & masks[0];
    PSC = PSC >> 8;

    SID = Tmp & masks[1];

    PES_PACKET_LENGTH = *(((uint16_t*) Input) + 2);
    PES_PACKET_LENGTH = xSwapBytes16(PES_PACKET_LENGTH);

    if(SID != 0b10111100
    && SID != 0b10111110
    && SID != 0b10111111 
    && SID != 0b11110000
    && SID != 0b11110010
    && SID != 0b11111111
    && SID != 0b11110010
    && SID != 0b11111000){
        // unsigned int mask = 0x11000000;
        // Ten = Input[startingPoint +6 ] & mask;
        // mask = 0x00110000;
        // Ten >>=6;
        // PES_S_C = Input[startingPoint + 6] & mask;
        // PES_S_C >>=4;

        // mask = 0x00001000;
        // PES_P = bool(Input[startingPoint + 6] & mask);
        // mask>>=1;
        // DATA_A_I = bool(Input[startingPoint + 6 ] & mask);
        // mask >>1;
        // copy = bool(Input[startingPoint]+ 6 & mask);
        // mask >>= 1;
        // O_C  = bool(Input[startingPoint + 6] & mask);
        // mask = 0xc0;
        // PTS_DTS_F = Input[startingPoint + 7] & mask;
        // PTS_DTS_F >>= 6;

        // mask = 0x20;
        // ESCR_F = bool(Input[startingPoint + 7] & mask);
        
        // mask = 0x10;
        // ES_R_F = bool(Input[startingPoint + 7 ] & mask);
        
        // mask = 0x08;
        // DSM_TMF = bool(Input[startingPoint + 7] & mask);

        // mask = 0x04;
        // ACIF = bool(Input[startingPoint + 7] & mask);

    
        // mask = 0x02;
        // CRCF = bool(Input[startingPoint + 7] & mask);

        // mask = 0x01;
        // EF = bool(Input[startingPoint + 7] & mask);
        
        headerLength = *(((uint8_t*) Input) + 8);
        headerLength += 6;
        //headerLength = xSwapBytes8(headerLength);
        
    }
    return 0;
}
void PES::PrintPES() const {
    printf(" PSCP=%d SID=%d L=%d HeaderL=%d",PSC, SID, PES_PACKET_LENGTH, headerLength);
}
PES_ASSEMBLER::eResult PES_ASSEMBLER::AbsorbPacket(const uint8_t* TransportStreamPacket, const xTS_PacketHeader* PacketHeader,   xTS_AdaptationField* AdaptationField){
    if(PacketHeader->getPID() !=  136){
        return eResult::UnexpectedPID;
    }
    else if(PacketHeader->getS() == 1){
        m_LastContinuityCounter = PacketHeader->getCC();
        m_Started = true;
        if(PacketHeader->hasAF == true){
            m_BufferSize = 188-4- 1  -  AdaptationField->getAF(); 
            for(int i=0;i<m_BufferSize;i++){
            m_Buffer [i] = TransportStreamPacket[ i + 5 + AdaptationField->getAF() ];
          }
        }
        else {
            m_BufferSize = 184;
            for(int i=0;i<m_BufferSize;i++){
            m_Buffer [i] = TransportStreamPacket[ i + 4];
          }
        }
        mpes.ParsePES(m_Buffer);
        m_pesPayloadData = m_BufferSize;
        return eResult::AssemblingStarted;
    }
    else if (m_Started){
        if(PacketHeader->getCC() == m_LastContinuityCounter+1 
            || (PacketHeader->getCC() == 0 
            && m_LastContinuityCounter == 15)){
            m_LastContinuityCounter = PacketHeader->getCC();
            if(m_pesPayloadData < mpes.getHeaderLength() + mpes.getPacketLength()){
                uint32_t tmplength = 0;
                    if(PacketHeader->hasAF == true){ 
                    tmplength = 188 - 4 - 1  -  AdaptationField->getAF(); 
                    for(int i=m_BufferSize;i<tmplength;i++){
                        m_Buffer [i] = TransportStreamPacket[ i + 5 + AdaptationField->getAF() ];
                }
                }
                else {
                    tmplength = 184; 
                    for(int i=m_BufferSize;i<tmplength;i++){
                        m_Buffer [i] = TransportStreamPacket[ i + 4];
                }
                }
                m_pesPayloadData +=  tmplength;
                printf("\tpesdataa=%d", m_pesPayloadData);
                printf("\theader=%d",  mpes.getHeaderLength() );
                if (m_pesPayloadData == mpes.getHeaderLength() + mpes.getPacketLength()){
                m_Started = false;
                FILE * PID = fopen("../PID136.mp2", "rb");
                fwrite(m_Buffer , 1, 1,PID);
                fclose(PID);
                delete m_Buffer;
                m_Buffer = new uint8_t[m_BufferSize];
                m_pesPayloadData = 0;
                printf("\tchuj=%d", m_pesPayloadData);
                return eResult::AssemblingFinished;
            }
                

                return eResult::AssemblingContinue;
            } 
            else {
                m_Started = false;
                m_pesPayloadData = 0;
                m_Buffer = new uint8_t[m_BufferSize];
                return eResult::StreamPackedLost;
            }   
        }
        
    
    }
    else {
            m_Started = false;
            m_pesPayloadData = 0;
            m_Buffer = new uint8_t[m_BufferSize];
            return eResult::StreamPackedLost;
        }
}
