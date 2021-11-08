/// \file GeometryConstruction.hh
/// \brief Definition of the GeometryConstruction class

#ifndef GeometryConstruction_h
#define GeometryConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "GeometryManager.hh"
#include "RunAction.hh"

#include "SimpleCube.hh"

#include "globals.hh"
//#include "G4RotationMatrix.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;
class GeometryConstructionMessenger;
class G4VIStore;

/// Detector construction class to define materials and geometry.



class GeometryConstruction : public G4VUserDetectorConstruction{

public:

    GeometryConstruction( RunAction* ra, GeometryManager* man );

    virtual ~GeometryConstruction();

public:

    virtual G4VPhysicalVolume* Construct();
        // This method calls DefineMaterials and DefineVolumes successively.

    G4VPhysicalVolume* ConstructWorld();

    void ConstructUserVolumes();

    //G4VIStore* CreateImportanceStore();


private:

    RunAction* fRunAction;
    GeometryConstructionMessenger* fDetectorMessenger;
    GeometryManager* fGeometryManager;
    
    bool fCheckOverlaps;

    G4double world_x;
    G4double world_y;
    G4double world_z;

    SimpleCube* simple_cube;

};


#endif
