#include "SmartMemoryTest.h"

#include <iostream>

using namespace std;


// internal helper functions, descreption see below
uint8_t u8_UpdateCmpVal(uint8_t _u8CmpVal, te_MarchElementAction _eAction, te_MarchElementDirection _eDir, uint8_t _u8BitCnt);
uint8_t u8_InitCmpVal(te_MarchElementAction _eAction);
uint8_t u8ReadByte_DirectionDependant(ts_MCM_ClassStruct* _pThis, uint32_t _u32ByteCnt, te_MarchElementDirection _eDir);
void vWriteByte_DirectionDependant(ts_MCM_ClassStruct* _pThis, uint32_t _u32ByteCnt, uint8_t _u8CmpVal, te_MarchElementDirection _eDir);
uint32_t u32ByteCnt_DirectionDependant(ts_MCM_ClassStruct* _pThis, te_MarchElementDirection _eDir, uint32_t _u32ByteCnt);
/*
 * brief: returns 0 if all pointers != 0 and 1 if one is NULL
*/

uint8_t b8_MCM_Init(    ts_MCM_ClassStruct* _pThis,
                        uint32_t _u32MemSize,           //!< siehe Definition von ts_BIR_ClassStruct
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


uint8_t b8_MCM_March(ts_MCM_ClassStruct* _pThis, te_MarchElementDirection _eDir, te_MarchElementAction _eAction, uint32_t* _pu32FailedAtByteNr)
{
    uint8_t u8RetVal = 0;
    uint8_t u8BitCnt = 0;
    uint8_t u8ReadVal = 0;
    uint32_t u32ByteCnt = 0;
    uint8_t u8CmpVal = 0x00;    // this is used to compare the freshly read value and then manipulated and written into the mem

    //for every byte
    for(u32ByteCnt = 0; (u32ByteCnt < _pThis->m_u32MemSize) && (u8RetVal == 0); u32ByteCnt++)
    {        
        u8CmpVal = u8_InitCmpVal(_eAction);

        //for every bit
        for(u8BitCnt = 0; (u8BitCnt < 8) && (u8RetVal == 0); u8BitCnt++)
        {
            u8ReadVal = u8ReadByte_DirectionDependant(_pThis, u32ByteCnt, _eDir);

            if(u8ReadVal != u8CmpVal)
            {
                *_pu32FailedAtByteNr = u32ByteCnt_DirectionDependant(_pThis, _eDir, u32ByteCnt);
                u8RetVal = 1;
            }

            u8CmpVal = u8_UpdateCmpVal(u8CmpVal, _eAction, _eDir, u8BitCnt);
            vWriteByte_DirectionDependant(_pThis, u32ByteCnt, u8CmpVal, _eDir);
        }
    }

   return u8RetVal;
}

// fuer den rueckgabewert muss man dann den ByteCnt fuer den Fall dass es von oben nach unten geht auch anpasen
uint32_t u32ByteCnt_DirectionDependant(ts_MCM_ClassStruct* _pThis, te_MarchElementDirection _eDir, uint32_t _u32ByteCnt)
{
    uint32_t u32RetVal = 0;

    if(_eDir == eBot2Top)
    {
        u32RetVal = _u32ByteCnt;
    }
    else
    {
        u32RetVal = (((_pThis->m_u32MemSize)-1)-_u32ByteCnt);
    }

    return u32RetVal;
}

// damit die for schleife in b8_MCM_Element() nicht rueckwaerts laufen muesste (dann brauchte man auch eine variable die in's negative laufen kann, um die stelle 0 noch zu erwischen) manipulieren wir lieber die schreib und lese adresse
uint8_t u8ReadByte_DirectionDependant(ts_MCM_ClassStruct* _pThis, uint32_t _u32ByteCnt, te_MarchElementDirection _eDir)
{
    uint8_t u8RetVal = 0;

    if(_eDir == eBot2Top)
    {
        u8RetVal = _pThis->m_fpReadByte(_u32ByteCnt);
    }
    else
    {
        u8RetVal = _pThis->m_fpReadByte(((_pThis->m_u32MemSize)-1)-_u32ByteCnt);
    }

    return u8RetVal;
}

// damit die for schleife in b8_MCM_Element() nicht rueckwaerts laufen muesste (dann brauchte man auch eine variable die in's negative laufen kann, um die stelle 0 noch zu erwischen) manipulieren wir lieber die schreib und lese adresse
void vWriteByte_DirectionDependant(ts_MCM_ClassStruct* _pThis, uint32_t _u32ByteCnt, uint8_t _u8CmpVal, te_MarchElementDirection _eDir)
{
    if(_eDir == eBot2Top)
    {
        _pThis->m_fpWriteByte(_u32ByteCnt, _u8CmpVal);
    }
    else
    {
        _pThis->m_fpWriteByte(((_pThis->m_u32MemSize)-1)-_u32ByteCnt, _u8CmpVal);
    }
    return;
}


/* this is a helper function that updates the compare (and write) value for b8_MCM_Element() depending on the active march element. It was created to increase readability */
uint8_t u8_UpdateCmpVal(uint8_t _u8CmpVal, te_MarchElementAction _eAction, te_MarchElementDirection _eDir, uint8_t _u8BitCnt)
{
    uint8_t u8_NewCmpVal = _u8CmpVal;

    if(_eAction == eR0W1)
    {
        if( _eDir == eBot2Top)
        {
            u8_NewCmpVal += 1 << _u8BitCnt;         //R0W1 Bot2Top
        }
        else      // eTop2Bot
        {
            u8_NewCmpVal += 1 << (7 - _u8BitCnt);   //R0W1 Top2Bot
        }
    }
    else        // eR1W0
    {
        if( _eDir == eBot2Top)
        {
            u8_NewCmpVal -= 1 << _u8BitCnt;         //R1W0 Bot2Top
        }
        else      // eTop2Bot
        {
            u8_NewCmpVal -= 1 << (7-_u8BitCnt);         //R1W0 Top2Bot
        }
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

uint8_t b8_MCM_Element_Any_R0(ts_MCM_ClassStruct* _pThis,  uint32_t* _pu32FailedAtByteNr)
{
    uint8_t u8RetVal = 0;
    uint32_t u32ByteCnt = 0;

    for(u32ByteCnt = 0; (u32ByteCnt < _pThis->m_u32MemSize) && (u8RetVal == 0); u32ByteCnt++)
    {
        if(_pThis->m_fpReadByte(u32ByteCnt) != 0)
        {
            u8RetVal = 1;
            *_pu32FailedAtByteNr = u32ByteCnt;
        }
    }

    return u8RetVal;
}
