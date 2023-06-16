
#include "MacroHandler.h"

#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TKey.h"

#include <sstream>
#include <iostream>


// Iterate over the list of objects until you find a matching name.
// input is the name of the ROOT file and name is the Macro name
//
TMacro GetMacro( string fileName, string macroName ){

    TFile* file = TFile::Open( fileName.c_str(), "READ");
    TMacro mac = GetMacro( file, macroName);
    file->Close();
    return mac;
}


// Variation that works directly on the ROOT file
//
TMacro GetMacro( TFile* file, string macroName ){

    TIter next( file->GetListOfKeys() );
    TString mac_name;
    TKey *key;

    while( (key=(TKey*)next()) ){

        mac_name = key->GetName();

        if ( mac_name.EqualTo( macroName.c_str() ) ){
            TMacro mac( *((TMacro*)file->Get( mac_name )) );
            return mac;
        }
    }
    std::cerr << "Failed to get macro " << macroName << endl;
    return TMacro();
}


// Warning: this function needs improvements
// At the current state, it does not deal with duplicate macro lines or lines starting with comment #
// Since GetLineWith will return the first line that contains the pattern.

double GetNbParticleSimulated( TMacro mac ){

    string target( mac.GetLineWith( "/run/beamOn" )->String() );
    stringstream ss( target );

    string foo;
    double NbParticle = -1;

    ss >> foo >> NbParticle;

    return NbParticle;
}


double GetMassByMaterial( TMacro macro, string name ){

    double mass = 0;

    TList* maclist = macro.GetListOfLines();
    TIter itr(maclist);

    for( itr=maclist->begin(); itr!=maclist->end(); itr() ){

        TObject* obj = *itr;

        string line( obj->GetName() );
        stringstream ss( line );

        string vol, mat;
        double mass_indiv;

        ss >> vol >> mass_indiv >> mat;

        if( mat==name ){
            mass += mass_indiv;
        }
    }

    return mass;

}
