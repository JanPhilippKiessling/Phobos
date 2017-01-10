/*
 *  File:   bitrunner.h
    Author: Jan Philipp Kiessling
    brief:  Fuer sicherheitskritische Anwendungen ist es noetig Speicher auf seine korrekte
            Taetigkeit zu pruefen.
            Hier wird ein einfacher Algorithmus dazu implementiert.
            Der Speicher wird mit einem schwarzen Hintergrund belegt (alles null),
            dann wird in das erste Bit eine 1 geschrieben. Anschliessend wird der gesamte
            Speicher ausgelesen und geprueft, ob alles wie erwartet ist.
            Im naechsten Schritt wird die 1 um eine Stelle weiter geschoben und
            der Vorgang wiederholt. Sobald ein Fehler erkannt wird, kehrt
            der Algorithmus mit !=0 zurueck

    Read/Writezyklen:
            Wenn ein EEPROM getestet wird, hier ist wie oft dieser Algo den speicher beschreibt:
            Am Anfang: jede Page ein mal, komplett mit null
            Zyklisch:
                Jedes Bit ein mal mit 1
                beim Bytewechsel das alte Byte noch mit 0
                    --> neun Schreibvorgaenge pro Byte
            Am Ende: jede Page ein mal, komplett mit dem Ausgangswert

            Also ist die Belastung jeder Page:
                BYTES_PRO_PAGE * 9 + 2

            Bei (BYTES_PRO_PAGE = 32) belastet ein kompletter Test jede Page also
            mit 290 Schreibvorgaengen.

    Achtung:
            Vor dem starten des Tests muessen die Interrupts ausgestellt werden!
            Ansonsten koennte waehrend des Tests jemand in den Speicher schreiben oder noch schlimmer daraus lesen
            Bzw andere Loesung: waehrend der Test laeuft in einen Schattenspeicher schreiben/lesen

    Zeitverbrauch:
            Dieser Test benoetigt:
                NUMBER_OF_PAGES*(BYTES_PRO_PAGE * 9 + 2) Schreibvorgaenge

                Lesen:
                 - Am Anfang einmal alles lesen --> (NUMBER_OF_PAGES*BYTES_PRO_PAGE)
                 - Beim Testen: 1 Bit schreiben, dann gesamten Speicher (NUMBER_OF_PAGES*BYTES_PRO_PAGE) lesen
                                wieder 1 Bit schreiben, dann gesamten Speicher (NUMBER_OF_PAGES*BYTES_PRO_PAGE) lesen
                             --> Anzahl Bits mal Anzahl Bytes
                             --> (NUMBER_OF_PAGES*BYTES_PRO_PAGE)*(NUMBER_OF_PAGES*BYTES_PRO_PAGE*8)
                Anzahl Gesamtlesevorgaenge:
                             (NUMBER_OF_PAGES*BYTES_PRO_PAGE)
                            +(NUMBER_OF_PAGES*BYTES_PRO_PAGE)*(NUMBER_OF_PAGES*BYTES_PRO_PAGE*8)

                Gesamte Anzahl Lese-/Schreibvorgaenge:
                             (NUMBER_OF_PAGES*BYTES_PRO_PAGE)
                            +(NUMBER_OF_PAGES*BYTES_PRO_PAGE)*(NUMBER_OF_PAGES*BYTES_PRO_PAGE*8)
                            + NUMBER_OF_PAGES*(BYTES_PRO_PAGE * 9 + 2)

            Beispiel:
                    8k EEPROM mit 256 Seiten zu je 32 Bytes, Lese- oder Schreibvorgang dauert 5 ms

                     256 * 32 = 8192
                     256*256*32*32*8 = 537 Millionen
                     256 * (32*9+2) = 75.000
                  --> 537 Millionen Vorgaenge a 5 ms
                  --> = 31 Tage (a 24 h)
                        ========
*/


#ifndef BITRUNNER_H
#define BITRUNNER_H

#include "stdint.h"
typedef uint8_t (*tfp_BR_ReadByte) (uint16_t const _u16ByteNr);
typedef void    (*tfp_BR_WriteByte)(uint16_t const _u16ByteNr, uint8_t const _u8Data);

typedef struct
{
    uint8_t m_b8InitDone;               //!< zeigt an, ob das objekt erfolgreich initialisiert wurde (1) oder nicht (0)
    tfp_BR_WriteByte m_fpWriteByte;     //!< mit dieser Funktion kann das Modul auf den zu pruefenden Speicher ein einzelnes Byte schreiben
    tfp_BR_ReadByte m_fpReadByte;       //!< mit dieser Funktion kann das Modul aus dem zu pruefenden Speicher ein einzelnes Byte lesen
    uint16_t m_u16Blocksize;            //<! Wie gross ist der zu pruefende Speicher? In Byte
    uint32_t m_u32Steps;                //!< hier speichert das Modul wieviele Steps es bereits durchgefuehrt hat
    uint16_t m_u16OldByteNr;            //!< Eine Hilfsvariable um herauszufinden, ob man gerade eine Bytegrenze ueberschritten hat
    uint8_t* m_pu8_Workmem;             //!< Hier wird der Inhalt des zu pruefenden Speichers gerettet, um ihn nach dem Test wieder herstellen zu koennen. Muss genauso gross sein, wie der zu preufende Speicher (vrgl. m_u16Blocksize)
}ts_BIR_ClassStruct;


uint8_t u8_BIR_InitBitrunner(
            ts_BIR_ClassStruct* _pThis,     //!< zeiger auf das struct dass das zu bearbeitende objekt repraesentiert, also die attribute/variablen der klasse
            uint8_t *_pu8_Workmem,          //!< siehe Definition von ts_BIR_ClassStruct
            uint16_t _u16Blocksize,         //!< siehe Definition von ts_BIR_ClassStruct
            tfp_BR_ReadByte _fpReadByte,    //!< siehe Definition von ts_BIR_ClassStruct
            tfp_BR_WriteByte _fpWriteByte   //!< siehe Definition von ts_BIR_ClassStruct
            );

uint8_t u8_BIR_Step(ts_BIR_ClassStruct* _pThis);
void v_BIR_SetNextStepNumber(ts_BIR_ClassStruct* _pThis, uint32_t _u32NextStep);
void v_BIR_PrepareMem(ts_BIR_ClassStruct* _pThis);
void v_BIR_RestoreMem(ts_BIR_ClassStruct* _pThis);


#endif // BITRUNNER_H
