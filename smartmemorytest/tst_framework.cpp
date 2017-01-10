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
    void TC_SmartMemoryTest();
    void TC_SmartMemoryTest_WholeMem_HappyPath();
    void TC_SmartMemoryTest_SafeAndRestoreMemory();
};


#define MEMSIZE 11
#define START_ADRESS 0

uint8_t a8MemToRun[MEMSIZE];

uint8_t u8ReadByte(uint16_t const _u16ByteNr)
{
    return a8MemToRun[_u16ByteNr];
}

static uint8_t mgu8_ManipulationValue   = 0;
static uint8_t mgb8_ByteNrToManipulate  = 0;
static uint8_t mgu8_ManipulateMemory    = 0;
static uint32_t mgu32_ManipulateAtStepNr  = 0;
static uint32_t u32StepCnt = 0;


void vWriteByte(uint16_t const _u16ByteNr, uint8_t const _u8Data)
{
    a8MemToRun[_u16ByteNr] = _u8Data;

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

void TST_Framework::TC_SmartMemoryTest()
{
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;
    uint8_t _pu8WorkMem[MEMSIZE];

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        u8++;
    }

    ts_SMT_ClassStruct sMySmartMemoryTest;

    // false init
    QVERIFY(u8_BIR_InitSmartMemoryTest(&sMySmartMemoryTest, 0, START_ADRESS, MEMSIZE, _fpReadByte, _fpWriteByte) == 1);

//
    // correct init
 //   QVERIFY(u8_BIR_InitSmartMemoryTest(&sMySmartMemoryTest, _pu8WorkMem, MEMSIZE, _fpReadByte, _fpWriteByte) == 0);

#if 0
    v_BIR_PrepareMem(&sMySmartMemoryTest);

    QVERIFY(a8MemToRun[0] == 0);
    QVERIFY(a8MemToRun[1] == 0);

    for(uint16_t i = 0; i < MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0);
    }

    u32StepCnt++;
    vManipulateMemory(10, 0x01);           // byte 10 kaputt machen
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);       //1
    QVERIFY(a8MemToRun[0] == 1);

    u32StepCnt++; 
    vManipulateMemory(10, 0x00);       // byte 10 wieder reparieren
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);      //2
    QVERIFY(a8MemToRun[0] == 2 );

    u32StepCnt++;    
    vManipulateMemory(5, 0x01);           // byte 5 kaputt machen
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);     //3
    QVERIFY(a8MemToRun[0] == 4 );

    a8MemToRun[5] = 0;           // byte 5 reparieren


    for(uint8_t i = 0; i < 5; i++)
    {
        u32StepCnt++;        
        QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //8
    }

    QVERIFY(a8MemToRun[0] == 0x80 );

    u32StepCnt++;    
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //9
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 1 );

    u32StepCnt++;

    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //10
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 2 );

    for(uint8_t i = 0; i < 6; i++)
    {
        u32StepCnt++;
        QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //16
    }
    QVERIFY(a8MemToRun[0] == 0 );
    Q7VERIFY(a8MemToRun[1] == 0x80 );

    u32StepCnt++;
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //17
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 1 );

    for(uint8_t i = 0; i < 7; i++)
    {
        u32StepCnt++;
        QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //24
    }

    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 0x80 );

    vManipulateMemory(0, 0x01);           // byte 0 kaputt machen
    u32StepCnt++;

    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);     //25
    QVERIFY(a8MemToRun[0] == 0x01 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 0 );
    QVERIFY(a8MemToRun[3] == 1 );

    vManipulateMemory(0, 0x00);           // byte 0 rep
    u32StepCnt++;

    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //26
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );

    vManipulateMemory(1, 0x01);           // byte 1 zerstören
    u32StepCnt++;

    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);     //27
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 1 );

    vManipulateMemory(1, 0x00);           // byte 1 rep
    u32StepCnt++;
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //28
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );

    vManipulateMemory(2, 0x01);           // byte 2 destroy
    u32StepCnt++;
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);     //29
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 1 );

    vManipulateMemory(2, 0x00);           // byte 2 rep
    u32StepCnt++;
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);     //30
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 0 );

    vManipulateMemory(3, 0x01);           // byte 3 destroy
    u32StepCnt++;
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 1);     //31
    QVERIFY(a8MemToRun[0] == 0 );
    QVERIFY(a8MemToRun[1] == 0 );
    QVERIFY(a8MemToRun[2] == 0 );
    QVERIFY(a8MemToRun[3] == 1);


    // restart at step 1
    v_BIR_SetNextStepNumber(&sMySmartMemoryTest, 1);
    QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);       //1
    QVERIFY(a8MemToRun[0] == 1);
#endif
}


/* a clean run over the whole memory with save and restore*/
void TST_Framework::TC_SmartMemoryTest_SafeAndRestoreMemory()
{
#if 0
    tfp_BR_ReadByte _fpReadByte   = &u8ReadByte;
    tfp_BR_WriteByte _fpWriteByte = &vWriteByte;
    uint8_t _pu8WorkMem[MEMSIZE];
    uint8_t a8CompareMem[MEMSIZE];

    u32StepCnt = 0;

    // setup semi random data
    uint8_t u8 = 0;
    for(uint16_t i = 0; i<MEMSIZE; i++)
    {
        a8MemToRun[i] = u8;
        a8CompareMem[i] = u8;
        u8++;
    }

    // construct and init the object
    ts_BIR_ClassStruct sMySmartMemoryTest;
    QVERIFY(u8_BIR_InitSmartMemoryTest(&sMySmartMemoryTest, _pu8WorkMem, MEMSIZE, _fpReadByte, _fpWriteByte) == 0);


    v_BIR_PrepareMem(&sMySmartMemoryTest);

    for(uint16_t i = 0; i < MEMSIZE; i++)
    {
        QVERIFY(a8MemToRun[i] == 0);
    }

    for(;u32StepCnt<MEMSIZE*8; u32StepCnt++)
    {
        QVERIFY(u8_BIR_Step(&sMySmartMemoryTest) == 0);
    }
    v_BIR_RestoreMem(&sMySmartMemoryTest);

    QVERIFY(a8MemToRun[0] == a8CompareMem[0]);
    QVERIFY(a8MemToRun[1] == a8CompareMem[1]);
    QVERIFY(a8MemToRun[2] == a8CompareMem[2]);
    QVERIFY(a8MemToRun[MEMSIZE-1] == a8CompareMem[MEMSIZE-1]);
#endif
}
QTEST_MAIN(TST_Framework)

#include "tst_framework.moc"
