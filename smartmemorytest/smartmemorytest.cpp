#include "SmartMemoryTest.h"




/*
 * brief: returns 0 if all pointers != 0 and 1 if one is NULL
*/

uint8_t u8_SMT_InitSmartMemoryTest( ts_SMT_ClassStruct* _pThis,     //!< zeiger auf das struct dass das zu bearbeitende objekt repraesentiert, also die attribute/variablen der klasse
                                    uint8_t *_pu8_Workmem,          //!< siehe Definition von ts_BIR_ClassStruct
                                    uint32_t _u32StartAdress,       //!< siehe Definition von ts_BIR_ClassStruct
                                    uint16_t _u16Blocksize,         //!< siehe Definition von ts_BIR_ClassStruct
                                    tfp_BR_ReadByte _fpReadByte,    //!< siehe Definition von ts_BIR_ClassStruct
                                    tfp_BR_WriteByte _fpWriteByte   //!< siehe Definition von ts_BIR_ClassStruct
                                    )
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
void v_SMT_PrepareMem(ts_SMT_ClassStruct* _pThis)
{
    for(uint16_t i = 0; i < _pThis->m_u16Blocksize; i++)
    {
        _pThis->m_pu8_Workmem[i] = _pThis->m_fpReadByte(i);
        _pThis->m_fpWriteByte(i,0);
    }
}

void v_SMT_RestoreMem(ts_SMT_ClassStruct* _pThis)
{
    for(uint16_t i = 0; i < _pThis->m_u16Blocksize; i++)
    {
        _pThis->m_fpWriteByte(i, _pThis->m_pu8_Workmem[i]);
    }
}
