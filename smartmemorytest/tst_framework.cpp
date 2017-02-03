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
    void TC_W0_and_BotToTop_R0W1();
    void TC_BotToTop_R1W0();
    void TC_TopToBot_R0W1();
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
static uint32_t mgu32StepCnt = 0; // starts with 1 (ie first write step)
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


// the algo runs through in one call. so if we want to inject to a specific byte while the algo runs, we need to manipulate the write or read function to do sth wrong.
// to get exactly the manipulation at the desired algo step we use a combination of a trigger-adress and step-counter. you probably want to reset the step counter every once in a while
// if we only used a trigger adress we couldnt manipulate the writing to byte 3 in the third "step" (remember: MCM is bit oriented)
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

void TST_Framework::TC_W0_and_BotToTop_R0W1()
{
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;

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

    QVERIFY(b8_MCM_Init(&sThis, MEMSIZE, _fpReadByte, _fpWriteByte ) == 0);

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

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);
    QVERIFY(a8MemToRun[0] == 0xFF);



    a8MemToRun[0] = 0x01; // place error in bit 0

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);

    // another run with another error
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    vManipulateMemory(  0, //!< _u32BytNr
                        3, //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        0
                       );

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);



    // another run with another error

    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  0, //!< _u32BytNr
                        5, //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        2 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);

    // another run with another error

    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  0, //!< _u32BytNr
                        5, //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        3 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);


    // another run with another error

    b8_MCM_Element_Any_W0(&sThis);

    mgu32StepCnt = 0;
    vManipulateMemory(  1, //!< _u32BytNr
                        5, //!<  _u8Value
                        0,//!< _u32TriggerAdress
                        0 //!< manip at write step nr
                       );

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 1);


}


void TST_Framework::TC_BotToTop_R1W0()
{

    // setup the test
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;

    ts_MCM_ClassStruct sThis;
    uint32_t u32_FailedAtByteNr = 0;

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        u8++;
    }

    // happy path
    QVERIFY(b8_MCM_Init(&sThis, MEMSIZE, _fpReadByte, _fpWriteByte ) == 0);
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0xFF);
    }

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);

    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0x00);
    }

    // put in an error in the byte 0
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  0, //!< _u32BytNr
                        3, //!< _u8Value
                        0, //!< _u32TriggerAdress
                        0   //!< manip at write step nr
                       );
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);


    // put in an error in the byte 1
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  1,     //!< _u32BytNr
                        0x3F,  //!< _u8Value
                        1,     //!< _u32TriggerAdress
                        15      //!< manip at write step nr
                       );
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 1);


    // put in an error in the byte at half way through
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  MEMSIZE/2,     //!< _u32BytNr
                        0x00,  //!< _u8Value
                        1,     //!< _u32TriggerAdress
                        0      //!< manip at write step nr, 0 for dont care
                       );
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE/2);


    // put in an error in the byte near to the end
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  MEMSIZE-5,     //!< _u32BytNr
                        0x02,  //!< _u8Value
                        1,     //!< _u32TriggerAdress
                        0      //!< manip at write step nr, 0 for dont care
                       );
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE-5);


    // put in an error in the byte at the end
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);

    mgu32StepCnt = 0;
    vManipulateMemory(  MEMSIZE-1,     //!< _u32BytNr
                        0x02,  //!< _u8Value
                        1,     //!< _u32TriggerAdress
                        0      //!< manip at write step nr, 0 for dont care
                       );
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE-1);
}



void TST_Framework::TC_TopToBot_R0W1()
{
    // setup the test
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;

    ts_MCM_ClassStruct sThis;
    uint32_t u32_FailedAtByteNr = 0;

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        u8++;
    }

    // happy path
    QVERIFY(b8_MCM_Init(&sThis, MEMSIZE, _fpReadByte, _fpWriteByte ) == 0);
    QVERIFY(b8_MCM_Element_Any_W0(&sThis) == 0);
    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR0W1, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0xFF);
    }

    QVERIFY(b8_MCM_March(&sThis, eBot2Top, eR1W0, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);

    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0x00);
    }

    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 0);
    QVERIFY(u32_FailedAtByteNr == 0);
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0xFF);
    }

    // put error in Byte 0
    // actually, to test R0W1 we dont need to run the whole MCM algo in front, it's enough when the class struct is inited and the memory is all 0
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[0] = 1;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 0);


    // put error in Byte 1
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[1] = 2;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == 1);

    // put error in Byte half way
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[MEMSIZE/2] = 4;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE/2);


    // put error in Byte close to the end
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[MEMSIZE-10] = 7;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE-10);

    // put error in the end, last bit
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[MEMSIZE-1] = 0x80;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE-1);

    // put error in the end, last bit-1
    for(uint32_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = 0x00;
    }
    a8MemToRun[MEMSIZE-1] = 0x40;
    QVERIFY(b8_MCM_March(&sThis, eTop2Bot, eR0W1, &u32_FailedAtByteNr) == 1);
    QVERIFY(u32_FailedAtByteNr == MEMSIZE-1);
}

#if 0
// viewcode

if(u8ByteCnt == 0)
{
cout << hex << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt+2)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt+1)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt))<< endl;
getchar();
}

if(u8ByteCnt == 1)
{
cout << hex << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt+1)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt+0)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt-1))<< endl;
getchar();
}

if(u8ByteCnt == 2)
{
cout << hex << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt+0)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt-1)) << (uint16_t)(_pThis->m_fpReadByte(u8ByteCnt-2))<< endl;
getchar();
}
#endif

QTEST_MAIN(TST_Framework)

#include "tst_framework.moc"
