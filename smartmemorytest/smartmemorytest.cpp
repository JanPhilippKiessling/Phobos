#include "SmartMemoryTest.h"

#include <iostream>

using namespace std;


uint8_t u8_UpdateCmpVal(uint8_t _u8CmpVal, te_MarchElementAction _eAction, uint8_t _u8BitCnt);
uint8_t u8_InitCmpVal(te_MarchElementAction _eAction);

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


uint8_t b8_MCM_Element_BotToTop(ts_MCM_ClassStruct* _pThis, te_MarchElementAction _eAction, uint32_t* _pu32FailedAtByteNr)
{
    uint8_t u8RetVal = 0;
    uint8_t u8BitCnt = 0;
    uint8_t u8ByteCnt = 0;
    uint8_t u8CmpVal = 0x00;    // this is used to compare the freshly read value and then manipulated and written into the mem

    //for every byte
    for(u8ByteCnt = 0; (u8ByteCnt < _pThis->m_u32MemSize) && (u8RetVal == 0); u8ByteCnt++)
    {        
        u8CmpVal = u8_InitCmpVal(_eAction);

        //for every bit
        for(u8BitCnt = 0; (u8BitCnt < 8) && (u8RetVal == 0); u8BitCnt++)
        {
            if(_pThis->m_fpReadByte(u8ByteCnt) != u8CmpVal)
            {
                *_pu32FailedAtByteNr = u8ByteCnt;
                u8RetVal = 1;
            }

            u8CmpVal = u8_UpdateCmpVal(u8CmpVal, _eAction, u8BitCnt);

            _pThis->m_fpWriteByte(u8ByteCnt, u8CmpVal);
        }
    }

   return u8RetVal;
}


/* this is a helper function that updates the compare (and write) value for b8_MCM_Element() depending on the active march element. It was created to increase readability */

uint8_t u8_UpdateCmpVal(uint8_t _u8CmpVal, te_MarchElementAction _eAction, uint8_t _u8BitCnt)
{
    uint8_t u8_NewCmpVal = _u8CmpVal;

    if(_eAction == eR0W1)
    {
        u8_NewCmpVal += 1 << _u8BitCnt;          //R0W1 BotToTop
    }
    else       // eR1W0
    {
        u8_NewCmpVal -= 1 << _u8BitCnt;          //R1W0 BotToTop
    }

    return u8_NewCmpVal;
}
/* this is a helper function that (re)inits the compare (and write) value for b8_MCM_Element() depending on the active march element. It was created to increase readability */
uint8_t u8_InitCmpVal(te_MarchElementAction _eAction)
{
    uint8_t u8_NewCmpVal = 0;

    if(_eAction == eR0W1)
    {
        u8_NewCmpVal = 0x00;    //R0W1 BotToTop
    }
    else       // eR1W0
    {
        u8_NewCmpVal = 0xFF;    //R1W0 BotToTop
    }

    return u8_NewCmpVal;
}
