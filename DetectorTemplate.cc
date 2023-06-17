/*
    Author:  Suerfu Burkhant
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/
/// \file DetectorTemplate.cc
/// \brief Main template for Geant4 detector simulation.
/// It provides the framework for 
/// 1) defining and/or adding geometries
/// 2) adding physics lists,
/// 3) generating particles, and
/// 4) accessing and recording hit and trajectory information.

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

//#include "G4ImportanceBiasing.hh"
//#include "G4GeometrySampler.hh"
// Above two header files required only when using geometry importance biasing

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <string>

using std::string;


/// Basic usage of the program.
void PrintUsage();

string GetClassName(){ return "main"; }

int main( int argc, char** argv ){
    
    // Get commandline arguments.
    //
    CommandlineArguments cmdl( argc, argv);
    cmdl.Print();

    // If u or interactive is specified, initialize UI executive and prepare UI session.
    // A non-zero pointer value is later used as a flag.
    //
    G4UIExecutive* ui = 0;
    if( cmdl.Find("u")==true || cmdl.Find("interactive")==true ){
        G4cout << "Entering interactive session..." <<G4endl;
        ui = new G4UIExecutive( argc, argv );
    }

    // If a macro is specified, get the macro name and execute the macro later.
    //
    G4String macroname = cmdl.Get("macro");
    if( macroname=="" ){
        macroname = cmdl.Get("m");
        if( macroname=="" && ui ==0 ){ 
            PrintUsage();
            return -2;
        }
    }

    G4cout << GetClassName() << ": Constructing RunManager..." << G4endl;
    G4RunManager * runManager = new G4RunManager();

    // Construct detector geometry
    // GeometryManager is simply a central place to obtain information regarding the geometries and materials used in this program.
    //
    G4cout << GetClassName() << ": Constructing GeometryManager..." << G4endl;
    GeometryManager* geometryManager = GeometryManager::Get();

    G4cout << GetClassName() << ": Constructing GeometryConstruction..." << G4endl;
    GeometryConstruction* detectorConstruction = new GeometryConstruction( geometryManager );

    G4cout << GetClassName() << ": Setting GeometryConstruction User Initialization..." << G4endl;
    runManager->SetUserInitialization( detectorConstruction );


    // Physics list
    // For now, simply use the shielding physics list. This may be changed in the future for more customization.
    //
    G4cout << GetClassName() << ": Constructing Shielding PhysicsList..." << G4endl;
    G4VModularPhysicsList* physicsList = new Shielding;

/*
    // Configure geometry importance biasing
    G4GeometrySampler geom_sampler_gamma(detectorConstruction->GetWorldPhysical(),"gamma");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_gamma) );
    G4GeometrySampler geom_sampler_e(detectorConstruction->GetWorldPhysical(),"e-");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_e) );
    G4GeometrySampler geom_sampler_ep(detectorConstruction->GetWorldPhysical(),"e+");
    physicsList->RegisterPhysics( new G4ImportanceBiasing(&geom_sampler_ep) );
*/
    // Note below line has to be after setting up biasing.
    G4cout << GetClassName() << ": Setting PhysicsList User Initialization..." << G4endl;
    runManager->SetUserInitialization( physicsList );


    // Run action
    //
    G4cout << GetClassName() << ": Constructing RunAction..." << G4endl;
    RunAction* runAction = new RunAction( &cmdl );

    G4cout << GetClassName() << ": Setting RunAction..." << G4endl;
    runManager->SetUserAction( runAction );


    // Primary generator
    //
    G4cout << GetClassName() << ": Constructing GeneratorAction..." << G4endl;
    GeneratorAction* generatorAction = new GeneratorAction( runAction );
    
    G4cout << GetClassName() << ": Setting GeneratorAction..." << G4endl;
    runManager->SetUserAction( generatorAction );


    // Event action
    //
    G4cout << GetClassName() << ": Constructing EventAction..." << G4endl;
    EventAction* eventAction = new EventAction( runAction );
    
    G4cout << GetClassName() << ": Setting EventAction..." << G4endl;
    runManager->SetUserAction( eventAction );


    // Tracking, stepping and stacking action
    // Enable them only when output is needed.
    // This is checked using RunAction's output filename since TTree and TTile are initialized afterwards.
    //
    //if( runAction->GetOutputFileName()!="" ){
    G4cout << GetClassName() << ": Constructing and setting TrackingAction..." << G4endl;
    runManager->SetUserAction( new TrackingAction( runAction, eventAction ) );
    
    G4cout << GetClassName() << ": Constructing and setting SteppingAction..." << G4endl;
    runManager->SetUserAction( new SteppingAction( runAction, eventAction ) );
    
    G4cout << GetClassName() << ": Constructing and setting StackingAction..." << G4endl;
    runManager->SetUserAction( new StackingAction( runAction, eventAction ) );
    //}

    //runManager->Initialize();
    //  this line should be called within the macro
    //  so that user can pass geometry parameters inside

    //detectorConstruction->CreateImportanceStore();


    // Visualization should be turned on when
    // 1. UI is enabled and 
    // 2. visualization is not explicitly disabled.
    //
    G4VisManager* visManager = 0;
    if( ( (cmdl.Find("vis")==true) || (cmdl.Find("v")==true) ) && ui!=0 ){
        visManager = new G4VisExecutive;
    }


    // Get the pointer to the User Interface manager
    // 
    G4UImanager* UImanager = G4UImanager::GetUIpointer();


    // Process macro or start UI session  
    // Batch mode
    // 
    if ( macroname!="" ){
        runAction->AddMacro( macroname );
        G4String command = "/control/execute ";
        UImanager->ApplyCommand( command+macroname );
    }
    // Interactive mode
    //
    else{
        if( visManager!=0 ){
            visManager->Initialize();
        }
        UImanager->ApplyCommand("/control/execute macros/init_vis.mac");
        if (ui->IsGUI()) {
            UImanager->ApplyCommand("/control/execute macros/gui.mac");
        }
        ui->SessionStart();
    }


    // End of run
    //
    if( ui ){
        delete ui;
    }
    if( visManager ){
        delete visManager;
    }

    // Note that visManager is deleted after UI manager.
    // Otherwise seg fault upon closing GUI.

    delete runManager;
}


void PrintUsage() {
    G4cerr << "\nUsage: executable [-option [argument(s)] ]" << G4endl;
    G4cerr << "\t-m/--macro,       used to specify the macro file to execute.\n";
    G4cerr << "\t-u/--interactive, enter interactive session. In this case, do not use -m/--macro\n";
    G4cerr << "\t-v,--vis,         enable visualization. (disabled by default)\n";
    G4cerr << "\t--seed,           the random seed to be used. (default current time)\n";
    G4cerr << "\t-o/--output,      specify the output file name to which trajectories will be recorded.\n";
    G4cerr << G4endl;
}

