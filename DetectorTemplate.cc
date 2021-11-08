//
/// \file DetectorTemplate.cc
/// \brief Main template for Geant4 detector simulation.
/// It provides framework for 
/// 1) defining and/or adding geometries
/// 2) adding physics lists,
/// 3) generating particles, and
/// 4) accessing hit and trajectory information.

#include "utility.hh"

#include "G4RunManager.hh"

#include "GeometryManager.hh"
#include "GeometryConstruction.hh"
#include "GeneratorAction.hh"

#include "Shielding.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"
#include "StackingAction.hh"

#include "G4UImanager.hh"
#include "G4UIcommand.hh"

#include "G4ImportanceBiasing.hh"
#include "G4GeometrySampler.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Randomize.hh"


/// Basic usage of the program.
void PrintUsage();


int main(int argc,char** argv){

    CommandlineArguments cmdl( argc, argv);
    cmdl.Print();


    // Random engine.
    // The seed is first set by the current time. Later it will be updated by the commandline parameter if provided.
    //
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    G4long seeds[2];
    time_t systime = time(NULL);
    if( cmdl.Find("seed") ){
        seeds[0] = stol( cmdl.Get("seed"));
    }
    else{
        seeds[0] = (long) systime;
        cmdl.Insert("seed", to_string(seeds[0]) );
    }
    seeds[1] = (long) (systime*G4UniformRand());
    G4cout << "Seeds for random generator are " << seeds[0] << ", " << seeds[1] << G4endl;
    G4Random::setTheSeeds(seeds);


    //
    G4String macroname = cmdl.Get("macro");
    if( macroname=="" ){
        macroname = cmdl.Get("m");
        if( macroname==""){
            G4cerr << "Macro file not specified. It should be specified with --macro or -m option.\n";
            G4cerr << "Program usage is:\n";
            PrintUsage();
            return -2;
        }
    }


    G4UIExecutive* ui = 0;
    if( cmdl.Find("u") || cmdl.Find("interactive") ){
        ui = new G4UIExecutive( argc, argv );
    }


    G4RunManager * runManager = new G4RunManager();
    
    // Run action
    //
    RunAction* runAction = new RunAction( &cmdl );
    //runAction->SetOutputFileName( filename );
    //runAction->AddRandomSeeds( seeds, 2);
    runManager->SetUserAction( runAction );


    // Construct detector geometry
    //
    GeometryManager* geometryManager = new GeometryManager();
    GeometryConstruction* detectorConstruction = new GeometryConstruction( runAction, geometryManager );
    runManager->SetUserInitialization( detectorConstruction );


    // Physics list
    //
    G4VModularPhysicsList* physicsList = new Shielding;
/*
    // Configure Biasing
    G4GeometrySampler geom_sampler_gamma(detectorConstruction->GetWorldPhysical(),"gamma");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_gamma) );
    G4GeometrySampler geom_sampler_e(detectorConstruction->GetWorldPhysical(),"e-");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_e) );
    G4GeometrySampler geom_sampler_ep(detectorConstruction->GetWorldPhysical(),"e+");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_ep) );
*/
    runManager->SetUserInitialization( physicsList );


    // Primary generator
    //
    GeneratorAction* generatorAction = new GeneratorAction( runAction );
    runManager->SetUserAction( generatorAction );




    // Event action
    EventAction* eventAction = new EventAction( runAction );
    runManager->SetUserAction( eventAction );

    // Tracking, stepping and stacking action
    runManager->SetUserAction( new TrackingAction( runAction, eventAction ) );
    runManager->SetUserAction( new SteppingAction( runAction, eventAction, detectorConstruction ) );
    runManager->SetUserAction( new StackingAction( runAction, eventAction ) );

    runManager->Initialize();
    //detectorConstruction->CreateImportanceStore();

    G4VisManager* visManager = new G4VisExecutive;


    // Get the pointer to the User Interface manager
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
  
    // Batch mode
    if ( macroname!="" ){
        runAction->AddMacro( macroname );
        G4String command = "/control/execute ";
        UImanager->ApplyCommand( command+macroname );
    }
    // Interactive mode
    else{
        visManager->Initialize();
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        if (ui->IsGUI()) {
            UImanager->ApplyCommand("/control/execute gui.mac");
        }
        ui->SessionStart();
    }

    delete ui;
    delete visManager;
        // Note that visManager is deleted after UI manager.
        // Otherwise seg fault upon closing GUI.

    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    delete runManager;
}



void PrintUsage() {
    G4cerr << "\nUsage: veto [-m macro.mac ] [-u] [-f output.root] [-r seed0 seed1] [-g generator]" << G4endl;
    G4cerr << "\t-m, used to spefify the macro file to execute.\n";
    G4cerr << "\t-u, enter interactive session.\n";
    G4cerr << "\t-f, spefify output ROOT file.\n";
    G4cerr << "\t-r, spefify two random seeds to be used.\n" << G4endl;
}



