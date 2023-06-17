#ifndef MACROHANDLER_H
#define MACROHANDLER_H 1

#include "TMacro.h"
#include "TFile.h"

#include <string>

using namespace std;

// Obtains a ROOT macro file from the specified ROOT file by name.
//
TMacro GetMacro( string rootName, string macroName);

TMacro GetMacro( TFile* file, string macroName );

// Get the number of particle simulated in this run
//
double GetNbParticleSimulated( TMacro mac );


// Returns the total mass of a certain material
double GetMassByMaterial( TMacro macro, string materialName );


// Returns the total mass based on material
//
double GetTotalMassByMaterial( TMacro macro, string MaterialName );



#endif
