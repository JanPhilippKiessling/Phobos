/*
 *  File:   smartmemorytest.h
    Author: Jan Philipp Kiessling
    brief:  Fuer sicherheitskritische Anwendungen ist es noetig Speicher auf seine korrekte
            Taetigkeit zu pruefen.

            Einfachste Speichertestalgorithmen können bis zu 31 Tage
            fuer 8k EEPROM Speicher benoetigen. Diese Zeiten koennen nicht in
            Kauf genommen werden, deshalb erfolgt hier ein kluegerer Test,
            der den physikalischen Aufbau von Speicher in Betracht zieht.

            Speicherfehler sind beispielsweise:
                - Schreiben in eine Speicherzelle verändert den Inhalt einer anderen Zelle
                - Lesen von einer Speicherzelle verändert den Inhalt einer anderen Zelle
                - 0 Schreiben in eine Speicherzelle hat keinen Effekt
                - 1 Schreiben in eine Speicherzelle hat keinen Effekt
                - 1 Lesen aus einer Speicherzelle ergibt 0
                - 0 Lesen aus einer Speicherzelle ergibt 1
                - 1 Lesen aus einer Speicherzelle ergibt 1, danach ist der Inhalt der Zelle aber 0
                - 0 Lesen aus einer Speicherzelle ergibt 0, danach ist der Inhalt der Zelle aber 1



                Betrachtung der theoretisch moeglichen Fehler
                *********************************************
                Bei Speichertests gibt es immer das Problem der Laufzeit. Primitive Algorithmen koennen beispielsweise
                bei einem 8 k EEPROM leicht 31 Tage Durchlaufzeit benoetigen. Wenn ein Test so lang benoetigt bietet er
                aber keinen Schutz, denn wenn es 31 Tage dauert um ein Geraet zu startet, wird dieses Geraet nicht verwendet
                werden.
                Es muessen also Algorithmen gefunden werden die in brauchbarer Zeit durchlaufen (< 2 s) und die trotzdem noch
                genuegend viele Fehler aufdecken. Ueblicherweise koennen schnelle Tests weniger Fehler aufdecken und umgekehrt.

                Diese Betrachtung dient dazu, die Guete von Testalgorithmen zu bewerten.

                Primitive Fehler mit Opferzellen
                ********************************
                Es werden zwei Speicherzellen betrachtet: Eine Taeterzelle und eine Opferzelle. Auf der
                Taeterzelle wird eine einzelne Speicheroperation ausgefuert. Diese Operation kann fehlerhafte Ergebnisse
                liefern und ausserem den Inhalt der Taeterzelle und/oder den Inhalt der Opferzeller zerstoeren.

                Bei dieser Betrachtung gibt es eine begrenzte Menge Operationen, die auf eine Speicherzelle ausgefuert werden
                koennen. Deshalb ist es moeglich hier alle theoretisch moeglichen Fehler darzustellen. Ob dies physikalisch
                im Chip praktisch moeglich ist wird an dieser Stelle nicht betrachtet.

                Moegliche Aktionen auf eine Zelle sind: Eine Zelle lesen, eine Zelle mit 1 beschreiben, eine Zelle mit 0
                beschreiben. Dabei kann der Speicher vor der Aktion entweder 0 oder 1 sein.

                Man erhaelt also folgende moegliche Operationen

                (Speicherzustand -> Aktion)
                0 -> L
                1 -> L
                0 -> 1 S
                1 -> 0 S
                0 -> 0 S
                1 -> 1 S

                Diese Aktionen koennen nun Ergebnisse haben, manche dieser Ergebnisse sind korrekt, andere sind fehlerhaft.
                Beim Schreiben hat sich der Speicher veraendert, oder nicht. Beim Lesen ist das theoretisch ebenfalls
                moeglich, ausserdem gibt es noch einen Rueckgabewert, naemlich das Leseergbnis. Des weiteren koennte der
                Inhalt einer Opferzelle zerstoert worden sein, oder nicht.

                (Speicherzustand -> Aktion -> Resultat (Neuer Zustand der Zelle, Der Inhalt einer zufälligen anderen Zelle wird zerstoert (1) oder nicht (0), Ergebnis des Lesevorgangs (entfaellt beim schreiben)) )

                Alle moeglichen Kombinationen aus Speicherzustand, Aktion und Resultat sind also:
                F0) 0 -> L -> (0, 0, 0)         // 0 Lesen erfolgreich
                F1) 0 -> L -> (0, 0, 1)         // gutartiger Lesefehler 0
                F2) 0 -> L -> (0, 1, 0)         // 0 Lesen erfolgreich mit zerstoerter Opferzelle
                F3) 0 -> L -> (0, 1, 1)         // gutartiger Lesefehler 0 mit zerstoerter Opferzelle
                F4) 0 -> L -> (1, 0, 0)         // Ninjalesefehler 0
                F5) 0 -> L -> (1, 0, 1)         // zerstoerender Lesefehler 0
                F6) 0 -> L -> (1, 1, 0)         // Ninjalesefehler 0 mit zerstoerter Opferzelle
                F7) 0 -> L -> (1, 1, 1)         // zerstoerender Lesefehler 0 mit zerstoerter Opferzelle

                F8) 1 -> L -> (0, 0, 0)         // zerstoerender Lesefehler 1
                F9) 1 -> L -> (0, 0, 1)         // Ninjalesefehler 1
                FA) 1 -> L -> (0, 1, 0)         // zerstoerender Lesefehler 1 mit zerstoerter Opferzelle
                FB) 1 -> L -> (0, 1, 1)         // Ninjalesefehler 1 mit zerstoerter Opferzelle
                FC) 1 -> L -> (1, 0, 0)         // Lesefehler 1
                FD) 1 -> L -> (1, 0, 1)         // 1 Lesen erfolgreich
                FE) 1 -> L -> (1, 1, 0)         // Lesefehler 1 mit zerstoerter Opferzelle
                FF) 1 -> L -> (1, 1, 1)         // 1 Lesen erfolgreich mit zerstoerter Opferzelle

                FG) 0 -> 1 S -> (0, 0)          // Schreibfehler 01
                FH) 0 -> 1 S -> (0, 1)          // Schreibfehler 01 mit zerstoerter Opferzelle
                FI) 0 -> 1 S -> (1, 0)          // Schreiben 01 erfolgreich
                FJ) 0 -> 1 S -> (1, 1)          // Schreiben 01 erfolgreich mit zerstoerter Opferzelle


                FK) 1 -> 0 S -> (0, 0)          // Schreiben 10 erfolgreich
                FL) 1 -> 0 S -> (0, 1)          // Schreiben 10 erfolgreich mit zerstoerter Opferzelle
                FM) 1 -> 0 S -> (1, 0)          // Schreibfehler 10
                FN) 1 -> 0 S -> (1, 1)          // Schreibfehler 10 mit zerstoerter Opferzelle

                FO) 0 -> 0 S -> (0, 0)          // Schreiben 00 erfolgreich
                FP) 0 -> 0 S -> (0, 1)          // Schreiben 00 erfolgreich mit zerstoerter Opferzelle
                FQ) 0 -> 0 S -> (1, 0)          // Schreibfehler 00
                FR) 0 -> 0 S -> (1, 1)          // mit zerstoerter Opferzelle

                FS) 1 -> 1 S -> (0, 0)          // Schreibfehler 11
                FT) 1 -> 1 S -> (0, 1)          // Schreibfehler 11 mit zerstoerter Opferzelle
                FU) 1 -> 1 S -> (1, 0)          // Schreiben 11 erfolgreich
                FV) 1 -> 1 S -> (1, 1)          // Schreiben 11 erfolgreich mit zerstoerter Opferzelle

                Zusammenfassend gibt es also:
                Erfolgreiches Lesen             - es wird der korrekte Wert zurueckgegeben und der Wert der Zelle wird NICHT veraendert
                Gutartige Lesefehler            - es wird zwar der falsche Wert zurueckgegeben, aber der Wert der Zelle wird nicht veraendert
                Zerstoerende Lesefehler         - es wird der falsche Wert zurueckgegeben und der Wert der Zelle wird veraendert
                Ninjalesefehler                 - es wird der korrekte (!) Wert zurueckgegeben, aber der Wert der Zelle wird veraendert
                Erfolgreiches Schreiben         - die Zelle hat nach dem Schreiben den korrekten Wert
                Schreibfehler                   - die Zelle hat nach dem Schreiben einen fehlerhaften Wert
                ----------
                mit/ohne zerstörter Opferzelle  - alle o.g. Vorkommnisse koennen eine Opferzelle zerstoeren oder nicht

                Mehrere Opferzellen
                *******************
                Es ist denkbar, dass eine Taeterzelle mehrere Opferzellen betrifft. Wenn ein Algorithmus in der Lage ist
                einzelne Opferzellen zu finden, wird er aber auch immer die mit mehreren Opferzellen finden. Deshalb
                werden diese Faelle hier nicht weiter betrachtet.


                Komplexe Fehlerarten
                *********************
                Hierbei handelt es sich um die Kombination mehrer Aktionen auf eine oder mehrere Zellen. Außerdem koennen
                diese Kombinationen noch zeitabhaengig sein.

                Kombinationsfehler: Es müssen mehrere Aktionen auf eine Zelle ausgeführt werden, um einen Fehler hervor zu rufen
                    Beispiel: Erst 0 Lesen, dann 1 Schreiben. Oder erst 0 Lesen, dann noch einmal lesen und anschließend 0 schreiben
                Dynamische Kombinationsfehler: Es müssen mehrere Aktionen in einer zeitlichen vorgabe ausgeführt werden
                    Beispiel: Erst 0 Lesen, dann 5 ms warten und dann noch einmal lesen
                Multizellenkombinationsfehler: Eine Kombination von Aktionen auf mehrere Zellen
                    Beispiel: Zelle A 0 lesen, Zelle B 10 Schreiben, Zelle C ist nun fehlerhaft
                dynamische Multizellenkombinationsfehler: Eine Kombination von Aktionen auf mehrere Zellen mit zeitlicher Abhaengigkeit
                    Beispiel: Zelle A 0 lesen, 5 ms warten, Zelle B 10 Schreiben, 3 ms warten Zelle C lesen, Zelle D ist nun fehlerhaft

                Bei den komplexen Fehlerarten explodieren die Moeglichkeiten. Wie im obigen Kapitel zu erkennen ist gibt es fuer
                Primitive Fehler mit Opferzellen 32 moegliche Operationen. Wenn man die Kombinationsfehler betrachten wollte
                ergäbe dies fuer 2er-Kombinationen 32 * 32 = 1024 Moeglichkeiten. Sollten drei Operatioen in Folge noetig sein
                waeren es 32*32*32 = 32768 Moeglichkeiten.

                Bei dynamische Kombinationsfehler waechst die theoretische Anzahl sogar ins Unendliche, da Zeit beliebig lange
                oder kurze Zeitspannen moeglich sind.

                Fuer Multizellenkombinationsfehler waechst die Zahl der Moeglichkeiten sogar nich schneller.

                Der Testaufwand ist fuer Primitive Fehler mit Opferzellen bereits ein Problem. Ausserdem ist unklar,
                ob ueberhaupt alle Primitiven Fehler mit Opferzellen praktisch vorkommen koennen. Fuer die komplexen
                Fehlerarten ist die Machbarkeit also noch schwieriger und es ist noch unklarer, ob sie praktisch ueberhaupt
                vorkommen koennen.

                Es erscheint also massiv aufwaendig die komplexen Fehlerarten zu pruefen und gleichzeit ist es fragwuerdig,
                ob diese Fehler ueberhaupt vorkommen koennen. Von einer weiteren Untersuchung dieser Fehlerarten wird daher
                an dieser Stelle abgesehen. Ein Spezialfall der dynamischen Fehler soll allerdings doch betrachtet werden,
                da sie moeglich erscheinen und vermutlich leicht zu entdecken sind, die Haltefehler.

                Die komplexen Fehlerarten sollen jedoch nicht komplett missachtet werden.
                Sobald ein Algorithmus entwickelt und verstanden wurde der fuer Primitive Fehler mit Opferzellen und
                Haltefehler wirksam ist, kann mit dem dadurch gewonnen Wissen die Komplexen Fehlerarten noch einmal betrachtet
                werden, um festzustellen, ob hier noch Fehlerarten abgedeckt werden koennen oder muessen. Bis dahin
                sollen die Fehlerarten behandelt werden, die definitv wichtig sind, naemlich die Primitive Fehler mit Opferzellen
                und Haltefehler.

                Haltefehler
                ***********
                Speicher wird mit einem Wert beschrieben und nach einer Zeit x kippt der Wert.

                Oszillierende Haltefehler
                **************************
                Speicher wird mit einem Wert beschrieben und nach einer Zeit x kippt der Wert. Nach einer Zeit
                y kippt der Wert erneut. Der Speicher wechselt nun alle x und y Zeiteinheiten seinen Wert.

                Zufaellige Bitkipper
                ********************
                Es ist moeglich, dass ein Speicher einwandfrei arbeitet, aber durch Strahlung kippt einmalig ein Bit von
                0 auf 1 oder umgekehrt. Dies kann auch fuer mehrere Bits gleichzeitig geschehen.
                Dieses zufaellige Bitkippen kann zu einem Schaden am Speicher fuehren, muss aber nicht.
                Falls der Speicher dadurch nicht beschädigt sondern nur der Inhalt gekippt wird, wird ein Speichertest
                dabei keinen Fehler feststellen. Die Integritaet der Daten muss also auf einer anderen Ebene gesichert
                werden.


                Moegliche Fehlerursachen
                *************************
                Strahlung kann einen Chip zerstoeren, von der NASA sind Rohrfoermige Durchschlaege von 1 um Durchmesser durch
                ganze Geraete bekannt.

                Strahlung kann ein Bit kippen lassen.

                Leitungsfehler und EMV:
                    Kurzschluss nach Masse, Kurzschluss nach Ground, Kurzschluss zu einer anderen Leitung,
                    EMV Einstrahlung (Schweissgeraete, Uebersprechen von anderen Leitungen oder Leitungsgebunden, zB aus der
                                     Spannungsversorgung)
                    Gilt fuer: Busleitungen, Adressleitungen, Kontrollleitungen, Serielle Busleitungen

                Alterung
                    Das Speichern und Lesen beruht auf physikalischen Vorgaengen in Bauteilen. Materialien veraendern mit
                    der Zeit ihre Eigenschaften. Dies kann zu Fehlverhalten fuehren

                Zu haeufiges Schreiben
                    EEPROM, Flash und aehnliche Speicher koennen nur eine gewisse Anzahl an Schreibvorgaengen leisten,
                    bevor sie defekt werden.

    Beschreibung des Algorithmus smartmemorytest
    *********************************************
    Dieser Algorithmus basiert auf den Ausfuehrungen von Michael Barr unter [1]. Die Grundidee ist dabei, Fehler im
    Verhalten von Datenbus, Adressbus und dem Speicherchip an sich getrennt zu suchen. Dazu besteht der Test aus drei Stufen,
    die hier einzeln beschrieben werden.

        Datenbustest
        ************
        Wenn man in der Lage ist jedes Bit des Datenbusses auf 1 oder 0 zu setzen, ohne die anderen Leitungen zu
        beeinflussen, funktioniert der Datenbus.
        Dazu wird ein wechselndes Datenwort immer wieder an die selbe Adresse geschrieben und anschliessend gelesen.
        Als wechselndes Datenwort wird eine 1 von rechts nach links durch das Wort geschoben. Beispiel fuer ein 8 Bit
        breiten Datenbus:

            Testdatum
            ---------
            0000 0000
            0000 0001
            0000 0010
            0000 0100
            0000 1000
            0001 0000
            0010 0000
            0100 0000
            1000 0000

        Mit diesem Test werden erst einmal von den obigen Fehlermodellen F0 bis FV keine allgemeingueltig getestet,
        denn es wird nur eine Aussage ueber eine einzelne Zelle getroffen werden, fuer die restlichen nicht und
        es wird auch keine Opferzelle betrachtet.

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            ***************************************************
            9 Schreib- und Lesezugriffe = 45 ms


        Adressbustest
        *************
        Hierbei wird ein Wort an verschiedene Adressen geschrieben. Auch hierbei wird davon ausgegangen, dass wenn
        sich jede Adressleitung auch 0 oder 1 setzen laesst, ohne die anderen Leitungen zu beinflussen, dass dann
        der Adressbus funktioniert.

        Beispielsweise wird zuerst das niederwertigste Bit des Adressbusses gesetzt, indem an Adresse 1 ein Byte geschrieben
        wird.
        0000 0001

        Es koennte nun sein, dass eine andere Adressleitung ebenfalls auf 1 gesetzt wird. Daher muessen beim
        Test folgende Adressen geprueft werden:
        0000 0011
        0000 0101
        0000 1001
        0001 0001
        0010 0001
        0100 0001
        1000 0001

        Natuerlich ist es noch moeglich, dass auch gleichzeitig noch eine zweite Adressleitung kippt, oder eine dritte,
        oder vierte und so weiter, bis moeglichweise alle Leitungen gleichzeitig 1 sind. Will man dies erkennen,
        wird es noetig nach einem Schreibvorgang den gesamten Speicher auszulesen. Unter [1] wird dies nicht gemacht,
        hier soll es aber geschehen.

        Anschliessend muss noch geprueft werden, ob sich die Adressleitungen unabhaengig auf 0 setzen lassen.
        Dazu wird der o.g. Vorgang mit invertierten Adressen wiederholt.

        Der Algorithmus muss also wie folgt aussehen:

            1.) Den gesamten Speicher mit 0xAA beschreiben (Hintergrund)
            2.) An Adresse 0000 0001 eine 0x55 schreiben
            3.) Den gesamten Speicher auslesen und pruefen
            4.) An Adresse 0000 0001 wieder eine 0xAA schreiben
            5.) Die Adresse um 1 nach links schieben und die Schritte 2 bis 4 wiederholen
            6.) Schritt 5 wiederholen, bis die Adresse 0 ist

            7.) Den oben genannten Vorgang fuer die folgenden Adressen wiederholen:
                1111 1111

                1111 1110
                1111 1101
                1111 1011
                1111 0111

                1110 1111
                1101 1111
                1011 1111
                0111 1111

                0000 0000

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            **************************************************
            256 Schreibvorgaenge fuer den Hintergrund (immer ganze Pages schreiben)
            18 Schreibvorgaenge
            256 * 18 Lesevorgaenge (es kann immer eine ganze Page gelesen werden)

            => 24 s

            Diese Laufzeit ist fuer sicherheitskritische Anwendungen zu hoch und eignet sich allenfalls als
            Test nach dem Starten des Geraets, der im Hintergrund laeuft. Das Ziel ist aber sein einen
            praktisch einsetzbaren Algorithmus zu finden. Daher wird der Vorschlag aus [1] betrachtet.

        Adressbustest reduzierter Umfang
        ********************************
        Hintergrund: Der gesamte Speicher wird mit 0x55 beschrieben.

        Es werden anschliessend folgende Adressen mit 0xAA beschrieben:

            Testlocation
            ------------
            0000 0000
            0000 0001
            0000 0010
            0000 0100
            0000 1000
            0001 0000
            0010 0000
            0100 0000
            1000 0000

       Nach dem ersten Schreiben werden folgende Speicherzellen auf 0x55 geprueft:
            0000 0010
            0000 0100
            0000 1000
            0001 0000
            0010 0000
            0100 0000
            1000 0000

        Außerdem muss Adresse 0000 0000 den Wert 0xAA enthalten, danach wird sie wieder mit 0x55 beschrieben.


        Nachdem Adresse 0000 0001 mit 0xAA beschrieben wurde, werden folgende Adressen auf 0x55 geprueft:
            0000 0011
            0000 0101
            0000 1001
            0001 0001
            0010 0001
            0100 0001
            1000 0001
        Außerdem muss Adresse 0000 0001 den Wert 0xAA enthalten, danach wird sie wieder mit 0x55 beschrieben.

        Weiter so bis der letzte Schritt ausgefuehrt wurde.

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            **************************************************
            256 Schreibvorgaenge fuer den Hintergrund (immer ganze Pages schreiben)
            17 Schreibvorgaenge fuer die Testlocations (das Beispiel oben ist mit 8 bit, ein 8k EEPROM hat aber einen 16 Bit Adressbus)
            15 * 17 Lesevorgaenge fuer die Nachbaradressen (das Beispiel oben ist mit 8 bit, ein 8k EEPROM hat aber einen 16 Bit Adressbus)

            --> 2,64 s
                ======
        Dies ueberschreitet zwar die oben genannte Forderung von 2 s, ist aber immer noch nah genug dran, um implementiert zu werden.


        Chiptest
        ********
        Hierbei wird an jede Speicherstelle im Chip einmal 0x55 und einmal 0xAA geschrieben und gelesen. Dadurch soll gezeigt
        werden, dass man die Zellen grundsaetzlich ansprechen kann.

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            **************************************************
            2 * 256 Schreibvorgaenge (immer ganze Pages schreiben)
            2 * 256 Lesevorgaenge    (immer ganze Pages lesem)

            --> 5,12 s
                ======


    [1] http://www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/software-based-memory-testing.html




    Read/Writezyklen:
            Wenn ein EEPROM getestet wird, hier ist wie oft dieser Algo den speicher beschreibt:


    Achtung:
            Vor dem starten des Tests muessen die Interrupts ausgestellt werden!
            Ansonsten koennte waehrend des Tests jemand in den Speicher schreiben oder noch schlimmer daraus lesen
            Bzw andere Loesung: waehrend der Test laeuft in einen Schattenspeicher schreiben/lesen

    Zeitverbrauch:
            Dieser Test benoetigt:

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
    uint32_t m_u32StartAdress;          //!< An welcher Adresse im Speicher soll der Test beginnen?
    uint16_t m_u16Blocksize;            //<! Wie gross ist der zu pruefende Speicher? In Byte
    uint32_t m_u32Steps;                //!< hier speichert das Modul wieviele Steps es bereits durchgefuehrt hat
    uint16_t m_u16OldByteNr;            //!< Eine Hilfsvariable um herauszufinden, ob man gerade eine Bytegrenze ueberschritten hat
    uint8_t* m_pu8_Workmem;             //!< Hier wird der Inhalt des zu pruefenden Speichers gerettet, um ihn nach dem Test wieder herstellen zu koennen. Muss genauso gross sein, wie der zu preufende Speicher (vrgl. m_u16Blocksize)
}ts_SMT_ClassStruct;


uint8_t u8_BIR_InitBitrunner(
            ts_SMT_ClassStruct* _pThis,     //!< zeiger auf das struct dass das zu bearbeitende objekt repraesentiert, also die attribute/variablen der klasse
            uint8_t *_pu8_Workmem,          //!< siehe Definition von ts_BIR_ClassStruct
            uint32_t _u32StartAdress,       //!< siehe Definition von ts_BIR_ClassStruct
            uint16_t _u16Blocksize,         //!< siehe Definition von ts_BIR_ClassStruct
            tfp_BR_ReadByte _fpReadByte,    //!< siehe Definition von ts_BIR_ClassStruct
            tfp_BR_WriteByte _fpWriteByte   //!< siehe Definition von ts_BIR_ClassStruct
            );

void v_BIR_PrepareMem(ts_SMT_ClassStruct* _pThis);
void v_BIR_RestoreMem(ts_SMT_ClassStruct* _pThis);


#endif // BITRUNNER_H
