#include <QtTest/QTest>
#include "smartmemorytest.h"
#include <iostream>


#define USE_SHOWMEM 0        // printing uses time, keep it off for standard runs
void vPrintMem(uint16_t NrOfBytes);



using namespace std;

class TST_Framework: public QObject
{
    Q_OBJECT
private slots:
    void TC_SmartMemoryTest_Init();
};


#define MEMSIZE 8192
#define PAGESIZE 32

uint8_t a8MemToRun[MEMSIZE];

void vReadBytes(uint32_t const _u32Adress, uint32_t const _u32Len,  uint8_t* const _pu8Data)
{
    uint32_t i = 0;
    for(uint32_t u32CurrentAdress = _u32Adress; u32CurrentAdress < (_u32Adress+_u32Len-1); u32CurrentAdress++)
    {
        _pu8Data[i] = a8MemToRun[u32CurrentAdress];
        i++;
    }
    return;
}

static uint8_t  mgu8_ManipulationValue    = 0;
static uint8_t  mgb8_ByteNrToManipulate   = 0;
static uint8_t  mgu8_ManipulateMemory     = 0;


void vWriteBytes(uint32_t const _u32Adress, uint32_t const _u32Len,  uint8_t* const _pu8Data)
{
    uint32_t i = 0;
    for(uint32_t u32CurrentAdress = _u32Adress; u32CurrentAdress < (_u32Adress+_u32Len-1); u32CurrentAdress++)
    {
        a8MemToRun[u32CurrentAdress] = _pu8Data[i];
        i++;
    }

    if(mgu8_ManipulateMemory)
    {
        a8MemToRun[mgb8_ByteNrToManipulate] = mgu8_ManipulationValue; //SmartMemoryTest needs to find that error
        mgu8_ManipulateMemory = 0;
    }
}

void vManipulateMemory(uint16_t _u16BytNr, uint8_t _u8Value)
{
    mgu8_ManipulationValue  = _u8Value;
    mgb8_ByteNrToManipulate = _u16BytNr;
    mgu8_ManipulateMemory   = 1;
}

void vPrintMem(uint16_t NrOfBytes)
{
#if USE_SHOWMEM // printing uses time, keep it off for standard runs
    for(int i = 0; i < NrOfBytes; i++)
    {
        uint8_t Temp = a8MemToRun[i];
        char show = 0;
        cout << "Byte " << i << ": ";
        for(int a = 0; a < 8; a++)
        {
            if( (Temp << a) & 0x80)
            {
                show = '1';
            }
            else
            {
                show = (char)' ';
            }
            cout << show;
        }
        cout <<endl;
    }
    cout << "----------------"<<endl;
#endif
}

void TST_Framework::TC_SmartMemoryTest_Init()
{
    tfp_BR_ReadBytes _fpReadByte   = &vReadBytes;
    tfp_BR_WriteBytes _fpWriteByte = &vWriteBytes;
    uint8_t _pu8CompleteMemBfr[MEMSIZE];
    uint8_t _pu8PageBfr[PAGESIZE];

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        u8++;
    }

    ts_SMT_ClassStruct sMySmartMemoryTest;

    /* false init */
    QVERIFY(u8_SMT_InitSmartMemoryTest( 0/*&sMySmartMemoryTest*/,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        _fpReadByte,
                                        _fpWriteByte) == 1);


    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        0/*_pu8CompleteMemBfr*/,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        _fpReadByte,
                                        _fpWriteByte) == 1);

    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        0/*_pu8PageBfr*/,
                                        PAGESIZE,
                                        _fpReadByte,
                                        _fpWriteByte) == 1);

    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        0/*_fpReadByte*/,
                                        _fpWriteByte) == 1);

    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        _fpReadByte,
                                        0/*_fpWriteByte*/) == 1);

    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        0/*MEMSIZE*/,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        _fpReadByte,
                                        _fpWriteByte) == 1);

    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        0/*PAGESIZE*/,
                                        _fpReadByte,
                                        _fpWriteByte) == 1);


    // correct init
    QVERIFY(u8_SMT_InitSmartMemoryTest( &sMySmartMemoryTest,
                                        _pu8CompleteMemBfr,
                                        MEMSIZE,
                                        _pu8PageBfr,
                                        PAGESIZE,
                                        _fpReadByte,
                                        _fpWriteByte) == 0);

    QVERIFY(sMySmartMemoryTest.m_pu8CompleteMemBfr == _pu8CompleteMemBfr);
    QVERIFY(sMySmartMemoryTest.m_u32MemSize = _u32MemSize);

}


QTEST_MAIN(TST_Framework)

#include "tst_framework.moc"
