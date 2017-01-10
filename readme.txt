/******************************************************************************
Copyright (c) 2017 Jan Philipp Kießling

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

This repo is used to build a collection of functional safety relevant software modules on embedded devices. They shall be uasable in real projects. 


IF YOU WANT TO DEVELOP SAFETY CRITICAL SOFTWARE YOU NEED THE HELP OF A PROFESSIONAL ENGINEER. 
One intention of this project is to spread high quality safety relevant algorithms. Yet, the application in any project needs at least one professional developer that knows what he's doing. 


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