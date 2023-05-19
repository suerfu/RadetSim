/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

#include "GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"


GeometryManager* GeometryManager::manager = 0;


GeometryManager* GeometryManager::Get(){
    return GetGeometryManager();
}

GeometryManager* GeometryManager::GetGeometryManager(){
    if (!manager){
        manager = new GeometryManager();
    }
    return manager;
}


GeometryManager::GeometryManager(){
    material_manager = GetMaterialManager();
    DefineMaterials();
}


GeometryManager::~GeometryManager(){
    //parameters.clear();
}


G4LogicalVolume* GeometryManager::GetLogicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name )->GetLogicalVolume();
}


G4VPhysicalVolume* GeometryManager::GetPhysicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name );
}


/*
G4double  GeometryManager::GetDimensions(G4String name){
	if( dimensions.find(name)==dimensions.end() ){
		return 0;
	}else{
		return dimensions[name];
	}
}
*/


G4NistManager* GeometryManager::GetMaterialManager(){
    return G4NistManager::Instance();
}


G4Material* GeometryManager::GetMaterial(G4String name){
    return material_manager->FindOrBuildMaterial( name );
}


void  GeometryManager::DefineMaterials( ){

    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons
    G4int natoms;
    G4int ncomp;
    G4double density;
    G4String symbol, name;

    // ***********************************
	// Liquid helium
    // ***********************************
    //
    G4Material* LHe = new G4Material( "LHe", z=2., a= 4.00*g/mole, density= 0.141*g/cm3 );
	materialColor["LHe"] = G4Color( 1, 0.1, 1 );



    // ***********************************
	// Sodium Iodide
    // ***********************************
    //
    G4Material* NaI = new G4Material("NaI", density = 3.67*g/cm3, ncomp=2);
    G4Element* Na = new G4Element( "Sodium", "Na", z = 11., a = 23 * g/mole );
    G4Element* I = new G4Element( "Iodine", "I", z = 53., a = 127 * g/mole );
    NaI->AddElement( Na, natoms = 1 );
    NaI->AddElement( I, natoms = 1 );
	materialColor["NaI"] = G4Color( 0.1, 0.1, 1 );

    // ***********************************
	// SS 304
    // ***********************************
    //
    G4Material* SS = new G4Material("SS", density = 7.8*g/cm3, ncomp=4 );
    G4Element* Mn = new G4Element( "Manganese", "Mn", z = 25.,  a = 54.9 * g/mole );
    G4Element* Cr = new G4Element( "Chromium", "Cr", z = 24., a = 52.0 * g/mole );
    G4Element* Ni = new G4Element( "Nickel", "Ni", z = 28., a = 58.7 * g/mole );
    G4Element* Fe = new G4Element( "Iron", "Fe", z = 26., a = 55.8 * g/mole );
    SS->AddElement( Fe, 0.70 );
    SS->AddElement( Mn, 0.02 ); //mass fraction
	SS->AddElement( Cr, 0.18 );
	SS->AddElement( Ni, 0.10 );
	materialColor["SS"] = G4Color( 0.5, 0.5, 0.9 );

    // ***********************************
    // Copper
    // ***********************************
    //
    G4Material* Cu = new G4Material( "Cu", density = 8.96*g/cm3, ncomp=1);
    G4Element* CuElement = new G4Element( "Copper", "Cu", z = 29.,  a = 63.5 * g/mole );
    Cu->AddElement(CuElement, natoms = 1);
	materialColor["Cu"] = G4Color( 1, 0.1, 0.1 );

    // ***********************************
    // Pure Copper (as an example of two different materials)
    // ***********************************
    //
	G4Material* PureCu = new G4Material( "PureCu", density = 8.96*g/cm3, ncomp=1);
	PureCu->AddElement( CuElement, natoms = 1);
    materialColor["PureCu"] = G4Color( 1, 0.1, 0.1 );

    // ***********************************
    // Tungsten from Geant4 NIST Material
    // ***********************************
    //

    material_manager->FindOrBuildMaterial( "G4_W" );
    materialColor["G4_W"] = G4Color( 0.5, 0.5, 0.9);

    G4cout << *( G4Material::GetMaterialTable() ) << G4endl;
}


void GeometryManager::GeometryHasBeenModified(){
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}


void GeometryManager::LoadFile( G4String filename ){
    G4cout << "Loading parameters from " << filename << G4endl;
}

