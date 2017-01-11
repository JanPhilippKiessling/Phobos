#include "SmartMemoryTest.h"




/*
 * brief: returns 0 if all pointers != 0 and 1 if one is NULL
*/

uint8_t u8_SMT_InitSmartMemoryTest(
                                    ts_SMT_ClassStruct* _pThis,     //!< zeiger auf das struct dass das zu bearbeitende objekt repraesentiert, also die attribute/variablen der klasse
                                    uint8_t *_pu8CompleteMemBfr,          //!< siehe Definition von ts_BIR_ClassStruct
                                    uint32_t _u32MemSize,       //!< siehe Definition von ts_BIR_ClassStruct

                                    uint8_t *_pu8PageBfr,          //!< siehe Definition von ts_BIR_ClassStruct
                                    uint32_t _u32PageSize,       //!< siehe Definition von ts_BIR_ClassStruct

                                    tfp_BR_ReadBytes _fpReadByte,    //!< siehe Definition von ts_BIR_ClassStruct
                                    tfp_BR_WriteBytes _fpWriteByte   //!< siehe Definition von ts_BIR_ClassStruct
                                    )
{
    uint8_t u8RetVal = 0;

    if(     _pThis == 0
            || _pu8CompleteMemBfr == 0
            || _fpReadByte == 0
            || _pu8PageBfr == 0
            || _fpWriteByte == 0
            || _u32MemSize == 0
            || _u32PageSize == 0)
    {
        u8RetVal = 1;
    }
    else
    {
        _pThis->m_pu8CompleteMemBfr = _pu8CompleteMemBfr;
    }

    return u8RetVal;
}


