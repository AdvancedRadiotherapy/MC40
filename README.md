# MC40 Cyclotron Model

Validated with Geant4.10.1 on Ubuntu 12.04. Tested and working on CentOS7 and SLC6 with Geant4.10.2.p03

Also requires ROOT (https://root.cern.ch)

To build: 
1) source relevant thisroot.sh and geant4make.sh 
2) change to <dir>/bhamBeamline 
3) type 'make'

To run a Bragg peak to test against validation data type

	'bhamBeamline macros/braggpeak.mac'

This will simulate 10k primaries and output a file called rootfiles/braggpeak.root


