This repo is used to build a collection of functional safety relevant software modules on embedded devices.

They shall be uasable in real projects. 

Language: C for modules, C++ for unit-tests (QTEST)

Possible modules are:
	ram test
	- Speichertest
	- Schutz vor zufälligen Bitkippern: erkennen 
	(- Schutz vor zufälligen Bitkippern: reparieren)
	
	rom test

	prog mem test

	eeprom test
		- stage 1: find erros -> Bitrunner (failed due to runtime req's)
		- stage 2: find 'em fast enough (< 2s)
		- Schutz vor zufälligen Bitkippern: erkennen 
		- Schutz vor zufälligen Bitkippern: reparieren

	nvm: 
		- Page abstraction 
		- Schutz vor zufälligen Bitkippern: erkennen 
		- Schutz vor zufälligen Bitkippern: reparieren
		




Further requirements: hw-independet, tdd