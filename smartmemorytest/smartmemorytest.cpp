#include "SmartMemoryTest.h"

#include <iostream>

using namespace std;

/*
 * brief: returns 0 if all pointers != 0 and 1 if one is NULL
*/

uint8_t b8_MCM_Init(ts_MCM_ClassStruct* _pThis,
                        uint32_t _u32MemSize,       //!< siehe Definition von ts_BIR_ClassStruct

                        tfp_BR_ReadByte _fpReadByte,    //!< siehe Definition von ts_BIR_ClassStruct
                        tfp_BR_WriteByte _fpWriteByte   //!< siehe Definition von ts_BIR_ClassStruct
                        )
{
    uint8_t u8RetVal = 0;

    if(     _pThis == 0
            || _u32MemSize == 0
            || _fpReadByte == 0
            || _fpWriteByte == 0)
    {
        u8RetVal = 1;
    }
    else
    {
        _pThis->m_u32MemSize = _u32MemSize;
        _pThis->m_fpReadByte = _fpReadByte;
        _pThis->m_fpWriteByte = _fpWriteByte;
    }


    return u8RetVal;
}


uint8_t b8_MCM_Element_Any_W0(ts_MCM_ClassStruct* _pThis)
{
    uint32_t i = 0;
    for(; i < _pThis->m_u32MemSize; i++)
    {
        _pThis->m_fpWriteByte(i,0);
    }
    return 0;
}


void vDebugOutput(uint16_t _u8)
{

}

uint8_t b8_MCM_Element_BotToTop_R0W1(ts_MCM_ClassStruct* _pThis, uint32_t* _pu32FailedAtByteNr)
{
    uint8_t u8RetVal = 0;
    uint8_t u8BitCnt = 0;
    uint8_t u8ByteCnt = 0;
    uint8_t u8CmpVal = 0x00;

    //for every byte
    for(u8ByteCnt = 0; (u8ByteCnt < _pThis->m_u32MemSize) && (u8RetVal == 0); u8ByteCnt++)
    {
        u8CmpVal = 0x00;
        //for every bit
        for(u8BitCnt = 0; (u8BitCnt < 8) && (u8RetVal == 0); u8BitCnt++)
        {
            if(_pThis->m_fpReadByte(u8ByteCnt) != u8CmpVal)
            {
                *_pu32FailedAtByteNr = u8ByteCnt;
                u8RetVal = 1;
            }
            u8CmpVal += 1 << u8BitCnt;
            _pThis->m_fpWriteByte(u8ByteCnt, u8CmpVal);
        }
    }

   return u8RetVal;
}

