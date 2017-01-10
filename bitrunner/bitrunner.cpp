#include "bitrunner.h"




/*
 * brief: returns 0 if all pointers != 0 and 1 if one is NULL
*/

uint8_t u8_BIR_InitBitrunner(    ts_BIR_ClassStruct* _pThis,
                            uint8_t* _pu8_Workmem,
                            uint16_t _u16Blocksize,
                            tfp_BR_ReadByte _fpReadByte,
                            tfp_BR_WriteByte _fpWriteByte)
{
    uint8_t u8RetVal = 1;
    if(    (_pThis           != 0)
        && (_fpWriteByte     != 0)
        && (_fpReadByte      != 0)
        && (_pu8_Workmem     != 0)
        )
    {
        _pThis->m_fpWriteByte = _fpWriteByte;
        _pThis->m_fpReadByte = _fpReadByte;
        _pThis->m_u16Blocksize = _u16Blocksize;
        _pThis->m_u32Steps = 0;
        _pThis->m_u16OldByteNr = 0;
        _pThis->m_pu8_Workmem = _pu8_Workmem;
        _pThis->m_b8InitDone = 1;
        u8RetVal = 0;
    }
    else
    {
        _pThis->m_fpWriteByte = 0;
        _pThis->m_fpReadByte = 0;
        _pThis->m_u16Blocksize = 0;
        _pThis->m_u32Steps = 0;
        _pThis->m_u16OldByteNr = 0;
        _pThis->m_pu8_Workmem = 0;
        _pThis->m_b8InitDone = 0;
    }
    return u8RetVal;
}

/*
 * brief: safes the memory to workmem and zeroes it afterwards
*/
void v_BIR_PrepareMem(ts_BIR_ClassStruct* _pThis)
{
    for(uint16_t i = 0; i < _pThis->m_u16Blocksize; i++)
    {
        _pThis->m_pu8_Workmem[i] = _pThis->m_fpReadByte(i);
        _pThis->m_fpWriteByte(i,0);
    }
}

void v_BIR_RestoreMem(ts_BIR_ClassStruct* _pThis)
{
    for(uint16_t i = 0; i < _pThis->m_u16Blocksize; i++)
    {
        _pThis->m_fpWriteByte(i, _pThis->m_pu8_Workmem[i]);
    }
}

uint8_t u8_BIR_Step(ts_BIR_ClassStruct* _pThis)
{
    uint16_t u16ByteNr = 0;
    uint8_t u8BitNr = 0;
    uint8_t u8RetVal = 0;

    _pThis->m_u32Steps += 1;

    u16ByteNr = (_pThis->m_u32Steps - 1) / 8;
    u8BitNr   = (_pThis->m_u32Steps - 1) - 8 * u16ByteNr;

    _pThis->m_fpWriteByte(u16ByteNr, 1 << u8BitNr);

    if(u16ByteNr != _pThis->m_u16OldByteNr)
    {
        _pThis->m_fpWriteByte(_pThis->m_u16OldByteNr, 0);
        _pThis->m_u16OldByteNr = u16ByteNr;
    }

    for(uint16_t u16ByteRunner = 0; u16ByteRunner < _pThis->m_u16Blocksize; u16ByteRunner++)
    {
        if(u16ByteRunner != u16ByteNr)
        {
            if(_pThis->m_fpReadByte(u16ByteRunner) != 0)
            {
                u8RetVal = 1;
            }
        }
        else
        {
            if(_pThis->m_fpReadByte(u16ByteRunner) !=  (1 << u8BitNr))
            {
                u8RetVal = 1;
            }
        }
    }

    return u8RetVal;

}

void v_BIR_SetNextStepNumber(ts_BIR_ClassStruct* _pThis, uint32_t _u32NextStep)
{
    _pThis->m_u32Steps = _u32NextStep - 1;
}
