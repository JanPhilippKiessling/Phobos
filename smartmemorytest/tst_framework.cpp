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
    void TC_MarchCMinus();
};


#define MEMSIZE 100

uint8_t a8MemToRun[MEMSIZE];

uint8_t u8ReadByte(uint32_t const _u32ByteNr)
{
    return a8MemToRun[_u32ByteNr];
}

static uint8_t mgu8_ManipulationValue   = 0;
static uint32_t mgu32_ByteNrToManipulate  = 0;
static uint8_t mgu8_ManipulateMemory    = 0;
static uint32_t mgu32_TriggerAdress     = 0;
static uint32_t mgu32StepCnt = 0;
static uint32_t mgu32_ManipAtStepNr = 0;


void vWriteByte(uint32_t const _u32ByteNr, uint8_t const _u8Data)
{
    mgu32StepCnt ++;
    a8MemToRun[_u32ByteNr] = _u8Data;

    if( mgu8_ManipulateMemory && (_u32ByteNr == mgu32_TriggerAdress) )
    {
        if( mgu32_ManipAtStepNr == 0 || mgu32StepCnt  == mgu32_ManipAtStepNr )
        {
            a8MemToRun[mgu32_ByteNrToManipulate] = mgu8_ManipulationValue; //SmartMemoryTest needs to find that error
            mgu8_ManipulateMemory = 0;
        }
    }
}

void vManipulateMemory(uint32_t _u32BytNr, uint8_t _u8Value, uint32_t _u32TriggerAdress, uint32_t _u32ManipAtStepNr)
{
    mgu8_ManipulationValue  = _u8Value;
    mgu32_ByteNrToManipulate = _u32BytNr;
    mgu32_TriggerAdress     = _u32TriggerAdress; //!< wenn jemand auf diese Byte Nummer schreibt, wird die Manipulatiion durhcgefuehrt
    mgu8_ManipulateMemory   = 1;
    mgu32_ManipAtStepNr = _u32ManipAtStepNr;
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

void TST_Framework::TC_MarchCMinus()
{
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;
    uint8_t _pu8Mem[MEMSIZE];

    ts_MCM_ClassStruct sThis;
    uint32_t u32_FailedAtByteNr = 0;

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        u8++;
    }

    QVERIFY(b8_MCM_Init(0, MEMSIZE, _fpReadByte, _fpWriteByte ) == 1);
    QVERIFY(b8_MCM_Init(&sThis, 0, _fpReadByte, _fpWriteByte ) == 1);
    QVERIFY(b8_MCM_Init(&sThis, MEMSIZE, 0, _fpWriteByte ) == 1);
    QVERIFY(b8_MCM_Init(&sThis, MEMSIZE, _fpReadByte, 0 ) == 1);

    QVERIFY(b8_MCM_Init(&sThis,  MEMSIZE, _fpReadByte, _fpWriteByte ) == 0);

    QVERIFY(sThis.m_u32MemSize == MEMSIZE);
    QVERIFY(sThis.m_fpReadByte == _fpReadByte);
    QVERIFY(sThis.m_fpWriteByte == _fpWriteByte);


    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(a8MemToRun[0] == 0);
    QVERIFY(a8MemToRun[1] == 0);
    QVERIFY(a8MemToRun[2] == 0);
    QVERIFY(a8MemToRun[3] == 0);
    QVERIFY(a8MemToRun[MEMSIZE-1] == 0);


    // happy path
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);
    QVERIFY(a8MemToRun[0] == 0xFF);



    a8MemToRun[0] = 0x01; // place error in bit 0

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);

    // another run with another error
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    vManipulateMemory(  0,  //!< _u32BytNr
                        3,  //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        0
                       );

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);



    // another run with another error

    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  0,  //!< _u32BytNr
                        5,  //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        2 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);

    // another run with another error

    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  0,  //!< _u32BytNr
                        5,  //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        3 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);




    // another run with another error

    b8_MCM_Element_Any_W0(&sThis);

    mgu32StepCnt = 0;
    vManipulateMemory(  1,  //!< _u32BytNr
                        5,  //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        0 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_Element_BotToTop_R0W1(&sThis, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 1);


}


QTEST_MAIN(TST_Framework)

#include "tst_framework.moc"
