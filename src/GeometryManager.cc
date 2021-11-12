
#include "GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"


GeometryManager::GeometryManager(){
    material_man = GetMaterialManager();
    DefineMaterials();
}


GeometryManager::~GeometryManager(){
    //dictionary.clear();
}

/*
void GeometryManager::Add( G4String name, G4LogicalVolume* log, G4VPhysicalVolume* phys){
    dictionary[name] = std::make_pair( log, phys);
}
*/


G4LogicalVolume* GeometryManager::GetLogicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name )->GetLogicalVolume();
    /*
    if( dictionary.find(name)==dictionary.end() ){
        return 0;
    }
    else{
        return dictionary[name].first;
    }
    */
}


G4VPhysicalVolume* GeometryManager::GetPhysicalVolume( G4String name ){
    return G4PhysicalVolumeStore::GetInstance()->GetVolume( name );
    /*
    if( dictionary.find(name)==dictionary.end() ){
        return 0;
    }
    else{
        return dictionary[name].second;
    }
    */
}


G4NistManager* GeometryManager::GetMaterialManager(){
    return G4NistManager::Instance();
}


G4Material* GeometryManager::GetMaterial(G4String name){
    return material_man->FindOrBuildMaterial( name );
}


void  GeometryManager::DefineMaterials( ){

    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons
    G4int natoms;
    G4int ncomp;
    G4double density;
    G4String symbol, name;

    G4Material* LHe = new G4Material("LHe", z=2., a= 4.00*g/mole, density= 0.141*g/cm3);

    G4Material* NaI = new G4Material("NaI", density = 3.67*g/cm3, ncomp=2);
    G4Element* Na = new G4Element( "Sodium", "Na", z = 11., a = 23 * g/mole );
    G4Element* I = new G4Element( "Iodine", "I", z = 53., a = 127 * g/mole );
    NaI->AddElement(Na, natoms = 1);
    NaI->AddElement( I, natoms = 1);

    G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    material_man->FindOrBuildMaterial( "G4_W" );
}


void GeometryManager::GeometryHasBeenModified(){
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}
