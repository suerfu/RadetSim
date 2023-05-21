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
    
    fGeometryType = 1;
        // Default geometry is type = 0

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


    // ***********************************
	// Rock at Homestake
    // ***********************************
    //
    G4double fracM;
        // Molar fraction
    
    G4int nComponents;

    G4Material* rock = new G4Material("Rock", density= 3.26*g/cm3, nComponents = 12);
        // The density for Homestake formation is taken from:
        //      https://sdaos.org/wp-content/uploads/pdfs/2014/roggenthen%2033-41.pdf
        // The chemical composition is taken from:
        //      https://arxiv.org/pdf/0912.0211.pdf

    G4Element* elSi = new G4Element( "Silicon", symbol = "Si" , z = 14., a = 28.086*g/mole);
    G4Element* elTi = new G4Element( "Titanium", symbol = "Ti" , z = 22., a = 47.867*g/mole);
    G4Element* elAl = new G4Element( "Aluminum", symbol = "Al" , z = 13., a = 26.981*g/mole);
    G4Element* elFe = new G4Element( "Iron", symbol = "Fe" , z = 26. , a = 55.845*g/mole);
    G4Element* elMn = new G4Element( "Manganese", symbol = "Mn" , z = 25. , a = 54.938*g/mole);
    G4Element* elMg = new G4Element( "Magnesium", symbol = "Mg" , z = 12. , a = 24.405*g/mole);
    G4Element* elCa = new G4Element( "Calsium", symbol = "Ca" , z = 20. , a = 40.078*g/mole);
	G4Element* elC  = new G4Element( "Carbon", symbol = "C", z = 6.0, a = 12.011*g/mole);
    G4Element* elNa = new G4Element( "Sodium", symbol = "Na", z = 11., a =  22.990*g/mole);
    G4Element* elK  = new G4Element( "Potassium", symbol = "K",  z = 19., a = 39.098*g/mole);
    G4Element* elP  = new G4Element( "Phosphorus", symbol = "P", z = 15., a = 30.974*g/mole);
    G4Element* elH  = new G4Element( "Hydrogen", symbol = "H", z = 1., a = 1.008*g/mole);
    G4Element* elO  = new G4Element( "Oxygen", symbol = "O", z = 8., a = 15.999*g/mole);

    G4double fracM_O = 0;
        // used to accumulate fractional mass of oxygen in the rock
    G4double fraction = 0;
        // fraction of weight of the composite in the rock.

    // ============ SiO2 =============
    fraction = 0.437;
    fracM = 28.086/(28.086+2*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elSi, fracM * fraction );

    // ============ TiO2 =============
    fraction = 0.0122;
    fracM = 47.867/(47.867+2*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elTi, fracM * fraction );

    // ============ Al2O3 =============
    fraction = 0.136;
    fracM = 2*26.981/(2*26.981+3*15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elAl, fracM * fraction );

    // ============ FeO =============
    fraction = 0.127;
    fracM = 55.845/(55.845+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elFe, fracM * fraction );

    // ============ MnO =============
    fraction = 0.0013;
    fracM = 54.938/(54.938+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elMn, fracM * fraction );

    // ============ MgO =============
    fraction = 0.07;
    fracM = 24.405/(24.405+15.999);
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elMg, fracM * fraction );

    // ============ CaO =============
    fraction = 0.079;
    fracM = 40.078 / ( 40.078 + 15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elCa, fracM * fraction );

    // ============ Na2O =============
    fraction = 0.0287;
    fracM = 2*22.990 / ( 2*22.990 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elNa, fracM * fraction );

    // ============ K2O =============
    fraction = 0.0021;
    fracM = 2*39.098 / ( 2*39.098 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elK, fracM * fraction );

    // ============ P2O5 =============
    fraction = 0.0007;
    fracM = 2*30.974 / ( 2*30.974 +5*15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elP, fracM * fraction );

    // ============ H2O =============
    fraction = 0.107;
    fracM = 2*1.008 / ( 2*1.008 +15.999 );
    fracM_O += (1-fracM)*fraction;
    rock->AddElement( elH, fracM * fraction );

    // ============ Oxygen =============
    rock->AddElement( elO, fracM_O );
    materialColor["rock"] = G4Color(G4Colour::Brown());

    G4cout << *( G4Material::GetMaterialTable() ) << G4endl;
}


void GeometryManager::GeometryHasBeenModified(){
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}


void GeometryManager::LoadFile( G4String filename ){
    G4cout << "Loading parameters from " << filename << G4endl;
}

