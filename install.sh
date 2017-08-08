#!/bin/bash
cc  Geant4phspMerger.cc iaea_header.cc iaea_phsp.cc iaea_record.cc utilities.cc -lm -lstdc++ -o  Geant4phspMerger
sudo cp  ./Geant4phspMerger  /usr/local/bin/   



