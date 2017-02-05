/*
 *  File:   smartmemorytest.h
    Author: Jan Philipp Kiessling

    Anforderungen:
            1.) Das Modul MCM muss einen MarchC- gemaess \research\ch03.pdf S.9 implementieren
            2.) Wenn eine Zelle als fehlerhaft erkannt wird, muss das Modul MCM die Adresse dieser Zelle zurueck geben.
            3.) Das Modul MCM muss objekt orientiert entwickelt werden.
            4.) Das Modul MCM muss nach tdd entwickelt werden.
            5.) Die Unittests fuer das Modul MCM muessen eine branch-Abdeckung von 100 % aufweisen.
            6.) Das Modul MCM muss einem Review unterzogen werden.
            7.) Das Modul MCM muss hardware-unabhaengig sein.



    brief:  long story short: this is a March Implementation. It was intended to be uses as MarchC- but the
            Algo got so abstract that it now can be used for several other March-Checks. The heart is b8_MCM_March()
            where you hand it the direction of the element (bot2top or top2bot) and the action (Read 0 Write 1 or vice versa)
            To keep it hardware-abstract it works with ReadByte and WriteByte functions. You need to supply it with a pointer
            to these functions.

            Why MarchC-? Because the Norms (61508 etc) accept this algo for RAM-Tests. The development started with the idea
            to provide a MemTest for EEPROM, but because of Checksums we dont need that, it takes too much time anyway.
            The following text is a nice small analysis of different techs to do so and memtests in general:



            --------------


            Fuer sicherheitskritische Anwendungen ist es noetig Speicher auf seine korrekte
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
        werden, dass man die Zellen grundsaetzlich ansprechen kann. Bevor der Test beendet wird, muss der letzte Lesevorgang
        fuer den gesamten Chip noch einmal wiederholt werden und die selben Ergebnisse wie zuvor lesen. Dieser Lesevorgang dient
        der Erkennung von Ninjalesefehlern, siehe unten.

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            **************************************************
            2 * 256 Schreibvorgaenge (immer ganze Pages schreiben)
            2 * 256 Lesevorgaenge    (immer ganze Pages lesem)
          + 1 * 256 Lesevorgaenge    (immer ganze Pages lesem)  - dieser Lesevorgang dient der Erkennung von Ninjalesefehlern, siehe unten

            --> 6,4 s
                =====


        Safe and restore
        ****************
        Before the test the data needs to be stored away and afterwards restored.

            Laufzeit fuer ein 8 k EEPROM mit 5 ms Zugriffszeit
            **************************************************
            256 Lesevorgaenge (immer ganze Pages schreiben)
            256 Schreibvorgaenge (immer ganze Pages schreiben)

        --> 2,55 s
            ======


    Bewertung dieses Tests in Hinsicht auf Fehleraufdeckung
    *******************************************************
    Dieser Test orientiert sich nicht an den Fehlerprimitiven, die im Kapitel "Betrachtung der theoretisch moeglichen Fehler"
    vorgestellt und analysiert wurden, sondern an moeglichen Fehlerursachen.

    Das heisst er prueft bestimmte Ursachen, die zu den Fehlerprimitiven fuehren koennen. Theoretisch kann es allerdings noch
    andere Fehlerusachen geben, die zu den selben Fehlerprimitiven fuehren, diese werden dann nicht oder nur zufaellig aufgedeckt.

    Um einer Bewertung naeher zu kommenm, soll vorerst einmal angenomemn werden, es gaebe keine weiteren Fehlerursachen.

        Annahme: keine unbekannten Fehlerursachen
        ******************************************
        In der Zusammenfassung von "Primitive Fehler mit Opferzellen" gibt es eine Auflistung der Fehlerklassen.


        Fehlerklasse                    | mögl.  Ursachen   | Wird erkannt durch    | fuer          | Kommentar
        ---------------------------------------------------------------------------------------------------------------
        Erfolgreiches Lesen             |     n/a           |   Chiptest            | alle zellen
        Gutartige Lesefehler            | AB, DB, Chip      |   ABT, DBT, CT        | alle zellen   | keine Unterscheidung zum Zerstoerenden Lesefehler
        Zerstoerende Lesefehler         | Chip              |   Chiptest            | alle zellen   | keine Unterscheidung zum Gutartigen Lesefehler
        Ninjalesefehler                 | Chip              |   keine Erkennung     |  n/a          |
        Erfolgreiches Schreiben         |     n/a           |   Chiptest            | alle zellen   |
        Schreibfehler                   | AB, DB, Chip      |   ABT, DBT, CT        | alle zellen   |
        ------------------------------- |                   |
        mit/ohne zerstörter Opferzelle  | AB                |   ABT                 | alle Zellen   |


        Erkennung von Ninjalesefehlern
        ******************************
        Wenn man den Algorithmus beim ChipTest um einen weiteren Lesevorgang erweitert, koennen Ninjalesefehler erkannt werden,
        denn: Diese Fehler liefern das korrekte Ergebnis, kippen aber dann den Inhalt der Zelle.
            Zum Beispiel:   Man hat eine Zelle mit diesem Fehler, die Zelle enthaelt eine 1. Wenn man diese Zelle liest erhaelt man
                            eine 1 zurueck und der Inhalt der Zelle ist anschliessend null. Um dieses Kippen zu bemerken,
                            kann man die Zelle einfach noch einmal auslesen.

        Dadurch ergibt sich folgende Aufdeckung:

        Fehlerklasse                    | mögl.  Ursachen   | Wird erkannt durch    | fuer          | Kommentar
        ---------------------------------------------------------------------------------------------------------------
        Erfolgreiches Lesen             |     n/a           |   Chiptest            | alle zellen   |
        Gutartige Lesefehler            | AB, DB, Chip      |   ABT, DBT, CT        | alle zellen   | keine Unterscheidung zum Zerstoerenden Lesefehler
        Zerstoerende Lesefehler         | Chip              |   Chiptest            | alle zellen   | keine Unterscheidung zum Gutartigen Lesefehler
        Ninjalesefehler                 | Chip              |   CT                  | alle zellen   |
        Erfolgreiches Schreiben         |     n/a           |   Chiptest            | alle zellen   |
        Schreibfehler                   | AB, DB, Chip      |   ABT, DBT, CT        | alle zellen   |
        ------------------------------- |                   |                       |               |
        mit/ohne zerstörter Opferzelle  | AB                |   ABT                 | alle Zellen   |

    Wie realistisch ist die Annahme, dass es keine weiteren Fehlerursachen gibt?
    *****************************************************************************
    Im Kapitel "Moegliche Fehlerursachen" wird auf verschiedene Fehlerursachen eingeangen. Diese sind:

        Fehlerursache             |  Führt zu
        ---------------------------------------------------------------------------------------------------
         Zu haeufiges Schreiben   | Chipfehler, nicht mehr schreib/lesbar
         Alterung                 | unbekannte auswirkungen
         Höhen Strahlung          | einmaligen Bitkippern oder Zerstörung von Chip oder Leitungen
         Leitungsfehler           | Adress- / Datenbusfehler
         EMV Einstrahlung         | einmaligen Bitkippern oder Zerstörung von Chip

    Welche Fehlerprimitive durch einen Defekt im Chip ausgeloest werden, kann vermutlich nur in Zusammenarbeit mit dem
    jeweiligen Chiphersteller ermittelt werden. Es ist daher unklar, ob die einfachen Lese-/Schreibzugriffe, die im Chiptest
    verwendet werden, dazu geeignet sind alle möglichen prmitiven Fehler die durch Alterung oder Höhenstrahlung auftreten koennen
    aufzudecken.
    Einmalige Bitkipper koennen ebenfalls nicht detektiert werden, das ist aber wie oben beschrieben durch diese Art von
    Speichertests prinzipiell nicht zu loesen und benoetigt andere Techniken wie z.B. CRC-Pruefsummen oder doppelte Datenhaltung.

    Anmerkung:
    Es koennte im Adressbus Kombinationsfehler geben, also z.B. dass Adressleitung 4 nur dann faelschlicherweise High wird,
    wenn Leitung 1 und 2 auch high sind. Dies wurde unter [1] nicht beachtet.

    Stoppage and background
    ************************
    Der Test muss im Hintergrund laufen koennen.
    Es muss eine Moeglichkeit geben, den Test zum abbruch zu zwingen, damit man beispielsweise
    bei einem brown-out Daten retten kann.


    Zeitverbrauch:
            Dieser Test benoetigt auf einem 8k EEPROM mit 5 ms Zugriffszeit vorraussichtlich 12 s.
            Der Zeitverbrauch entspricht etwa 8*n, wobei n die Anzahl der Bits ist.


    Fazit
    *****
    Die Annahme aus [1], dass mit diesen Tests alle moeglichen Fehlerursachen abgedeckt sind und damit alle moeglichen primitiven
    Fehler gefunden werden koennen muss angezweifelt werden. Fuer Anwendungen mit hoechsten Sicherheitsanforderungen sollte
    daher weiter recherchiert werden, ob mit Hilfe von March Algorithmen nicht doch eine zuverlaessig, ursachenunabhaengige
    Aufdeckung bei akzeptabler Laufzeit erreicht werden kann.
    Recherchen haben ergeben, dass MarchC- etwa 10*n an Laufzeit verbraucht. Wenn man diesen Algorithmus so modifiziert dass
    er zuverlaessig alle oder fast alle Fehlerprimitive aufdeckt, verbraucht er kaum mehr.
    Diese Algorithmen bauen dann nicht darauf auf, die Fehlerursachen festzustellen, sondern pruefen direkt auf die Fehlerprimitive.
    Die Aufdeckungsrate ist daher deutlich zuverlaessiger und da der erwartete Zeitverbraucht kaum hoeher sein sollte,
    soll hier so eine Alternative entwickelt werden.




    Untersuchung MarchC-
    *********************
    Achtung, man braucht garkeinen Memtest fuer EEPROMs. Denn da kann  man immer einfach checksummen nehmen.

    --> MarchC- ramtest implementieren, da dieser ein in den normen anerkannter standard ist

    [1] http://2www.esacademy.com/en/library/technical-articles-and-documents/miscellaneous/software-based-memory-testing.html
*/


#ifndef SMARTMEMTEST_H
#define SMARTMEMTEST_H

#include "stdint.h"
typedef uint8_t (*tfp_BR_ReadByte) (uint32_t const _u32ByteNr);
typedef void    (*tfp_BR_WriteByte)(uint32_t const _u32ByteNr, uint8_t const _u8Data);


typedef enum
{
    eR0W1 = 0,      //!< if you dont know what this is read the document ../research/ch03.pdf
    eR1W0
}te_MarchElementAction;

typedef enum
{
    eBot2Top = 0,   //!< if you dont know what this is read the document ../research/ch03.pdf
    eTop2Bot
}te_MarchElementDirection;

typedef struct
{
    uint8_t* m_pu8Mem;
    uint32_t m_u32MemSize;
    tfp_BR_ReadByte m_fpReadByte;
    tfp_BR_WriteByte m_fpWriteByte;
}ts_MCM_ClassStruct;


uint8_t b8_MCM_Init(ts_MCM_ClassStruct* _pThis,     //!< zeiger auf das struct dass das zu bearbeitende objekt repraesentiert, also die attribute/variablen der klasse
            uint32_t _u32MemSize,
            tfp_BR_ReadByte _fpReadByte,
            tfp_BR_WriteByte _fpWriteByte
            );

uint8_t b8_MCM_Element_Any_W0(ts_MCM_ClassStruct* _pThis);
uint8_t b8_MCM_Element_Any_R0(ts_MCM_ClassStruct* _pThis, uint32_t *_pu32FailedAtByteNr);

uint8_t b8_MCM_March(   ts_MCM_ClassStruct* _pThis,
                        te_MarchElementDirection _eDir,
                        te_MarchElementAction _eAction,
                        uint32_t* _pu32FailedAtByteNr   //!< contains the number of the broken byte, in case sth went wrong, cnt starts at 0
                        );

#endif
