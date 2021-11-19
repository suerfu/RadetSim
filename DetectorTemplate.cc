/*
    Author:  Burkhant Suerfu
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


/// Basic usage of the program.
void PrintUsage();


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
            G4cerr << "Macro file not specified. It should be specified with --macro or -m option.\n";
            G4cerr << "Program usage is:\n";
            PrintUsage();
            return -2;
        }
    }


    G4RunManager * runManager = new G4RunManager();


    // Construct detector geometry
    // GeometryManager is simply a central place to obtain information regarding the geometries and materials used in this program.
    //
    GeometryManager* geometryManager = new GeometryManager();
    GeometryConstruction* detectorConstruction = new GeometryConstruction( geometryManager );
    runManager->SetUserInitialization( detectorConstruction );


    // Physics list
    // For now, simply use the shielding physics list. This may be changed in the future for more customization.
    //
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
    runManager->SetUserInitialization( physicsList );


    // Run action
    //
    RunAction* runAction = new RunAction( &cmdl );
    runManager->SetUserAction( runAction );


    // Primary generator
    //
    GeneratorAction* generatorAction = new GeneratorAction( runAction, geometryManager );
    runManager->SetUserAction( generatorAction );


    // Event action
    //
    EventAction* eventAction = new EventAction( runAction );
    runManager->SetUserAction( eventAction );


    // Tracking, stepping and stacking action
    //
    runManager->SetUserAction( new TrackingAction( runAction, eventAction ) );
    runManager->SetUserAction( new SteppingAction( runAction, eventAction, detectorConstruction ) );
    runManager->SetUserAction( new StackingAction( runAction, eventAction ) );


    runManager->Initialize();
    //detectorConstruction->CreateImportanceStore();


    // Visualization should be turned on when
    // 1. UI is enabled and 
    // 2. visualization is not explicitly disabled.
    //
    G4VisManager* visManager = 0;
    if( cmdl.Find("V")==false && ui!=0 ){
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
    G4cerr << "\t-m/--macro,       used to spefify the macro file to execute.\n";
    G4cerr << "\t-u/--interactive, enter interactive session.\n";
    G4cerr << "\t-V,               disable visualization.\n";
    G4cerr << "\t--seed,           the random seed to be used. If unspecified, current time will be used..\n";
    G4cerr << "\t-o/--output       specify the output file name to which trajectories will be recorded.\n";
    G4cerr << G4endl;
}



