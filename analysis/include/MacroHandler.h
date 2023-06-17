#ifndef MACROHANDLER_H
#define MACROHANDLER_H 1

#include "TMacro.h"
#include "TFile.h"

#include <string>

using namespace std;

// Obtains a ROOT macro file from the specified ROOT file by name.
//
TMacro GetMacro( string rootName, string macroName);


// Retrieves a ROOT macro file from opened ROOT file pointer
//
TMacro GetMacro( TFile* file, string macroName );


// Returns the (single) float number that contains in the given macro
// Note: the macro should contain only one number in this case
//
double GetFloat( TMacro macro );


// Get the number of particle simulated in this run
//
double GetNbParticleSimulated( TMacro mac );


// Get the mass of the specified volume
// Note: it doesn't deal with volumes with identical names
//
double GetMassByName( TMacro macro, string volName );


// Returns the total mass of a certain material
//
double GetMassByMaterial( TMacro macro, string materialName );


// Returns the total mass based on material
//
//double GetTotalMassByMaterial( TMacro macro, string MaterialName );



#endif
